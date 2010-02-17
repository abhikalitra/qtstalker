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


#include "Retracement.h"
#include "PrefDialog.h"
#include "Config.h"

#include <QObject>
#include <QPainter>

Retracement::Retracement ()
{
  type = (int) CORetracement;
  extend = 0;
  line1 = 0;
  line2 = 0;
  line3 = 0;
  line4 = 0;
  line5 = 0;
  line6 = 0;
}

void Retracement::draw (QPixmap &buffer, BarData *data, int startX, int pixelspace, int startIndex,
		     Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  QFont font;
  painter.setFont(font);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  QDateTime dt = date2;

  if (extend)
    data->getDate(data->count() - 1, dt);

  x2 = data->getX(dt);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
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
      int y = scaler.convertToY(r);
      painter.drawLine (x, y, x2, y);
      painter.drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      setSelectionArea(new QRegion(array));
    }
  }

  // draw the low line
  int y = scaler.convertToY(low);
  painter.drawLine (x, y, x2, y);
  painter.drawText(x, y - 1, "0% - " + QString::number(low));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  setSelectionArea(new QRegion(array));

  // draw the high line
  int y2 = scaler.convertToY(high);
  painter.drawLine (x, y2, x2, y2);
  painter.drawText(x, y2 - 1, "100% - " + QString::number(high));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));

  if (selected)
  {
    clearGrabHandles();

    //bottom left corner
    y = scaler.convertToY(low);
    setGrabHandle(new QRegion(x,
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
    setGrabHandle(new QRegion(x2,
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

void Retracement::getInfo (Setting &info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Retracement");
  info.setData(k, d);

  k = QObject::tr("High Point");
  d = QString::number(high);
  info.setData(k, d);

  k = QObject::tr("Low Point");
  d = QString::number(low);
  info.setData(k, d);

  k = QObject::tr("Line 1");
  d = QString::number(line1);
  info.setData(k, d);

  k = QObject::tr("Line 2");
  d = QString::number(line2);
  info.setData(k, d);

  k = QObject::tr("Line 3");
  d = QString::number(line3);
  info.setData(k, d);

  k = QObject::tr("Line 4");
  d = QString::number(line4);
  info.setData(k, d);

  k = QObject::tr("Line 5");
  d = QString::number(line5);
  info.setData(k, d);

  k = QObject::tr("Line 6");
  d = QString::number(line6);
  info.setData(k, d);
}

void Retracement::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Retracement");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(0, page, s, color);

  s = QObject::tr("High Point");
  dialog->addDoubleItem(1, page, s, high);

  s = QObject::tr("Low Point");
  dialog->addDoubleItem(2, page, s, low);

  s = QObject::tr("Line 1");
  dialog->addDoubleItem(3, page, s, line1);

  s = QObject::tr("Line 2");
  dialog->addDoubleItem(4, page, s, line2);

  s = QObject::tr("Line 3");
  dialog->addDoubleItem(5, page, s, line3);

  s = QObject::tr("Line 4");
  dialog->addDoubleItem(6, page, s, line4);

  s = QObject::tr("Line 5");
  dialog->addDoubleItem(7, page, s, line5);

  s = QObject::tr("Line 6");
  dialog->addDoubleItem(8, page, s, line6);

  s = QObject::tr("Extend");
  dialog->addCheckItem(9, page, s, extend);

  int def = FALSE;
  s = QObject::tr("Set Default");
  dialog->addCheckItem(10, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  dialog->getColor(0, color);
  high = dialog->getDouble(1);
  low = dialog->getDouble(2);
  line1 = dialog->getDouble(3);
  line2 = dialog->getDouble(4);
  line3 = dialog->getDouble(5);
  line4 = dialog->getDouble(6);
  line5 = dialog->getDouble(7);
  line6 = dialog->getDouble(8);
  extend = dialog->getCheck(9);
  def = dialog->getCheck(10);

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

  setSaveFlag(TRUE);
  delete dialog;
}

void Retracement::setDate (QDateTime &d)
{
  date = d;
}

void Retracement::getDate (QDateTime &d)
{
  d = date;
}

void Retracement::setDate2 (QDateTime &d)
{
  date2 = d;
}

void Retracement::getDate2 (QDateTime &d)
{
  d = date2;
}

void Retracement::setSettings (Setting &set)
{
  QString k = QString::number(ParmColor);
  QString d;
  set.getData(k, d);
  color.setNamedColor(d);

  k = QString::number(ParmHigh);
  high = set.getDouble(k);

  k = QString::number(ParmLow);
  low = set.getDouble(k);

  k = QString::number(ParmLine1);
  line1 = set.getDouble(k);

  k = QString::number(ParmLine2);
  line2 = set.getDouble(k);

  k = QString::number(ParmLine3);
  line3 = set.getDouble(k);

  k = QString::number(ParmLine4);
  line4 = set.getDouble(k);

  k = QString::number(ParmLine5);
  line5 = set.getDouble(k);

  k = QString::number(ParmLine6);
  line6 = set.getDouble(k);

  k = QString::number(ParmExtend);
  extend = set.getInt(k);

  k = QString::number(ParmDate);
  set.getData(k, d);
  date = QDateTime::fromString(d, Qt::ISODate);

  k = QString::number(ParmDate2);
  set.getData(k, d);
  date2 = QDateTime::fromString(d, Qt::ISODate);
}

void Retracement::getSettings (QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,type,color,high,low,line1,line2,line3,line4,line5,line6,extend,date,date2) VALUES (";
  set.append(id);
  set.append(",'" + symbol + "'");
  set.append(",'" + indicator + "'");
  set.append("," + QString::number(type));
  set.append(",'" + color.name() + "'");
  set.append("," + QString::number(high));
  set.append("," + QString::number(low));
  set.append("," + QString::number(line1));
  set.append("," + QString::number(line2));
  set.append("," + QString::number(line3));
  set.append("," + QString::number(line4));
  set.append("," + QString::number(line5));
  set.append("," + QString::number(line6));
  set.append("," + QString::number(extend));
  set.append(",'" + date.toString(Qt::ISODate) + "'");
  set.append(",'" + date2.toString(Qt::ISODate) + "'");
  set.append(")");
}

void Retracement::create (QString &_id, QString &_symbol, QString &_indicator)
{
  setSaveFlag(TRUE);
  id = _id;
  symbol = _symbol;
  indicator = _indicator;
  emit signalMessage(QString(tr("Select Retracement high point...")));
}

int Retracement::create2 (QDateTime &x, double y)
{
  date = x;
  high = y;
  emit signalMessage(QString(tr("Select Retracement low point...")));
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
  return 0;
}

void Retracement::moving (QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // bottom left corner
      if (x > date2)
        return;
      if (y > high)
        return;
      date = x;
      low = y;
      setSaveFlag(TRUE);
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    case 2: //top right corner
      if (x < date)
        return;
      if (y < low)
        return;
      date2 = x;
      high = y;
      setSaveFlag(TRUE);
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    default: // new object moving
      if (x < date)
        return;
      if (y > high)
        return;
      date2 = x;
      low = y;
      setSaveFlag(TRUE);
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
  }
}

