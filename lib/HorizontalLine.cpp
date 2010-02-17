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


#include "HorizontalLine.h"
#include "PrefDialog.h"
#include "Config.h"

#include <QObject>
#include <QPainter>

HorizontalLine::HorizontalLine ()
{
  type = (int) COHorizontalLine;
  price = 0;

}

void HorizontalLine::draw (QPixmap &buffer, BarData *, int, int, int, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  QFont font;
  painter.setFont(font);

  int y = scaler.convertToY(price);

  // if value is off chart then don't draw it
  if (price < scaler.getLow())
    return;

  painter.setPen(color);

  QFontMetrics fm(font);
  int pixelsWide = fm.width(label);
  painter.drawLine (0, y, buffer.width(), y);
  painter.drawText(0, y - 1, label);
  painter.drawText(0 + pixelsWide + 1, y - 1, QString::number(price));

  clearSelectionArea();
  QPolygon array;
  array.putPoints(0, 4, 0, y - 4, 0, y + 4, buffer.width(), y + 4, buffer.width(), y - 4);
  setSelectionArea(new QRegion(array));

  if (selected)
  {
    clearGrabHandles();
    int t = (int) buffer.width() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      setGrabHandle(new QRegion(t * loop,
				y - (handleWidth / 2),
				handleWidth,
				handleWidth,
				QRegion::Rectangle));

      painter.fillRect(t * loop,
		       y - (handleWidth / 2),
		       handleWidth,
		       handleWidth,
		       color);
    }
  }

  painter.end();
}

void HorizontalLine::getInfo (Setting &info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Sell Arrow");
  info.setData(k, d);

  k = QObject::tr("Price");
  d = QString::number(price);
  info.setData(k, d);
}

void HorizontalLine::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Horizontal Line");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(0, page, s, color);

  s = QObject::tr("Price");
  dialog->addDoubleItem(1, page, s, price);

  s = QObject::tr("Label");
  dialog->addTextItem(2, page, s, label);

  int def = FALSE;
  s = QObject::tr("Set Default");
  dialog->addCheckItem(3, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  dialog->getColor(0, color);
  price = dialog->getDouble(1);
  dialog->getText(2, label);
  def = dialog->getCheck(3);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultHorizontalLineColor, color);
  }

  setSaveFlag(TRUE);
  delete dialog;
}

void HorizontalLine::setPrice (double d)
{
  price = d;
}

void HorizontalLine::setSettings (Setting &set)
{
  QString k = QString::number(ParmColor);
  QString d;
  set.getData(k, d);
  color.setNamedColor(d);

  k = QString::number(ParmPrice);
  price = set.getDouble(k);

  k = QString::number(ParmLabel);
  set.getData(k, label);
}

void HorizontalLine::getSettings (QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,type,color,price,label) VALUES (";
  set.append(id);
  set.append(",'" + symbol + "'");
  set.append(",'" + indicator + "'");
  set.append("," + QString::number(type));
  set.append(",'" + color.name() + "'");
  set.append("," + QString::number(price));
  set.append(",'" + label + "'");
  set.append(")");
}

void HorizontalLine::create (QString &_id, QString &_symbol, QString &_indicator)
{
  id = _id;
  symbol = _symbol;
  indicator = _indicator;
  emit signalMessage(QString(tr("Select point to place Horizontal Line...")));
}

int HorizontalLine::create2 (QDateTime &, double y)
{
  setSaveFlag(TRUE);
  price = y;
  return 0;
}

void HorizontalLine::moving (QDateTime &, double y, int)
{
  price = y;
  setSaveFlag(TRUE);
  emit signalMessage(QString(QString::number(y)));
}

