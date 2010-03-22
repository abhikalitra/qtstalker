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


#include "HLine.h"
#include "PrefDialog.h"
#include "Config.h"
#include "CODataBase.h"

#include "../pics/horizontal.xpm"

#include <QObject>
#include <QFont>
#include <QPainter>

HLine::HLine ()
{
  plugin = "HLine";
  color.setNamedColor("red");
  price = 0;
  label = plugin;
}

void HLine::draw (PlotData &pd)
{
  // if value is off chart then don't draw it
  if (price < pd.scaler.getLow())
    return;

  QPainter painter;
  painter.begin(&pd.buffer);

  painter.setFont(pd.plotFont);

  painter.setPen(color);

  int y = pd.scaler.convertToY(price);
  
  // test start
  QString s = QString::number(price);
  QFontMetrics fm = painter.fontMetrics();
  QRect rc = painter.boundingRect(pd.startX, y - (fm.height() / 2), 1, 1, 0, s);
  painter.fillRect(rc, pd.backgroundColor); // fill in behind text first
  painter.drawText(rc, s); // draw text
  painter.drawRect(rc); // draw a nice little box around text
  
  painter.drawLine (rc.x() + rc.width(), y, pd.buffer.width(), y);

  clearSelectionArea();
  QPolygon array;
  array.putPoints(0, 4, 0, y - 4, 0, y + 4, pd.buffer.width(), y + 4, pd.buffer.width(), y - 4);
  setSelectionArea(new QRegion(array));

  if (selected)
  {
    clearGrabHandles();
    int t = (int) pd.buffer.width() / 4;
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

void HLine::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("HLine");
  info->setData(k, d);

  k = QObject::tr("Price");
  d = QString::number(price);
  info->setData(k, d);
}

void HLine::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit HLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, color);

  s = QObject::tr("Price");
  dialog->addDoubleItem(pid++, page, s, price);

  s = QObject::tr("Label");
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
  price = dialog->getDouble(pid++);
  dialog->getText(pid++, label);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultHLineColor, color);
  }

  saveFlag = TRUE;
  
  delete dialog;
}

void HLine::load (QSqlQuery &q)
{
  id = q.value(0).toInt();
  symbol = q.value(1).toString();
  indicator = q.value(2).toString();
  color.setNamedColor(q.value(4).toString()); // t1 field
  label = q.value(5).toString(); // t2 field
  price = q.value(24).toDouble(); // d1 field
}

void HLine::save ()
{
  if (! saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,d1,t2) VALUES (";
  s.append(QString::number(id));
  s.append(",'" + symbol + "'");
  s.append(",'" + indicator + "'");
  s.append(",'" + plugin + "'");
  s.append(",'" + color.name() + "'");
  s.append("," + QString::number(price));
  s.append(",'" + label + "'");
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  saveFlag = FALSE;
}

void HLine::create ()
{
  emit signalMessage(QString(QObject::tr("Select point to place Horizontal Line...")));
}

int HLine::create2 (QDateTime &, double y)
{
  saveFlag = TRUE;
  price = y;
  emit signalMessage(QString());
  return 0;
}

void HLine::moving (QDateTime &, double y, int)
{
  saveFlag = TRUE;
  price = y;
  emit signalMessage(QString::number(y));
}

void HLine::getIcon (QIcon &d)
{
  d = QIcon(horizontal_xpm);
}

int HLine::getHighLow (double &h, double &l)
{
  h = price;
  l = price;
  return 0;
}

int HLine::inDateRange (PlotData &, QDateTime &, QDateTime &)
{
  int rc = TRUE;
  return rc;
}

