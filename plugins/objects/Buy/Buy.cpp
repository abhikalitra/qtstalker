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


#include "Buy.h"
#include "PrefDialog.h"
#include "Config.h"
#include "buyarrow.xpm"

#include <QObject>

Buy::Buy ()
{
}

void Buy::draw (ChartObject *co, QPixmap &buffer, BarData *data, int startX, int pixelspace,
		int startIndex, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  QDateTime date;
  co->getDate(ChartObject::ParmDate, date);
  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  double price = co->getDouble(ChartObject::ParmPrice);
  int y = scaler.convertToY(price);

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y + 5,
                  x + 2, y + 5,
                  x + 2, y + 11,
                  x - 2, y + 11,
	          x - 2, y + 5,
                  x - 5, y + 5);
  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  painter.setBrush(color);
  
  painter.drawPolygon(arrow, Qt::OddEvenFill);
  
  co->clearSelectionArea();
  co->setSelectionArea(new QRegion(arrow));

  if (co->getInt(ChartObject::ParmSelected))
  {
    co->clearGrabHandles();

    co->setGrabHandle(new QRegion(x - (handleWidth / 2),
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

void Buy::getInfo (ChartObject *co, Setting &info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Buy");
  info.setData(k, d);

  k = QObject::tr("Date");
  co->getData(ChartObject::ParmDate, d);
  info.setData(k, d);

  k = QObject::tr("Price");
  co->getData(ChartObject::ParmPrice, d);
  info.setData(k, d);
}


void Buy::dialog (ChartObject *co)
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Buy");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0 ;
  dialog->addPage(page, s);

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("Price");
  double price = co->getDouble(ChartObject::ParmPrice);
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
  co->setData(ChartObject::ParmColor, color);
  
  price = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmPrice, price);
  
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultBuyColor, color);
  }

  co->setData(ChartObject::ParmSave, TRUE);
  
  delete dialog;
}

void Buy::setSettings (ChartObject *co, QSqlQuery &q)
{
  QString s = q.value(0).toString();
  co->setData(ChartObject::ParmID, s);
  
  s = q.value(1).toString();
  co->setData(ChartObject::ParmSymbol, s);

  s = q.value(2).toString();
  co->setData(ChartObject::ParmIndicator, s);

  s = q.value(3).toString();
  co->setData(ChartObject::ParmPlugin, s);

  s = q.value(4).toString();
  co->setData(ChartObject::ParmColor, s);

  s = q.value(5).toString();
  co->setData(ChartObject::ParmDate, s);

  s = q.value(24).toString();
  co->setData(ChartObject::ParmPrice, s);
}

void Buy::getSettings (ChartObject *co, QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,t2,d1) VALUES (";
  
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
  
  co->getData(ChartObject::ParmDate, s);
  set.append(",'" + s + "'");
  
  co->getData(ChartObject::ParmPrice, s);
  set.append("," + s);
  
  set.append(")");
}

void Buy::create (ChartObject *co)
{
  QString s = "Buy";
  co->setData(ChartObject::ParmPlugin, s);

  s = "green";
  co->setData(ChartObject::ParmColor, s);

  co->setData(ChartObject::ParmPrice, 0);

  s = QObject::tr("Select point to place Buy arrow...");
  co->message(s);
}

int Buy::create2 (ChartObject *co, QDateTime &x, double y)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmDate, x);
  co->setData(ChartObject::ParmPrice, y);
  return 0;
}

void Buy::moving (ChartObject *co, QDateTime &x, double y, int)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmDate, x);
  co->setData(ChartObject::ParmPrice, y);
  
  QString s = x.toString(Qt::ISODate) + " " + QString::number(y);
  co->message(s);
}

void Buy::getIcon (QIcon &d)
{
  d = QIcon(buyarrow_xpm);
}

//*************************************************************
//*************************************************************
//*************************************************************

COPlugin * createCOPlugin ()
{
  Buy *o = new Buy;
  return ((COPlugin *) o);
}


