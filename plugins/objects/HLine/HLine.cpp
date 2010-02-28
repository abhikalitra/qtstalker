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


#include "HLine.h"
#include "PrefDialog.h"
#include "Config.h"
#include "horizontal.xpm"

#include <QObject>

HLine::HLine ()
{
}

void HLine::draw (ChartObject *co, QPixmap &buffer, BarData *, int startX, int, int, Scaler &scaler)
{
  double price = co->getDouble(ChartObject::ParmPrice);

  // if value is off chart then don't draw it
  if (price < scaler.getLow())
    return;

  QPainter painter;
  painter.begin(&buffer);

  QFont font;
  co->getFont(ChartObject::ParmFont, font);
  painter.setFont(font);

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  painter.setPen(color);

  QString label;
  co->getData(ChartObject::ParmLabel, label);

  int y = scaler.convertToY(price);
  
  painter.drawLine (0, y, buffer.width(), y);

  // test start
  QString s = QString::number(price);
  painter.setBackgroundMode(Qt::OpaqueMode);
  QFontMetrics fm(font);
  QRect rc = painter.boundingRect(startX, y - (fm.height() / 2), 1, 1, 0, s);
  painter.drawText(rc, s);
  painter.drawRect(rc);
  painter.setBackgroundMode(Qt::TransparentMode);
  // test end
  
//  QFontMetrics fm(font);
//  int pixelsWide = fm.width(label);
//  painter.drawLine (0, y, buffer.width(), y);
//  painter.drawText(0, y - 1, label);
//  painter.drawText(0 + pixelsWide + 1, y - 1, QString::number(price));

  co->clearSelectionArea();
  QPolygon array;
  array.putPoints(0, 4, 0, y - 4, 0, y + 4, buffer.width(), y + 4, buffer.width(), y - 4);
  co->setSelectionArea(new QRegion(array));

  if (co->getInt(ChartObject::ParmSelected))
  {
    co->clearGrabHandles();
    int t = (int) buffer.width() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      co->setGrabHandle(new QRegion(t * loop,
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

void HLine::getInfo (ChartObject *co, Setting &info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("HLine");
  info.setData(k, d);

  k = QObject::tr("Price");
  co->getData(ChartObject::ParmPrice, d);
  info.setData(k, d);
}

void HLine::dialog (ChartObject *co)
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit HLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("Price");
  double price = co->getDouble(ChartObject::ParmPrice);
  dialog->addDoubleItem(pid++, page, s, price);

  s = QObject::tr("Label");
  QString label;
  co->getData(ChartObject::ParmLabel, label);
  dialog->addTextItem(pid++, page, s, label);

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

  dialog->getText(pid++, label);
  co->setData(ChartObject::ParmLabel, label);

  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultHLineColor, color);
  }

  co->setData(ChartObject::ParmSave, TRUE);
  
  delete dialog;
}

void HLine::setSettings (ChartObject *co, QSqlQuery &q)
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

  s = q.value(7).toString();
  co->setData(ChartObject::ParmPrice, s);

  s = q.value(12).toString();
  co->setData(ChartObject::ParmLabel, s);
}

void HLine::getSettings (ChartObject *co, QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,color,price,label) VALUES (";

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
  
  co->getData(ChartObject::ParmPrice, s);
  set.append("," + s);
  
  co->getData(ChartObject::ParmLabel, s);
  set.append(",'" + s + "'");
  
  set.append(")");
}

void HLine::create (ChartObject *co)
{
  QString s = "HLine";
  co->setData(ChartObject::ParmPlugin, s);
  co->setData(ChartObject::ParmPrice, 0);

  s = QObject::tr("Select point to place Horizontal Line...");
  co->message(s);
}

int HLine::create2 (ChartObject *co, QDateTime &, double y)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmPrice, y);
  return 0;
}

void HLine::moving (ChartObject *co, QDateTime &, double y, int)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmPrice, y);
  
  QString s = QString::number(y);
  co->message(s);
}

void HLine::getIcon (QIcon &d)
{
  d = QIcon(horizontal_xpm);
}

//*************************************************************
//*************************************************************
//*************************************************************

COPlugin * createCOPlugin ()
{
  HLine *o = new HLine;
  return ((COPlugin *) o);
}


