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


#include "Sell.h"
#include "PrefDialog.h"
#include "Config.h"
#include "CODataBase.h"

#include "../pics/sellarrow.xpm"

#include <QObject>
#include <QPainter>

Sell::Sell ()
{
  plugin = "Sell";
  color.setNamedColor("red");
  price = 0;
}

void Sell::draw (PlotData &pd, DateBar &dateBars, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = dateBars.getX(date);
  if (x2 == -1)
    return;

  int x = (x2 * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
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
		     y + (handleWidth / 2),
		     handleWidth,
		     handleWidth,
		     color);
  }

  painter.end();
}

void Sell::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Sell");
  info->setData(k, d);

  k = QObject::tr("Date");
  d = date.toString(Qt::ISODate);
  info->setData(k, d);

  k = QObject::tr("Price");
  d = QString::number(price);
  info->setData(k, d);
}

void Sell::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Sell");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0 ;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("Price");
  dialog->addDoubleItem(pid++, page, s, price);

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
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultSellColor, color);
  }

  saveFlag = TRUE;
  
  delete dialog;
}

void Sell::load (QSqlQuery &q)
{
  id = q.value(0).toInt();
  exchange = q.value(1).toString();
  symbol = q.value(2).toString();
  indicator = q.value(3).toString();
  color.setNamedColor(q.value(5).toString());
  date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate);
  price = q.value(25).toDouble();
}

void Sell::save ()
{
  if (! saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,t2,d1) VALUES (";
  s.append(QString::number(id));
  s.append(",'" + exchange + "'");
  s.append(",'" + symbol + "'");
  s.append(",'" + indicator + "'");
  s.append(",'" + plugin + "'");
  s.append(",'" + color.name() + "'");
  s.append(",'" + date.toString(Qt::ISODate) + "'");
  s.append("," + QString::number(price));
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  saveFlag = FALSE;
}

void Sell::create ()
{
  emit signalMessage(QString(QObject::tr("Select point to place Sell arrow...")));
}

int Sell::create2 (QDateTime &x, double y)
{
  saveFlag = TRUE;
  date = x;
  price = y;
  emit signalMessage(QString());
  return 0;
}

void Sell::moving (QDateTime &x, double y, int)
{
  saveFlag = TRUE;
  date = x;
  price = y;
  
  emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
}

void Sell::getIcon (QIcon &d)
{
  d = QIcon(sellarrow_xpm);
}

int Sell::getHighLow (double &h, double &l)
{
  h = price;
  l = price;
  return 0;
}

int Sell::inDateRange (QDateTime &startDate, QDateTime &endDate, DateBar &)
{
  int rc = FALSE;
  if (date >= startDate && date <= endDate)
    rc = TRUE;
  
  return rc;
}

