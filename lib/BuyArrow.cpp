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


#include "BuyArrow.h"
#include "PrefDialog.h"
#include "Config.h"

#include <QPainter>
#include <QDebug>

BuyArrow::BuyArrow ()
{
  type = (int) COBuyArrow;
  color.setNamedColor("green");
  price = 0;

}

void BuyArrow::draw (QPixmap &buffer, BarData *data, int startX, int pixelspace, int startIndex,
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
                  x + 5, y + 5,
                  x + 2, y + 5,
                  x + 2, y + 11,
                  x - 2, y + 11,
	          x - 2, y + 5,
                  x - 5, y + 5);
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

void BuyArrow::getInfo (Setting &info)
{
  QString k = tr("Type");
  QString d = tr("Buy Arrow");
  info.setData(k, d);

  k = tr("Date");
  d = date.toString(Qt::ISODate);
  info.setData(k, d);

  k = tr("Price");
  d = QString::number(price);
  info.setData(k, d);
}


void BuyArrow::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = tr("Edit Buy Arrow");
  dialog->setWindowTitle(s);
  s = tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  s = tr("Color");
  dialog->addColorItem(0, page, s, color);

  s = tr("Price");
  dialog->addDoubleItem(1, page, s, price);

  int def = FALSE;
  s = tr("Set Default");
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
    config.setBaseData((int) Config::DefaultBuyArrowColor, color);
  }

  setSaveFlag(TRUE);
  delete dialog;
}

void BuyArrow::setDate (QDateTime &d)
{
  date = d;
}

void BuyArrow::setPrice (double d)
{
  price = d;
}

void BuyArrow::setSettings (QSqlQuery &q)
{
  id = q.value(0).toString();
  symbol = q.value(1).toString();
  indicator = q.value(2).toString();
  color.setNamedColor(q.value(4).toString());
  date = q.value(5).toDateTime();
  price = q.value(7).toDouble();
}

void BuyArrow::getSettings (QString &set)
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

void BuyArrow::create (QString &_id, QString &_symbol, QString &_indicator)
{
  id = _id;
  symbol = _symbol;
  indicator = _indicator;
  emit signalMessage(QString(tr("Select point to place Buy Arrow...")));
}

int BuyArrow::create2 (QDateTime &x, double y)
{
  setSaveFlag(TRUE);
  date = x;
  price = y;
  return 0;
}

void BuyArrow::moving (QDateTime &x, double y, int)
{
  date = x;
  price = y;
  setSaveFlag(TRUE);
  emit signalMessage(QString(date.toString(Qt::ISODate) + " " + QString::number(y)));
}

