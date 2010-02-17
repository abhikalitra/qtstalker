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


#include "SellArrow.h"
#include "PrefDialog.h"
#include "Config.h"

#include <QObject>
#include <QPainter>

SellArrow::SellArrow ()
{
  type = (int) COSellArrow;
  price = 0;

}

void SellArrow::draw (QPixmap &buffer, BarData *data, int startX, int pixelspace, int startIndex,
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

  int y = scaler.convertToY(price);

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
	          x + 2, y - 5,
	          x + 2, y - 11,
	          x - 2, y - 11,
	          x - 2, y - 5,
                  x - 5, y - 5);
  painter.setBrush(color);
  painter.drawPolygon(arrow, Qt::OddEvenFill);

  clearSelectionArea();
  setSelectionArea(new QRegion(arrow));

  if (selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - (handleWidth / 2),
			      y - handleWidth,
			      handleWidth,
			      handleWidth,
			      QRegion::Rectangle));

    painter.fillRect(x - (handleWidth / 2),
		     y - handleWidth,
		     handleWidth,
		     handleWidth,
		     color);
  }

  painter.end();
}

void SellArrow::getInfo (Setting &info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Sell Arrow");
  info.setData(k, d);

  k = QObject::tr("Date");
  d = date.toString(Qt::ISODate);
  info.setData(k, d);

  k = QObject::tr("Price");
  d = QString::number(price);
  info.setData(k, d);
}


void SellArrow::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Sell Arrow");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(0, page, s, color);

  s = QObject::tr("Price");
  dialog->addDoubleItem(1, page, s, price);

  int def = FALSE;
  s = QObject::tr("Set Default");
  dialog->addCheckItem(2, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  dialog->getColor(0, color);
  price = dialog->getDouble(1);
  def = dialog->getCheck(2);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultSellArrowColor, color);
  }

  setSaveFlag(TRUE);
  delete dialog;
}

void SellArrow::setDate (QDateTime &d)
{
  date = d;
}

void SellArrow::setPrice (double d)
{
  price = d;
}

void SellArrow::setSettings (Setting &set)
{
  QString k = QString::number(ParmColor);
  QString d;
  set.getData(k, d);
  color.setNamedColor(d);

  k = QString::number(ParmDate);
  set.getData(k, d);
  date = QDateTime::fromString(d, Qt::ISODate);

  k = QString::number(ParmPrice);
  price = set.getDouble(k);
}

void SellArrow::getSettings (QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,type,color,date,price) VALUES (";
  set.append(id);
  set.append(",'" + symbol + "'");
  set.append(",'" + indicator + "'");
  set.append("," + QString::number(type));
  set.append(",'" + color.name() + "'");
  set.append(",'" + date.toString(Qt::ISODate) + "'");
  set.append("," + QString::number(price));
  set.append(")");
}

void SellArrow::create (QString &_id, QString &_symbol, QString &_indicator)
{
  id = _id;
  symbol = _symbol;
  indicator = _indicator;
  emit signalMessage(QString(tr("Select point to place Sell Arrow...")));
}

int SellArrow::create2 (QDateTime &x, double y)
{
  setSaveFlag(TRUE);
  date = x;
  price = y;
  return 0;
}

void SellArrow::moving (QDateTime &x, double y, int)
{
  date = x;
  price = y;
  setSaveFlag(TRUE);
  emit signalMessage(QString(date.toString(Qt::ISODate) + " " + QString::number(y)));
}

