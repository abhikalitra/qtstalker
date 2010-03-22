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
#include "CODataBase.h"

#include "../pics/vertical.xpm"

#include <QObject>
#include <QPainter>

VLine::VLine ()
{
  plugin = "VLine";
  color.setNamedColor("red");
}

void VLine::draw (PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = pd.dateBars.getX(date);
  if (x2 == -1)
    return;

  int x = pd.startX + (x2 * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
  if (x == -1)
    return;

  painter.setPen(color);

  painter.drawLine (x, 0, x, pd.buffer.height());

  clearSelectionArea();
  QPolygon array;
  array.putPoints(0,
  	          4,
		  x - (handleWidth / 2), 0,
		  x + (handleWidth / 2), 0,
		  x + (handleWidth / 2), pd.buffer.height(),
		  x - (handleWidth / 2), pd.buffer.height());
  setSelectionArea(new QRegion(array));

  if (selected)
  {
    clearGrabHandles();
    int t = (int) pd.buffer.height() / 4;
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

void VLine::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = "VLine";
  info->setData(k, d);

  k = QObject::tr("Date");
  d = date.toString(Qt::ISODate);
  info->setData(k, d);
}

void VLine::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit VLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
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
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultVLineColor, color);
  }

  saveFlag = TRUE;
  
  delete dialog;
}

void VLine::load (QSqlQuery &q)
{
  id = q.value(0).toInt();
  symbol = q.value(1).toString();
  indicator = q.value(2).toString();
  color.setNamedColor(q.value(4).toString()); // t1 field
  date = QDateTime::fromString(q.value(5).toString(), Qt::ISODate); // t2 field
}

void VLine::save ()
{
  if (! saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,t2) VALUES (";
  s.append(QString::number(id));
  s.append(",'" + symbol + "'");
  s.append(",'" + indicator + "'");
  s.append(",'" + plugin + "'");
  s.append(",'" + color.name() + "'");
  s.append(",'" + date.toString(Qt::ISODate) + "'");
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  saveFlag = FALSE;
}

void VLine::create ()
{
  emit signalMessage(QString(QObject::tr("Select point to place VLine...")));
}

int VLine::create2 (QDateTime &x, double)
{
  saveFlag = TRUE;
  date = x;
  emit signalMessage(QString());
  return 0;
}

void VLine::moving (QDateTime &x, double, int)
{
  saveFlag = TRUE;
  date = x;
  
  emit signalMessage(QString(x.toString(Qt::ISODate)));
}

void VLine::getIcon (QIcon &d)
{
  d = QIcon(vertical_xpm);
}

int VLine::getHighLow (double &, double &)
{
  return 1;
}

int VLine::inDateRange (PlotData &, QDateTime &startDate, QDateTime &endDate)
{
  int rc = FALSE;
  if (date >= startDate && date <= endDate)
    rc = TRUE;
  
  return rc;
}

