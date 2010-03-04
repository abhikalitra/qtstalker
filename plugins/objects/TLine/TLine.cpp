/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include "trend.xpm"

#include <QObject>

TLine::TLine ()
{
  fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");
}

void TLine::draw (ChartObject *co, QPixmap &buffer, DateBar &data, int startX, int pixelspace,
		  int startIndex, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  QDateTime date;
  co->getDate(ChartObject::ParmDate, date);
  int x2 = data.getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  QDateTime date2;
  co->getDate(ChartObject::ParmDate2, date2);
  x2 = data.getX(date2);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x2 == -1)
    return;

  double price = co->getDouble(ChartObject::ParmPrice);
  int y = scaler.convertToY(price);
  price = co->getDouble(ChartObject::ParmPrice2);
  int y2 = scaler.convertToY(price);

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  painter.setPen(color);

  painter.drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (co->getInt(ChartObject::ParmExtend))
  {
    int ydiff = y - y2;
    int xdiff = x2 - x;
    if (xdiff > 0)
    {
      while (x2 < buffer.width())
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
  co->clearSelectionArea();
  QPolygon array;
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);
  co->setSelectionArea(new QRegion(array));

  if (co->getInt(ChartObject::ParmSelected))
  {
    co->clearGrabHandles();

    co->setGrabHandle(new QRegion(tx,
		      ty - (handleWidth / 2),
		      handleWidth,
		      handleWidth,
		      QRegion::Rectangle));

    painter.fillRect(tx,
		     ty - (handleWidth / 2),
		     handleWidth,
		     handleWidth,
		     color);

    co->setGrabHandle(new QRegion(tx2,
		      ty2 - (handleWidth / 2),
		      handleWidth,
		      handleWidth,
		      QRegion::Rectangle));

    painter.fillRect(tx2,
		     ty2 - (handleWidth / 2),
		     handleWidth,
		     handleWidth,
		     color);
  }

  painter.end();
}

void TLine::getInfo (ChartObject *co, Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = "TLine";
  info->setData(k, d);

  k = QObject::tr("Start Date");
  co->getData(ChartObject::ParmDate, d);
  info->setData(k, d);

  k = QObject::tr("Start Price");
  co->getData(ChartObject::ParmPrice, d);
  info->setData(k, d);

  k = QObject::tr("End Date");
  co->getData(ChartObject::ParmDate2, d);
  info->setData(k, d);

  k = QObject::tr("End Price");
  co->getData(ChartObject::ParmPrice2, d);
  info->setData(k, d);
}

void TLine::dialog (ChartObject *co)
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit TLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("Start Price");
  double price = co->getDouble(ChartObject::ParmPrice);
  dialog->addDoubleItem(pid++, page, s, price);

  s = QObject::tr("End Price");
  double price2 = co->getDouble(ChartObject::ParmPrice2);
  dialog->addDoubleItem(pid++, page, s, price2);

  s = QObject::tr("Extend");
  int extend = co->getInt(ChartObject::ParmExtend);
  dialog->addCheckItem(pid++, page, s, extend);

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
  
  dialog->getColor(pid++, color);
  co->setData(ChartObject::ParmColor, color);
  
  price = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmPrice, price);
  
  price2 = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmPrice2, price2);
  
  extend = dialog->getCheck(pid++);
  co->setData(ChartObject::ParmExtend, extend);

  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultTLineColor, color);
    config.setBaseData((int) Config::DefaultTLineExtend, extend);
  }

  co->setData(ChartObject::ParmSave, TRUE);
  
  delete dialog;
}

void TLine::setSettings (ChartObject *co, QSqlQuery &q)
{
  QString s = q.value(0).toString();
  co->setData(ChartObject::ParmID, s);
  
  s = q.value(1).toString();
  co->setData(ChartObject::ParmSymbol, s);

  s = q.value(2).toString();
  co->setData(ChartObject::ParmIndicator, s);

  s = q.value(3).toString();
  co->setData(ChartObject::ParmPlugin, s);

  s = q.value(4).toString(); // t1 field
  co->setData(ChartObject::ParmColor, s);

  s = q.value(5).toString(); // t2 field
  co->setData(ChartObject::ParmDate, s);

  s = q.value(6).toString(); // t3 field
  co->setData(ChartObject::ParmDate2, s);

  s = q.value(14).toString(); // i1 field
  co->setData(ChartObject::ParmExtend, s);

  s = q.value(24).toString(); // d1 field
  co->setData(ChartObject::ParmPrice, s);

  s = q.value(25).toString(); // d2 field
  co->setData(ChartObject::ParmPrice2, s);
}

void TLine::getSettings (ChartObject *co, QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,t2,t3,d1,d2,i1) VALUES (";
  
  QString s;
  co->getData(ChartObject::ParmID, s);
  set.append(s);
  
  co->getData(ChartObject::ParmSymbol, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmIndicator, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmPlugin, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmColor, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmDate, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmDate2, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmPrice, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmPrice2, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmExtend, s);
  set.append("," + s);
  
  set.append(")");
}

void TLine::create (ChartObject *co)
{
  QString s = "TLine";
  co->setData(ChartObject::ParmPlugin, s);

  s = "red";
  co->setData(ChartObject::ParmColor, s);

  co->setData(ChartObject::ParmExtend, 0);

  co->setData(ChartObject::ParmPrice, 0);

  co->setData(ChartObject::ParmPrice2, 0);

  s = QObject::tr("Select TLine starting point...");
  co->message(s);
}

int TLine::create2 (ChartObject *co, QDateTime &x, double y)
{
  co->setData(ChartObject::ParmDate, x);
  co->setData(ChartObject::ParmDate2, x);
  co->setData(ChartObject::ParmPrice, y);
  co->setData(ChartObject::ParmPrice2, y);

  QString s = QObject::tr("Select TLine ending point...");
  co->message(s);

  return 1;
}

int TLine::create3 (ChartObject *co, QDateTime &x, double y)
{
  QDateTime date;
  co->getDate(ChartObject::ParmDate, date);
  if (x < date)
    return 1;

  co->setData(ChartObject::ParmDate2, x);
  co->setData(ChartObject::ParmPrice2, y);
  co->setData(ChartObject::ParmSave, TRUE);
  
  return 0;
}

void TLine::moving (ChartObject *co, QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // first point move
    {
      QDateTime date2;
      co->getDate(ChartObject::ParmDate2, date2);
      if (x > date2)
        return;
      
      co->setData(ChartObject::ParmDate, x);
      co->setData(ChartObject::ParmPrice, y);
      co->setData(ChartObject::ParmSave, TRUE);
      
      QString s = x.toString(Qt::ISODate) + " " + QString::number(y);
      co->message(s);
      break;
    }
    case 2: // second point move
    {
      QDateTime date;
      co->getDate(ChartObject::ParmDate, date);
      if (x < date)
        return;
      
      co->setData(ChartObject::ParmDate2, x);
      co->setData(ChartObject::ParmPrice2, y);
      co->setData(ChartObject::ParmSave, TRUE);
      
      QString s = x.toString(Qt::ISODate) + " " + QString::number(y);
      co->message(s);
      break;
    }
    default: // new object moving
    {
      QDateTime date;
      co->getDate(ChartObject::ParmDate, date);
      if (x < date)
	return;
      
      co->setData(ChartObject::ParmDate2, x);
      co->setData(ChartObject::ParmPrice2, y);
      co->setData(ChartObject::ParmSave, TRUE);
      
      QString s = x.toString(Qt::ISODate) + " " + QString::number(y);
      co->message(s);
      break;
    }
  }
}

void TLine::getIcon (QIcon &d)
{
  d = QIcon(trend_xpm);
}

int TLine::getHighLow (ChartObject *co)
{
  double h = -99999999.0;
  double l = 99999999.0;
  
  double price = co->getDouble(ChartObject::ParmPrice);
  double price2 = co->getDouble(ChartObject::ParmPrice2);

  if (price > h)
    h = price;
  if (price < l)
    l = price;

  if (price2 > h)
    h = price2;
  if (price2 < l)
    l = price2;
  
  co->setData(ChartObject::ParmHigh, h);
  co->setData(ChartObject::ParmLow, l);
  
  return 0;
}

int TLine::inDateRange (ChartObject *co, QDateTime &startDate, QDateTime &endDate)
{
  int rc = FALSE;
  QDateTime sd;
  co->getDate(ChartObject::ParmDate, sd);
  QDateTime ed;
  co->getDate(ChartObject::ParmDate2, ed);
  
  // is start past our end?
  if (startDate > ed)
    return rc;
  
  // is end before our start?
  if (endDate < sd)
    return rc;
  
  if (startDate >= sd && startDate <= ed)
    return TRUE;
  
  if (endDate >= sd && endDate <= ed)
    return TRUE;
  
  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

COPlugin * createCOPlugin ()
{
  TLine *o = new TLine;
  return ((COPlugin *) o);
}


