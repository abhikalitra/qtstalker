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


#include "TLine.h"
#include "PrefDialog.h"
#include "Config.h"
#include "CODataBase.h"

#include "../pics/trend.xpm"

#include <QObject>
#include <QPainter>

TLine::TLine ()
{
  _plugin = "TLine";
  _price = 0;
  _price2 = 0;
  
  _fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");

  Config config;
  config.getData(Config::DefaultTLineColor, _color);
  if (! _color.isValid())
  {
    _color.setNamedColor("red");
    config.setData(Config::DefaultTLineColor, _color);
  }

  QString s;
  config.getData(Config::DefaultTLineExtend, s);
  if (s.isEmpty())
  {
    _extend = 1;
    config.setData(Config::DefaultTLineExtend, _extend);
  }
  else
    _extend = s.toInt();
}

void TLine::draw (PlotData &pd, DateBar &dateBars, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = dateBars.getX(_date);
  if (x2 == -1)
    return;

  int x = (x2 * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
  if (x == -1)
    return;

  x2 = dateBars.getX(_date2);
  if (x2 == -1)
    return;

  x2 = (x2 * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
  if (x2 == -1)
    return;

  int y = scaler.convertToY(_price);
  int y2 = scaler.convertToY(_price2);

  painter.setPen(_color);

  painter.drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (_extend)
  {
    int ydiff = y - y2;
    int xdiff = x2 - x;
    if (xdiff > 0)
    {
      while (x2 < pd.buffer.width())
      {
        x = x2;
        y = y2;
        x2 = x2 + xdiff;
        y2 = y2 - ydiff;
        painter.drawLine (x, y, x2, y2);
      }
    }
  }

  // store the selectable area the line occupies
  clearSelectionArea();
  QPolygon array;
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));

  if (_selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(tx,
		  ty - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(tx,
		     ty - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);

    setGrabHandle(new QRegion(tx2,
		  ty2 - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(tx2,
		     ty2 - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);
  }

  painter.end();
}

void TLine::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = "TLine";
  info->setData(k, d);

  k = QObject::tr("Start Date");
  d = _date.toString(Qt::ISODate);
  info->setData(k, d);

  k = QObject::tr("Start Price");
  d = QString::number(_price);
  info->setData(k, d);

  k = QObject::tr("End Date");
  d = _date2.toString(Qt::ISODate);
  info->setData(k, d);

  k = QObject::tr("End Price");
  d = QString::number(_price2);
  info->setData(k, d);
}

void TLine::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit TLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, _color);

  s = QObject::tr("Start Price");
  dialog->addDoubleItem(pid++, page, s, _price);

  s = QObject::tr("End Price");
  dialog->addDoubleItem(pid++, page, s, _price2);

  s = QObject::tr("Extend");
  dialog->addCheckItem(pid++, page, s, _extend);

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
  _price2 = dialog->getDouble(pid++);
  _extend = dialog->getCheck(pid++);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultTLineColor, _color);
    config.setData((int) Config::DefaultTLineExtend, _extend);
  }

  _saveFlag = TRUE;
  
  delete dialog;
}

void TLine::load (QSqlQuery &q)
{
  _id = q.value(0).toInt();
  _exchange = q.value(1).toString();
  _symbol = q.value(2).toString();
  _indicator = q.value(3).toString();
  _color.setNamedColor(q.value(5).toString()); // t1 field
  _date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate); // t2 field
  _date2 = QDateTime::fromString(q.value(7).toString(), Qt::ISODate); // t3 field
  _extend = q.value(15).toInt(); // i1 field
  _price = q.value(25).toDouble(); // d1 field
  _price2 = q.value(26).toDouble(); // d2 field
}

void TLine::save ()
{
  if (! _saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,t2,t3,d1,d2,i1) VALUES (";
  s.append(QString::number(_id));
  s.append(",'" + _exchange + "'");
  s.append(",'" + _symbol + "'");
  s.append(",'" + _indicator + "'");
  s.append(",'" + _plugin + "'");
  s.append(",'" + _color.name() + "'");
  s.append(",'" + _date.toString(Qt::ISODate) + "'");
  s.append(",'" + _date2.toString(Qt::ISODate) + "'");
  s.append("," + QString::number(_price));
  s.append("," + QString::number(_price2));
  s.append("," + QString::number(_extend));
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  _saveFlag = FALSE;
}

void TLine::create ()
{
  emit signalMessage(QString(QObject::tr("Select TLine starting point...")));
}

int TLine::create2 (QDateTime &x, double y)
{
  _date = x;
  _date2 = x;
  _price = y;
  _price2 = y;

  emit signalMessage(QString(QObject::tr("Select TLine ending point...")));

  return 1;
}

int TLine::create3 (QDateTime &x, double y)
{
  if (x < _date)
    return 1;

  _date2 = x;
  _price2 = y;
  _saveFlag = TRUE;
  
  emit signalMessage(QString());

  return 0;
}

void TLine::moving (QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // first point move
    {
      if (x > _date2)
        return;
      
      _date = x;
      _price = y;
      _saveFlag = TRUE;
      
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
    case 2: // second point move
    {
      if (x < _date)
        return;
      
      _date2 = x;
      _price2 = y;
      _saveFlag = TRUE;
      
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
    default: // new object moving
    {
      if (x < _date)
	return;
      
      _date2 = x;
      _price2 = y;
      _saveFlag = TRUE;
      
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
  }
}

void TLine::getIcon (QIcon &d)
{
  d = QIcon(trend_xpm);
}

int TLine::getHighLow (double &h, double &l)
{
  h = -99999999.0;
  l = 99999999.0;
  
  if (_price > h)
    h = _price;
  if (_price < l)
    l = _price;

  if (_price2 > h)
    h = _price2;
  if (_price2 < l)
    l = _price2;
  
  return 0;
}

int TLine::inDateRange (QDateTime &startDate, QDateTime &endDate, DateBar &dateBars)
{
  int rc = FALSE;

  QDateTime dt = _date2;
  if (_extend)
    dateBars.getDate(dateBars.count() - 1, dt);
  
  // is start past our end?
  if (startDate > dt)
    return rc;
  
  // is end before our start?
  if (endDate < _date)
    return rc;
  
  if (startDate >= _date && startDate <= dt)
    return TRUE;
  
  if (endDate >= _date && endDate <= dt)
    return TRUE;
  
  return rc;
}

int TLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<PRICE>,<DATE2>,<PRICE2>,<COLOR>
  //  0    1      2      3       4       5        6

  if (l.count() != 7)
  {
    qDebug() << _plugin << "::CUS: invalid parm count" << l.count();
    return 1;
  }

  _date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _date.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid start date" << l[2];
    return 1;
  }

  bool ok;
  _price = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::CUS: invalid start price" << l[3];
    return 1;
  }

  _date2 = QDateTime::fromString(l[4], Qt::ISODate);
  if (! _date2.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid end date" << l[4];
    return 1;
  }

  _price2 = l[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::CUS: invalid end price" << l[5];
    return 1;
  }

  _color.setNamedColor(l[6]);
  if (! _color.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid color" << l[6];
    return 1;
  }

  return 0;
}

