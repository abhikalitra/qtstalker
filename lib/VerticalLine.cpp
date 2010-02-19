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


#include "VerticalLine.h"
#include "PrefDialog.h"
#include "Config.h"

#include <QPainter>

VerticalLine::VerticalLine ()
{
  type = (int) COVerticalLine;
}

void VerticalLine::draw (QPixmap &buffer, BarData *data, int startX, int pixelspace, int startIndex, Scaler &)
{
  QPainter painter;
  painter.begin(&buffer);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  painter.setPen(color);

  painter.drawLine (x, 0, x, buffer.height());

  clearSelectionArea();
  QPolygon array;
  array.putPoints(0,
  	          4,
		  x - (handleWidth / 2), 0,
		  x + (handleWidth / 2), 0,
		  x + (handleWidth / 2), buffer.height(),
		  x - (handleWidth / 2), buffer.height());
  setSelectionArea(new QRegion(array));

  if (selected)
  {
    clearGrabHandles();
    int t = (int) buffer.height() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      setGrabHandle(new QRegion(x - (handleWidth / 2),
				t * loop,
				handleWidth,
				handleWidth,
				QRegion::Rectangle));

      painter.fillRect(x - (handleWidth / 2),
		       t * loop,
		       handleWidth,
		       handleWidth,
		       color);
    }
  }

  painter.end();
}

void VerticalLine::getInfo (Setting &info)
{
  QString k = tr("Type");
  QString d = tr("Vertical Line");
  info.setData(k, d);

  k = tr("Date");
  d = date.toString(Qt::ISODate);
  info.setData(k, d);
}

void VerticalLine::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = tr("Edit Vertical Line");
  dialog->setWindowTitle(s);
  s = tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = tr("Color");
  dialog->addColorItem(pid++, page, s, color);

  int def = FALSE;
  s = tr("Set Default");
  dialog->addCheckItem(pid++, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pid = 0;
  dialog->getColor(pid++, color);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultVerticalLineColor, color);
  }

  setSaveFlag(TRUE);
  delete dialog;
}

void VerticalLine::setDate (QDateTime &d)
{
  date = d;
}

void VerticalLine::setSettings (QSqlQuery &q)
{
  id = q.value(0).toString();
  symbol = q.value(1).toString();
  indicator = q.value(2).toString();
  color.setNamedColor(q.value(4).toString());
  date = q.value(5).toDateTime();
}

void VerticalLine::getSettings (QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,type,color,date) VALUES (";
  set.append(id);
  set.append(",'" + symbol + "'");
  set.append(",'" + indicator + "'");
  set.append("," + QString::number(type));
  set.append(",'" + color.name() + "'");
  set.append(",'" + date.toString(Qt::ISODate) + "'");
  set.append(")");
}

void VerticalLine::create (QString &_id, QString &_symbol, QString &_indicator)
{
  id = _id;
  symbol = _symbol;
  indicator = _indicator;
  emit signalMessage(QString(tr("Select point to place Vertical Line...")));
}

int VerticalLine::create2 (QDateTime &x, double)
{
  setSaveFlag(TRUE);
  date = x;
  return 0;
}

void VerticalLine::moving (QDateTime &x, double, int)
{
  date = x;
  setSaveFlag(TRUE);
  emit signalMessage(QString(date.toString(Qt::ISODate)));
}

