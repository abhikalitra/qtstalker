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
#include "ChartObjectTextDialog.h"
#include "DateScaleDraw.h"
#include "ChartObjectTextDraw.h"

#include <QDebug>

ChartObjectText::ChartObjectText ()
{
  _draw = new ChartObjectTextDraw;

  _settings.type = (int) ChartObject::_Text;

  Config config;
  config.transaction();
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

  config.commit();
}

void ChartObjectText::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("Text"));
  info.setData(QObject::tr("D"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), _settings.date.toString("HH:mm:ss"));
  info.setData(QObject::tr("Price"), _settings.price);
  info.setData(QObject::tr("Text"), _settings.text);
}

int ChartObjectText::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings.date);

  if (x < start || x > end)
    return 0;

  h = _settings.price;
  l = _settings.price;

  return 1;
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

void ChartObjectText::move (QPoint p)
{
  switch (_status)
  {
    case _Move:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      dsd->date(x, _settings.date);

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings.price = map.invTransform((double) p.y());
      
      _draw->setSettings(_settings);

      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectText::click (int button, QPoint p)
{
  switch (_status)
  {
    case _Selected:
    {
      switch (button)
      {
        case Qt::LeftButton:
          if (_draw->isGrabSelected(p))
          {
            _status = _Move;
            emit signalMoveStart(_settings.id);
            _modified = 1;
            return;
          }

          if (! _draw->isSelected(p))
          {
            _status = _None;
            _draw->setSelected(FALSE);
            emit signalUnselected(_settings.id);
            _draw->plot()->replot();
            return;
          }
          break;
        case Qt::RightButton:
          _menu->exec(QCursor::pos());
          break;
        default:
          break;
      }

      break;
    }
    case _Move:
    {
      switch (button)
      {
        case Qt::LeftButton:
          _status = _Selected;
          emit signalMoveEnd(_settings.id);
          return;
        default:
          break;
      }

      break;
    }
    default: // _None
    {
      switch (button)
      {
        case Qt::LeftButton:
          if (_draw->isSelected(p))
          {
            _status = _Selected;
            _draw->setSelected(TRUE);
            emit signalSelected(_settings.id);
            _draw->plot()->replot();
          }
          break;
        default:
          break;
      }

      break;
    }
  }
}

void ChartObjectText::dialog ()
{
  ChartObjectTextDialog *dialog = new ChartObjectTextDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartObjectText::dialog2 (ChartObjectSettings set)
{
  _modified = 1;
  setSettings(set);
  _draw->plot()->replot();
}

void ChartObjectText::create ()
{
  _modified = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings.id);
  emit signalMoveStart(_settings.id);
}

