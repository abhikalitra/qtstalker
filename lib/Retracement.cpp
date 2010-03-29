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
#include "CODataBase.h"

#include "../pics/fib.xpm"

#include <QObject>
#include <QDebug>
#include <QPainter>

Retracement::Retracement ()
{
  plugin = "Retracement";
  color.setNamedColor("red");
  high = 0;
  low = 0;
  extend = 0;
  line1 = 0.236;
  line2 = 0.382;
  line3 = 0.5;
  line4 = 0.618;
  line5 = 0;
  line6 = 0;
}

void Retracement::draw (PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  painter.setFont(pd.plotFont);

  int x = pd.dateBars.getX(date);
  if (x == -1)
    return;

  x = pd.startX + (x * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
  if (x == -1)
    return;

  QDateTime dt = date2;

  if (extend)
    pd.dateBars.getDate(pd.dateBars.count() - 1, dt);

  int x2 = pd.dateBars.getX(dt);
  if (x2 == -1)
    return;

  x2 = pd.startX + (x2 * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
  if (x2 == -1)
    return;

  painter.setPen(color);

  QPolygon array;
  clearSelectionArea();

  QList<double> lineList;
  lineList.append(line1);
  lineList.append(line2);
  lineList.append(line3);
  lineList.append(line4);
  lineList.append(line5);
  lineList.append(line6);

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
      int y = pd.scaler.convertToY(r);
      painter.drawLine (x, y, x2, y);
      painter.drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      setSelectionArea(new QRegion(array));
    }
  }
  
  // draw the low line
  int y = pd.scaler.convertToY(low);
  painter.drawLine (x, y, x2, y);
  painter.drawText(x, y - 1, "0% - " + QString::number(low));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  setSelectionArea(new QRegion(array));

  // draw the high line
  int y2 = pd.scaler.convertToY(high);
  painter.drawLine (x, y2, x2, y2);
  painter.drawText(x, y2 - 1, "100% - " + QString::number(high));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));
  
  if (selected)
  {
    clearGrabHandles();

    //top left corner
    y = pd.scaler.convertToY(high);
    setGrabHandle(new QRegion(x, y - (handleWidth / 2),
		  handleWidth,
		  handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x,
		     y - (handleWidth / 2),
		     handleWidth,
		     handleWidth,
		     color);

    //bottom right corner
    x2 = pd.startX + (pd.dateBars.getX(date2) * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
    y2 = pd.scaler.convertToY(low);
    setGrabHandle(new QRegion(x2, y2 - (handleWidth / 2),
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

void Retracement::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Retracement");
  info->setData(k, d);

  k = QObject::tr("High Point");
  d = QString::number(high);
  info->setData(k, d);

  k = QObject::tr("Low Point");
  d = QString::number(low);
  info->setData(k, d);

  k = QObject::tr("Line 1");
  d = QString::number(line1);
  info->setData(k, d);

  k = QObject::tr("Line 2");
  d = QString::number(line2);
  info->setData(k, d);

  k = QObject::tr("Line 3");
  d = QString::number(line3);
  info->setData(k, d);

  k = QObject::tr("Line 4");
  d = QString::number(line4);
  info->setData(k, d);

  k = QObject::tr("Line 5");
  d = QString::number(line5);
  info->setData(k, d);

  k = QObject::tr("Line 6");
  d = QString::number(line6);
  info->setData(k, d);
}

void Retracement::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Retracement");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("High Point");
  dialog->addDoubleItem(pid++, page, s, high);

  s = QObject::tr("Low Point");
  dialog->addDoubleItem(pid++, page, s, low);

  s = QObject::tr("Extend");
  dialog->addCheckItem(pid++, page, s, extend);

  int def = FALSE;
  s = QObject::tr("Default");
  dialog->addCheckItem(pid++, page, s, def);

  page++;
  s = QObject::tr("Lines");
  dialog->addPage(page, s);

  s = QObject::tr("Line 1");
  dialog->addDoubleItem(pid++, page, s, line1);

  s = QObject::tr("Line 2");
  dialog->addDoubleItem(pid++, page, s, line2);

  s = QObject::tr("Line 3");
  dialog->addDoubleItem(pid++, page, s, line3);

  s = QObject::tr("Line 4");
  dialog->addDoubleItem(pid++, page, s, line4);

  s = QObject::tr("Line 5");
  dialog->addDoubleItem(pid++, page, s, line5);

  s = QObject::tr("Line 6");
  dialog->addDoubleItem(pid++, page, s, line6);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pid = 0;
  dialog->getColor(pid++, color);
  high = dialog->getDouble(pid++);
  low = dialog->getDouble(pid++);
  extend = dialog->getCheck(pid++);
  def = dialog->getCheck(pid++);
  line1 = dialog->getDouble(pid++);
  line2 = dialog->getDouble(pid++);
  line3 = dialog->getDouble(pid++);
  line4 = dialog->getDouble(pid++);
  line5 = dialog->getDouble(pid++);
  line6 = dialog->getDouble(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultRetracementColor, color);
    config.setData((int) Config::DefaultRetracementLine1, line1);
    config.setData((int) Config::DefaultRetracementLine2, line2);
    config.setData((int) Config::DefaultRetracementLine3, line3);
    config.setData((int) Config::DefaultRetracementLine4, line4);
    config.setData((int) Config::DefaultRetracementLine5, line5);
    config.setData((int) Config::DefaultRetracementLine6, line6);
  }

  saveFlag = TRUE;
  
  delete dialog;
}

void Retracement::load (QSqlQuery &q)
{
  id = q.value(0).toInt();
  exchange = q.value(1).toString();
  symbol = q.value(2).toString();
  indicator = q.value(3).toString();
  color.setNamedColor(q.value(5).toString()); // t1 field
  date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate); // t2 field
  date2 = QDateTime::fromString(q.value(7).toString(), Qt::ISODate); // t3 field
  high = q.value(25).toDouble(); // d1 field
  low = q.value(26).toDouble(); // d2 field
  line1 = q.value(27).toDouble(); // d3 field
  line2 = q.value(28).toDouble(); // d4 field
  line3 = q.value(29).toDouble(); // d5 field
  line4 = q.value(30).toDouble(); // d6 field
  line5 = q.value(31).toDouble(); // d7 field
  line6 = q.value(32).toDouble(); // d8 field
  extend = q.value(15).toInt(); // i1 field
}

void Retracement::save ()
{
  if (! saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,d1,d2,d3,d4,d5,d6,d7,d8,i1,t2,t3) VALUES (";
  s.append(QString::number(id));
  s.append(",'" + exchange + "'");
  s.append(",'" + symbol + "'");
  s.append(",'" + indicator + "'");
  s.append(",'" + plugin + "'");
  s.append(",'" + color.name() + "'");
  s.append("," + QString::number(high));
  s.append("," + QString::number(low));
  s.append("," + QString::number(line1));
  s.append("," + QString::number(line2));
  s.append("," + QString::number(line3));
  s.append("," + QString::number(line4));
  s.append("," + QString::number(line5));
  s.append("," + QString::number(line6));
  s.append("," + QString::number(extend));
  s.append(",'" + date.toString(Qt::ISODate) + "'");
  s.append(",'" + date2.toString(Qt::ISODate) + "'");
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  saveFlag = FALSE;
}

void Retracement::create ()
{
  saveFlag = TRUE;
  emit signalMessage(QString(QObject::tr("Select Retracement high point...")));
}

int Retracement::create2 (QDateTime &x, double y)
{
  date = x;
  high = y;
  emit signalMessage(QString(QObject::tr("Select Retracement low point...")));
  
  return 1;
}

int Retracement::create3 (QDateTime &x, double y)
{
  if (x < date)
    return 1;

  if (y > high)
    return 1;

  date2 = x;
  low = y;

  emit signalMessage(QString());

  return 0;
}

void Retracement::moving (QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // top left corner
    {
      if (x > date2)
        return;
      
      if (y < low)
        return;
      
      date = x;
      high = y;
      saveFlag = TRUE;
      
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
    case 2:
    default: // bottom right corner
    {
      if (x < date)
        return;
      
      if (y > high)
        return;
      
      date2 = x;
      low = y;
      saveFlag = TRUE;

      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
  }
}

void Retracement::getIcon (QIcon &d)
{
  d = QIcon(fib_xpm);
}

int Retracement::inDateRange (PlotData &pd, QDateTime &startDate, QDateTime &endDate)
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

int Retracement::getHighLow (double &h, double &l)
{
  h = high;
  l = low;
  return 0;
}

