/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */


#include "ChartObjectText.h"
#include "Config.h"
#include "ChartObjectDataBase.h"
#include "ChartObjectTextDialog.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <QtSql>
#include <qwt_plot.h>

ChartObjectText::ChartObjectText ()
{
  _settings.type = (int) ChartObject::_Text;

  Config config;
  config.getData(Config::DefaultChartObjectTextColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::red);
    config.setData(Config::DefaultChartObjectTextColor, _settings.color);
  }

  QString s;
  config.getData(Config::DefaultChartObjectTextFont, s);
  if (s.isEmpty())
  {
    _settings.font = QFont("Helvetica,9,50,0");
    config.setData(Config::DefaultChartObjectTextFont, _settings.font);
  }
  else
    config.getData(Config::DefaultChartObjectTextFont, _settings.font);

  config.getData(Config::DefaultChartObjectTextLabel, _settings.text);
  if (_settings.text.isEmpty())
  {
    _settings.text = "Text";
    config.setData(Config::DefaultChartObjectTextLabel, _settings.text);
  }

  setYAxis(QwtPlot::yRight);
}

int ChartObjectText::rtti () const
{
  return Rtti_PlotUserItem;
}

void ChartObjectText::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings.date));

  int y = yMap.transform(_settings.price);

  p->setPen(_settings.color);

  p->drawText(x, y, _settings.text);

  QFontMetrics fm = p->fontMetrics();
  
  _selectionArea.clear();
  
  _selectionArea.append(QRegion(x,
		                y - fm.height(),
		                fm.width(_settings.text, -1),
		                fm.height(),
		                QRegion::Rectangle));

/*  
  if (_selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - _handleWidth - 1,
		  y - (fm.height() / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x - _handleWidth - 1,
		     y - (fm.height() / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);
  }
*/
}

void ChartObjectText::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("Text"));
  info.setData(QObject::tr("D"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), _settings.date.toString("HH:mm:ss"));
  info.setData(QObject::tr("Price"), _settings.price);
  info.setData(QObject::tr("Text"), _settings.text);
}

ChartObjectDialog * ChartObjectText::dialog ()
{
  ChartObjectTextDialog *dialog = new ChartObjectTextDialog;
  dialog->setSettings(_settings);
  return dialog;
}

void ChartObjectText::highLow (double &h, double &l)
{
  h = _settings.price;
  l = _settings.price;
}

int ChartObjectText::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<PRICE>,<LABEL>,<COLOR>
  //  0    1      2      3       4       5

  if (l.count() != 6)
  {
    qDebug() << "ChartObjectText::CUS: invalid parm count" << l.count();
    return 1;
  }

  _settings.date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectText::CUS: invalid date" << l[2];
    return 1;
  }

  bool ok;
  _settings.price = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectText::CUS: invalid price" << l[3];
    return 1;
  }

  _settings.text = l[4];

  _settings.color.setNamedColor(l[5]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectText::CUS: invalid color" << l[5];
    return 1;
  }

  return 0;
}

