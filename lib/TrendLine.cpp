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


#include "TrendLine.h"
#include "PrefDialog.h"
#include "Config.h"

#include <QObject>
#include <QPainter>

TrendLine::TrendLine ()
{
  type = (int) COTrendLine;
  extend = 0;
  useBar = 0;
  price = 0;
  price2 = 0;
  bar = QObject::tr("Close");
  fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");
}

void TrendLine::draw (QPixmap &buffer, BarData *data, int startX, int pixelspace, int startIndex,
		     Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  x2 = data->getX(date2);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x2 == -1)
    return;

  int y = 0;
  int y2 = 0;
  if (useBar)
  {
    int i = data->getX(date);
    int i2 = data->getX(date2);

    switch (fieldList.indexOf(bar))
    {
      case 0: // open
        y = scaler.convertToY(data->getOpen(i));
        y2 = scaler.convertToY(data->getOpen(i2));
	break;
      case 1: // high
        y = scaler.convertToY(data->getHigh(i));
        y2 = scaler.convertToY(data->getHigh(i2));
	break;
      case 2: // low
        y = scaler.convertToY(data->getLow(i));
        y2 = scaler.convertToY(data->getLow(i2));
	break;
      default:
        // assume Close, for now. Need to display a warning.
        y = scaler.convertToY(data->getClose(i));
        y2 = scaler.convertToY(data->getClose(i2));
	break;
    }
  }
  else
  {
    y = scaler.convertToY(price);
    y2 = scaler.convertToY(price2);
  }

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

void TrendLine::getInfo (Setting &info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("TrendLine");
  info.setData(k, d);

  k = QObject::tr("Start Date");
  d = date.toString(Qt::ISODate);
  info.setData(k, d);

  k = QObject::tr("Start Price");
  d = QString::number(price);
  info.setData(k, d);

  k = QObject::tr("End Date");
  d = date2.toString(Qt::ISODate);
  info.setData(k, d);

  k = QObject::tr("End Price");
  d = QString::number(price2);
  info.setData(k, d);
}

void TrendLine::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit TrendLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(0, page, s, color);

  s = QObject::tr("Start Price");
  dialog->addDoubleItem(1, page, s, price);

  s = QObject::tr("End Price");
  dialog->addDoubleItem(2, page, s, price2);

  s = QObject::tr("Bar Field");
  dialog->addComboItem(3, page, s, fieldList, bar);

  s = QObject::tr("Use Bar");
  dialog->addCheckItem(4, page, s, useBar);

  s = QObject::tr("Extend");
  dialog->addCheckItem(5, page, s, extend);

  int def = FALSE;
  s = QObject::tr("Set Default");
  dialog->addCheckItem(6, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  dialog->getColor(0, color);
  price = dialog->getDouble(1);
  price2 = dialog->getDouble(2);
  dialog->getCombo(3, bar);
  useBar = dialog->getCheck(4);
  extend = dialog->getCheck(5);
  def = dialog->getCheck(6);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultTrendLineColor, color);
    config.setData(Config::DefaultTrendLineBar, bar);
    config.setBaseData((int) Config::DefaultTrendLineUseBar, useBar);
    config.setBaseData((int) Config::DefaultTrendLineExtend, extend);
  }

  setSaveFlag(TRUE);
  delete dialog;
}

void TrendLine::setDate (QDateTime &d)
{
  date = d;
}

void TrendLine::getDate (QDateTime &d)
{
  d = date;
}

void TrendLine::setDate2 (QDateTime &d)
{
  date2 = d;
}

void TrendLine::getDate2 (QDateTime &d)
{
  d = date2;
}

void TrendLine::setPrice (double d)
{
  price = d;
}

void TrendLine::setPrice2 (double d)
{
  price2 = d;
}

void TrendLine::getHighLow (double &h, double &l)
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
}

void TrendLine::setSettings (Setting &set)
{
  QString k = QString::number(ParmColor);
  QString d;
  set.getData(k, d);
  color.setNamedColor(d);

  k = QString::number(ParmDate);
  set.getData(k, d);
  date = QDateTime::fromString(d, Qt::ISODate);

  k = QString::number(ParmDate2);
  set.getData(k, d);
  date2 = QDateTime::fromString(d, Qt::ISODate);

  k = QString::number(ParmPrice);
  price = set.getDouble(k);

  k = QString::number(ParmPrice2);
  price2 = set.getDouble(k);

  k = QString::number(ParmExtend);
  extend = set.getInt(k);

  k = QString::number(ParmUseBar);
  useBar = set.getInt(k);

  k = QString::number(ParmBarField);
  set.getData(k, bar);
}

void TrendLine::getSettings (QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,type,color,date,date2,price,price2,extend,useBar,barField) VALUES (";
  set.append(id);
  set.append(",'" + symbol + "'");
  set.append(",'" + indicator + "'");
  set.append("," + QString::number(type));
  set.append(",'" + color.name() + "'");
  set.append(",'" + date.toString(Qt::ISODate) + "'");
  set.append(",'" + date2.toString(Qt::ISODate) + "'");
  set.append("," + QString::number(price));
  set.append("," + QString::number(price2));
  set.append("," + QString::number(extend));
  set.append("," + QString::number(useBar));
  set.append(",'" + bar + "'");
  set.append(")");
}

void TrendLine::create (QString &_id, QString &_symbol, QString &_indicator)
{
  id = _id;
  symbol = _symbol;
  indicator = _indicator;
  emit signalMessage(QString(tr("Select Trend Line starting point...")));
}

int TrendLine::create2 (QDateTime &x, double y)
{
  date = x;
  date2 = x;
  price = y;
  price2 = y;
  emit signalMessage(QString(tr("Select TrendLine ending point...")));
  return 1;
}

int TrendLine::create3 (QDateTime &x, double y)
{
  if (x < date)
    return 1;

  date2 = x;
  price2 = y;
  setSaveFlag(TRUE);
  return 0;
}

void TrendLine::moving (QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // first point move
      if (x > date2)
        return;
      date = x;
      price = y;
      setSaveFlag(TRUE);
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    case 2: // second point move
      if (x < date)
        return;
      date2 = x;
      price2 = y;
      setSaveFlag(TRUE);
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    default: // new object moving
      if (x < date)
	return;
      date2 = x;
      price2 = y;
      setSaveFlag(TRUE);
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
  }
}

