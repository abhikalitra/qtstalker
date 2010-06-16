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


#include "Sell.h"
#include "PrefDialog.h"
#include "Config.h"
#include "CODataBase.h"

#include "../pics/sellarrow.xpm"

#include <QObject>
#include <QPainter>

Sell::Sell ()
{
  _plugin = "Sell";
  _price = 0;

  Config config;
  config.getData(Config::DefaultSellColor, _color);
  if (! _color.isValid())
  {
    _color.setNamedColor("red");
    config.setData(Config::DefaultSellColor, _color);
  }
}

void Sell::draw (PlotData &pd, DateBar &dateBars, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = dateBars.getX(_date);
  if (x2 == -1)
    return;

  int x = (x2 * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
  if (x == -1)
    return;

  int y = scaler.convertToY(_price);

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
	          x + 2, y - 5,
	          x + 2, y - 11,
	          x - 2, y - 11,
	          x - 2, y - 5,
                  x - 5, y - 5);
  painter.setBrush(_color);
  painter.drawPolygon(arrow, Qt::OddEvenFill);

  clearSelectionArea();
  setSelectionArea(new QRegion(arrow));

  if (_selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - (_handleWidth / 2),
		  y - _handleWidth,
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x - (_handleWidth / 2),
		     y + (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);
  }

  painter.end();
}

void Sell::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Sell");
  info->setData(k, d);

  k = QObject::tr("Date");
  d = _date.toString(Qt::ISODate);
  info->setData(k, d);

  k = QObject::tr("Price");
  d = QString::number(_price);
  info->setData(k, d);
}

void Sell::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Sell");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0 ;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, _color);

  s = QObject::tr("Price");
  dialog->addDoubleItem(pid++, page, s, _price);

  int def = FALSE;
  s = QObject::tr("Default");
  dialog->addCheckItem(pid++, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pid = 0;
  dialog->getColor(pid++, _color);
  _price = dialog->getDouble(pid++);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultSellColor, _color);
  }

  _saveFlag = TRUE;
  
  delete dialog;
}

void Sell::load (QSqlQuery &q)
{
  _id = q.value(0).toInt();
  _exchange = q.value(1).toString();
  _symbol = q.value(2).toString();
  _indicator = q.value(3).toString();
  _color.setNamedColor(q.value(5).toString());
  _date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate);
  _price = q.value(25).toDouble();
}

void Sell::save ()
{
  if (! _saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,t2,d1) VALUES (";
  s.append(QString::number(_id));
  s.append(",'" + _exchange + "'");
  s.append(",'" + _symbol + "'");
  s.append(",'" + _indicator + "'");
  s.append(",'" + _plugin + "'");
  s.append(",'" + _color.name() + "'");
  s.append(",'" + _date.toString(Qt::ISODate) + "'");
  s.append("," + QString::number(_price));
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  _saveFlag = FALSE;
}

void Sell::create ()
{
  emit signalMessage(QString(QObject::tr("Select point to place Sell arrow...")));
}

int Sell::create2 (QDateTime &x, double y)
{
  _saveFlag = TRUE;
  _date = x;
  _price = y;
  emit signalMessage(QString());
  return 0;
}

void Sell::moving (QDateTime &x, double y, int)
{
  _saveFlag = TRUE;
  _date = x;
  _price = y;
  
  emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
}

void Sell::getIcon (QIcon &d)
{
  d = QIcon(sellarrow_xpm);
}

int Sell::getHighLow (double &h, double &l)
{
  h = _price;
  l = _price;
  return 0;
}

int Sell::inDateRange (QDateTime &startDate, QDateTime &endDate, DateBar &)
{
  int rc = FALSE;
  if (_date >= startDate && _date <= endDate)
    rc = TRUE;
  
  return rc;
}

int Sell::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<PRICE>,<COLOR>
  //  0    1      2      3       4

  if (l.count() != 5)
  {
    qDebug() << _plugin << "::CUS: invalid parm count" << l.count();
    return 1;
  }

  _date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _date.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid date" << l[2];
    return 1;
  }

  bool ok;
  _price = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::CUS: invalid price" << l[3];
    return 1;
  }

  _color.setNamedColor(l[4]);
  if (! _color.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid color" << l[4];
    return 1;
  }

  return 0;
}

