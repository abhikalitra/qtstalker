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
  plugin = "TLine";
  color.setNamedColor("red");
  price = 0;
  price2 = 0;
  extend = 0;
  
  fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");
}

void TLine::draw (PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = pd.dateBars.getX(date);
  if (x2 == -1)
    return;

  int x = pd.startX + (x2 * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
  if (x == -1)
    return;

  x2 = pd.dateBars.getX(date2);
  if (x2 == -1)
    return;

  x2 = pd.startX + (x2 * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
  if (x2 == -1)
    return;

  int y = pd.scaler.convertToY(price);
  int y2 = pd.scaler.convertToY(price2);

  painter.setPen(color);

  painter.drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (extend)
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

  if (selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(tx,
		  ty - (handleWidth / 2),
		  handleWidth,
		  handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(tx,
		     ty - (handleWidth / 2),
		     handleWidth,
		     handleWidth,
		     color);

    setGrabHandle(new QRegion(tx2,
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

void TLine::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = "TLine";
  info->setData(k, d);

  k = QObject::tr("Start Date");
  d = date.toString(Qt::ISODate);
  info->setData(k, d);

  k = QObject::tr("Start Price");
  d = QString::number(price);
  info->setData(k, d);

  k = QObject::tr("End Date");
  d = date2.toString(Qt::ISODate);
  info->setData(k, d);

  k = QObject::tr("End Price");
  d = QString::number(price2);
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
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("Start Price");
  dialog->addDoubleItem(pid++, page, s, price);

  s = QObject::tr("End Price");
  dialog->addDoubleItem(pid++, page, s, price2);

  s = QObject::tr("Extend");
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
  price = dialog->getDouble(pid++);
  price2 = dialog->getDouble(pid++);
  extend = dialog->getCheck(pid++);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultTLineColor, color);
    config.setBaseData((int) Config::DefaultTLineExtend, extend);
  }

  saveFlag = TRUE;
  
  delete dialog;
}

void TLine::load (QSqlQuery &q)
{
  id = q.value(0).toInt();
  symbol = q.value(1).toString();
  indicator = q.value(2).toString();
  color.setNamedColor(q.value(4).toString()); // t1 field
  date = QDateTime::fromString(q.value(5).toString(), Qt::ISODate); // t2 field
  date2 = QDateTime::fromString(q.value(6).toString(), Qt::ISODate); // t3 field
  extend = q.value(14).toInt(); // i1 field
  price = q.value(24).toDouble(); // d1 field
  price2 = q.value(25).toDouble(); // d2 field
}

void TLine::save ()
{
  if (! saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,t2,t3,d1,d2,i1) VALUES (";
  s.append(QString::number(id));
  s.append(",'" + symbol + "'");
  s.append(",'" + indicator + "'");
  s.append(",'" + plugin + "'");
  s.append(",'" + color.name() + "'");
  s.append(",'" + date.toString(Qt::ISODate) + "'");
  s.append(",'" + date2.toString(Qt::ISODate) + "'");
  s.append("," + QString::number(price));
  s.append("," + QString::number(price2));
  s.append("," + QString::number(extend));
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  saveFlag = FALSE;
}

void TLine::create ()
{
  emit signalMessage(QString(QObject::tr("Select TLine starting point...")));
}

int TLine::create2 (QDateTime &x, double y)
{
  date = x;
  date2 = x;
  price = y;
  price2 = y;

  emit signalMessage(QString(QObject::tr("Select TLine ending point...")));

  return 1;
}

int TLine::create3 (QDateTime &x, double y)
{
  if (x < date)
    return 1;

  date2 = x;
  price2 = y;
  saveFlag = TRUE;
  
  emit signalMessage(QString());

  return 0;
}

void TLine::moving (QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // first point move
    {
      if (x > date2)
        return;
      
      date = x;
      price = y;
      saveFlag = TRUE;
      
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
    case 2: // second point move
    {
      if (x < date)
        return;
      
      date2 = x;
      price2 = y;
      saveFlag = TRUE;
      
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
    default: // new object moving
    {
      if (x < date)
	return;
      
      date2 = x;
      price2 = y;
      saveFlag = TRUE;
      
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
  
  if (price > h)
    h = price;
  if (price < l)
    l = price;

  if (price2 > h)
    h = price2;
  if (price2 < l)
    l = price2;
  
  return 0;
}

int TLine::inDateRange (PlotData &pd, QDateTime &startDate, QDateTime &endDate)
{
  int rc = FALSE;

  QDateTime dt = date2;
  if (extend)
    pd.dateBars.getDate(pd.dateBars.count() - 1, dt);
  
  // is start past our end?
  if (startDate > dt)
    return rc;
  
  // is end before our start?
  if (endDate < date)
    return rc;
  
  if (startDate >= date && startDate <= dt)
    return TRUE;
  
  if (endDate >= date && endDate <= dt)
    return TRUE;
  
  return rc;
}

