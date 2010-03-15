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


#include "VLine.h"
#include "PrefDialog.h"
#include "Config.h"
#include "vertical.xpm"

#include <QObject>

VLine::VLine ()
{
}

void VLine::draw (ChartObject *co, QPixmap &buffer, DateBar &data, int startX, int pixelspace,
		  int startIndex, Scaler &)
{
  QPainter painter;
  painter.begin(&buffer);

  QDateTime date;
  co->getDate(ChartObject::ParmDate, date);
  int x2 = data.getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  painter.setPen(color);

  painter.drawLine (x, 0, x, buffer.height());

  co->clearSelectionArea();
  QPolygon array;
  array.putPoints(0,
  	          4,
		  x - (handleWidth / 2), 0,
		  x + (handleWidth / 2), 0,
		  x + (handleWidth / 2), buffer.height(),
		  x - (handleWidth / 2), buffer.height());
  co->setSelectionArea(new QRegion(array));

  if (co->getInt(ChartObject::ParmSelected))
  {
    co->clearGrabHandles();
    int t = (int) buffer.height() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      co->setGrabHandle(new QRegion(x - (handleWidth / 2),
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

void VLine::getInfo (ChartObject *co, Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = "VLine";
  info->setData(k, d);

  k = QObject::tr("Date");
  co->getData(ChartObject::ParmDate, d);
  info->setData(k, d);
}

void VLine::dialog (ChartObject *co)
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit VLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  dialog->addColorItem(pid++, page, s, color);

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

  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultVLineColor, color);
  }

  co->setData(ChartObject::ParmSave, TRUE);
  
  delete dialog;
}

void VLine::setSettings (ChartObject *co, QSqlQuery &q)
{
  QString s = q.value(0).toString();
  co->setData(ChartObject::ParmID, s);
  
  s = q.value(1).toString();
  co->setData(ChartObject::ParmSymbol, s);

  s = q.value(2).toString();
  co->setData(ChartObject::ParmIndicator, s);

  s = q.value(3).toString();
  co->setData(ChartObject::ParmPlugin, s);

  s = q.value(4).toString(); // t1 field
  co->setData(ChartObject::ParmColor, s);

  s = q.value(5).toString(); // t2 field
  co->setData(ChartObject::ParmDate, s);
}

void VLine::getSettings (ChartObject *co, QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,t2) VALUES (";
  
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
  
  set.append(")");
}

void VLine::create (ChartObject *co)
{
  QString s = "VLine";
  co->setData(ChartObject::ParmPlugin, s);

  s = "red";
  co->setData(ChartObject::ParmColor, s);

  s = QObject::tr("Select point to place VLine...");
  co->message(s);
}

int VLine::create2 (ChartObject *co, QDateTime &x, double)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmDate, x);
  return 0;
}

void VLine::moving (ChartObject *co, QDateTime &x, double, int)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmDate, x);
  
  QString s = x.toString(Qt::ISODate);
  co->message(s);
}

void VLine::getIcon (QIcon &d)
{
  d = QIcon(vertical_xpm);
}

int VLine::getHighLow (ChartObject *)
{
  return 1;
}

int VLine::inDateRange (ChartObject *co, QDateTime &startDate, QDateTime &endDate)
{
  int rc = FALSE;
  QDateTime dt;
  co->getDate(ChartObject::ParmDate, dt);
  if (dt >= startDate && dt <= endDate)
    rc = TRUE;
  
  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

COPlugin * createCOPlugin ()
{
  VLine *o = new VLine;
  return ((COPlugin *) o);
}


