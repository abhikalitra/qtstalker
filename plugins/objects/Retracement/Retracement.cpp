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


#include "Retracement.h"
#include "PrefDialog.h"
#include "Config.h"
#include "fib.xpm"

#include <QObject>
#include <QDebug>

Retracement::Retracement ()
{
}

void Retracement::draw (ChartObject *co, QPixmap &buffer, DateBar &data, int startX, int pixelspace,
			int startIndex, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  QFont font;
  co->getFont(ChartObject::ParmFont, font);
  painter.setFont(font);

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
  QDateTime dt = date2;

  int extend = co->getInt(ChartObject::ParmExtend);
  if (extend)
    data.getDate(data.count() - 1, dt);

  x2 = data.getX(dt);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x2 == -1)
    return;

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  painter.setPen(color);

  QPolygon array;
  co->clearSelectionArea();

  QList<double> lineList;
  lineList.append(co->getDouble(ChartObject::ParmLine1));
  lineList.append(co->getDouble(ChartObject::ParmLine2));
  lineList.append(co->getDouble(ChartObject::ParmLine3));
  lineList.append(co->getDouble(ChartObject::ParmLine4));
  lineList.append(co->getDouble(ChartObject::ParmLine5));
  lineList.append(co->getDouble(ChartObject::ParmLine6));

  double high = co->getDouble(ChartObject::ParmHigh);
  double low = co->getDouble(ChartObject::ParmLow);
  
  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = high - low;
      double r = 0;
      if (td < 0)
        r = low + (range * td);
      else
      {
        if (td > 0)
          r = low + (range * td);
        else
        {
          if (td < 0)
            r = high;
          else
            r = low;
        }
      }
      int y = scaler.convertToY(r);
      painter.drawLine (x, y, x2, y);
      painter.drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      co->setSelectionArea(new QRegion(array));
    }
  }
  
  // draw the low line
  int y = scaler.convertToY(low);
  painter.drawLine (x, y, x2, y);
  painter.drawText(x, y - 1, "0% - " + QString::number(low));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  co->setSelectionArea(new QRegion(array));

  // draw the high line
  int y2 = scaler.convertToY(high);
  painter.drawLine (x, y2, x2, y2);
  painter.drawText(x, y2 - 1, "100% - " + QString::number(high));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  co->setSelectionArea(new QRegion(array));

  if (co->getInt(ChartObject::ParmSelected))
  {
    co->clearGrabHandles();

    //bottom left corner
    y = scaler.convertToY(low);
    co->setGrabHandle(new QRegion(x,
		      y - (handleWidth / 2),
		      handleWidth,
		      handleWidth,
		      QRegion::Rectangle));

    painter.fillRect(x,
		     y - (handleWidth / 2),
		     handleWidth,
		     handleWidth,
		     color);

    //top right corner
    y2 = scaler.convertToY(high);
    co->setGrabHandle(new QRegion(x2,
		      y2 - (handleWidth / 2),
		      handleWidth,
		      handleWidth,
		      QRegion::Rectangle));

    painter.fillRect(x2,
		     y2 - (handleWidth / 2),
		     handleWidth,
		     handleWidth,
		     color);
  }

  painter.end();
}

void Retracement::getInfo (ChartObject *co, Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Retracement");
  info->setData(k, d);

  k = QObject::tr("High Point");
  co->getData(ChartObject::ParmHigh, d);
  info->setData(k, d);

  k = QObject::tr("Low Point");
  co->getData(ChartObject::ParmLow, d);
  info->setData(k, d);

  k = QObject::tr("Line 1");
  co->getData(ChartObject::ParmLine1, d);
  info->setData(k, d);

  k = QObject::tr("Line 2");
  co->getData(ChartObject::ParmLine2, d);
  info->setData(k, d);

  k = QObject::tr("Line 3");
  co->getData(ChartObject::ParmLine3, d);
  info->setData(k, d);

  k = QObject::tr("Line 4");
  co->getData(ChartObject::ParmLine4, d);
  info->setData(k, d);

  k = QObject::tr("Line 5");
  co->getData(ChartObject::ParmLine5, d);
  info->setData(k, d);

  k = QObject::tr("Line 6");
  co->getData(ChartObject::ParmLine6, d);
  info->setData(k, d);
}

void Retracement::dialog (ChartObject *co)
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Retracement");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("High Point");
  double high = co->getDouble(ChartObject::ParmHigh);
  dialog->addDoubleItem(pid++, page, s, high);

  s = QObject::tr("Low Point");
  double low = co->getDouble(ChartObject::ParmLow);
  dialog->addDoubleItem(pid++, page, s, low);

  s = QObject::tr("Extend");
  int extend = co->getInt(ChartObject::ParmExtend);
  dialog->addCheckItem(pid++, page, s, extend);

  int def = FALSE;
  s = QObject::tr("Default");
  dialog->addCheckItem(pid++, page, s, def);

  page++;
  s = QObject::tr("Lines");
  dialog->addPage(page, s);

  s = QObject::tr("Line 1");
  double line1 = co->getDouble(ChartObject::ParmLine1);
  dialog->addDoubleItem(pid++, page, s, line1);

  s = QObject::tr("Line 2");
  double line2 = co->getDouble(ChartObject::ParmLine2);
  dialog->addDoubleItem(pid++, page, s, line2);

  s = QObject::tr("Line 3");
  double line3 = co->getDouble(ChartObject::ParmLine3);
  dialog->addDoubleItem(pid++, page, s, line3);

  s = QObject::tr("Line 4");
  double line4 = co->getDouble(ChartObject::ParmLine4);
  dialog->addDoubleItem(pid++, page, s, line4);

  s = QObject::tr("Line 5");
  double line5 = co->getDouble(ChartObject::ParmLine5);
  dialog->addDoubleItem(pid++, page, s, line5);

  s = QObject::tr("Line 6");
  double line6 = co->getDouble(ChartObject::ParmLine6);
  dialog->addDoubleItem(pid++, page, s, line6);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pid = 0;
  
  dialog->getColor(pid++, color);
  co->setData(ChartObject::ParmColor, color);
  
  high = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmHigh, high);
  
  low = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmLow, low);
  
  extend = dialog->getCheck(pid++);
  co->setData(ChartObject::ParmExtend, extend);
  
  def = dialog->getCheck(pid++);
  
  line1 = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmLine1, line1);
  
  line2 = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmLine2, line2);

  line3 = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmLine3, line3);

  line4 = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmLine4, line4);

  line5 = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmLine5, line5);

  line6 = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmLine6, line6);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultRetracementColor, color);
    config.setBaseData((int) Config::DefaultRetracementLine1, line1);
    config.setBaseData((int) Config::DefaultRetracementLine2, line2);
    config.setBaseData((int) Config::DefaultRetracementLine3, line3);
    config.setBaseData((int) Config::DefaultRetracementLine4, line4);
    config.setBaseData((int) Config::DefaultRetracementLine5, line5);
    config.setBaseData((int) Config::DefaultRetracementLine6, line6);
  }

  co->setData(ChartObject::ParmSave, TRUE);
  
  delete dialog;
}

void Retracement::setSettings (ChartObject *co, QSqlQuery &q)
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

  s = q.value(24).toString(); // d1 field
  co->setData(ChartObject::ParmHigh, s);

  s = q.value(25).toString(); // d2 field
  co->setData(ChartObject::ParmLow, s);

  s = q.value(26).toString(); // d3 field
  co->setData(ChartObject::ParmLine1, s);

  s = q.value(27).toString(); // d4 field
  co->setData(ChartObject::ParmLine2, s);

  s = q.value(28).toString(); // d5 field
  co->setData(ChartObject::ParmLine3, s);

  s = q.value(29).toString(); // d6 field
  co->setData(ChartObject::ParmLine4, s);

  s = q.value(30).toString(); // d7 field
  co->setData(ChartObject::ParmLine5, s);

  s = q.value(31).toString(); // d8 field
  co->setData(ChartObject::ParmLine6, s);

  s = q.value(14).toString(); // i1 field
  co->setData(ChartObject::ParmExtend, s);
}

void Retracement::getSettings (ChartObject *co, QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,d1,d2,d3,d4,d5,d6,d7,d8,i1,t2,t3) VALUES (";
  
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
  
  co->getData(ChartObject::ParmHigh, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLow, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLine1, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLine2, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLine3, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLine4, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLine5, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLine6, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmExtend, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmDate, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmDate2, s);
  set.append(",'" + s + "'");
  
  set.append(")");
}

void Retracement::create (ChartObject *co)
{
  QString s = "Retracement";
  co->setData(ChartObject::ParmPlugin, s);
  
  co->setData(ChartObject::ParmExtend, 0);
  co->setData(ChartObject::ParmLine1, 0);
  co->setData(ChartObject::ParmLine2, 0);
  co->setData(ChartObject::ParmLine3, 0);
  co->setData(ChartObject::ParmLine4, 0);
  co->setData(ChartObject::ParmLine5, 0);
  co->setData(ChartObject::ParmLine6, 0);
  co->setData(ChartObject::ParmSave, TRUE);

  s = QObject::tr("Select Retracement high point...");
  co->message(s);
}

int Retracement::create2 (ChartObject *co, QDateTime &x, double y)
{
  co->setData(ChartObject::ParmDate, x);
  co->setData(ChartObject::ParmPrice, y);
  co->setData(ChartObject::ParmHigh, y);
  
  QString s = QObject::tr("Select Retracement low point...");
  co->message(s);
  
  return 1;
}

int Retracement::create3 (ChartObject *co, QDateTime &x, double y)
{
  QDateTime date;
  co->getDate(ChartObject::ParmDate, date);
  if (x < date)
    return 1;

  double high = co->getDouble(ChartObject::ParmHigh);
  if (y > high)
    return 1;

  co->setData(ChartObject::ParmDate2, x);
  co->setData(ChartObject::ParmLow, y);

  return 0;
}

void Retracement::moving (ChartObject *co, QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // bottom left corner
    {
      QDateTime date2;
      co->getDate(ChartObject::ParmDate2, date2);
      if (x > date2)
        return;
      
      double high = co->getDouble(ChartObject::ParmHigh);
      if (y > high)
        return;
      
      co->setData(ChartObject::ParmDate, x);
      co->setData(ChartObject::ParmLow, y);
      co->setData(ChartObject::ParmSave, TRUE);
      
      QString s = x.toString(Qt::ISODate) + " " + QString::number(y);
      co->message(s);
      break;
    }
    case 2: //top right corner
    {
      QDateTime date;
      co->getDate(ChartObject::ParmDate, date);
      if (x < date)
        return;
      
      double low = co->getDouble(ChartObject::ParmLow);
      if (y < low)
        return;
      
      co->setData(ChartObject::ParmDate2, x);
      co->setData(ChartObject::ParmHigh, y);
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
      
      double high = co->getDouble(ChartObject::ParmHigh);
      if (y > high)
        return;
      
      co->setData(ChartObject::ParmDate2, x);
      co->setData(ChartObject::ParmLow, y);
      co->setData(ChartObject::ParmSave, TRUE);

      QString s = x.toString(Qt::ISODate) + " " + QString::number(y);
      co->message(s);
      break;
    }
  }
}

void Retracement::getIcon (QIcon &d)
{
  d = QIcon(fib_xpm);
}

int Retracement::inDateRange (ChartObject *co, QDateTime &startDate, QDateTime &endDate)
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
  Retracement *o = new Retracement;
  return ((COPlugin *) o);
}


