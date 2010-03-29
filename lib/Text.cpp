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


#include "Text.h"
#include "PrefDialog.h"
#include "Config.h"
#include "CODataBase.h"

#include "../pics/text.xpm"

#include <QObject>
#include <QPainter>

Text::Text ()
{
  plugin = "Text";
  label = plugin;
  color.setNamedColor("red");
  price = 0;
}

void Text::draw (PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = pd.dateBars.getX(date);
  if (x2 == -1)
    return;

  int x = pd.startX + (x2 * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
  if (x == -1)
    return;

  painter.setFont(pd.plotFont);

  painter.setPen(color);

  int y = pd.scaler.convertToY(price);

  painter.drawText(x, y, label);

  QFontMetrics fm = painter.fontMetrics();
  clearSelectionArea();
  setSelectionArea(new QRegion(x,
		   y - fm.height(),
		   fm.width(label, -1),
		   fm.height(),
		   QRegion::Rectangle));

  if (selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - handleWidth - 1,
		  y - (fm.height() / 2),
		  handleWidth,
		  handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x - handleWidth - 1,
		     y - (fm.height() / 2),
		     handleWidth,
		     handleWidth,
		     color);
  }

  painter.end();
}

void Text::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Text");
  info->setData(k, d);

  k = QObject::tr("Label");
  d = label;
  info->setData(k, d);
}

void Text::dialog ()
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

  s = QObject::tr("Price");
  dialog->addDoubleItem(pid++, page, s, price);

  s = QObject::tr("Label");
  dialog->addTextItem(pid++, page, s, label);

  s = QObject::tr("Font");
  dialog->addFontItem(pid++, page, s, font);

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
  dialog->getFont(pid++, font);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultTextColor, color);
    config.setData(Config::DefaultTextLabel, label);
    config.setData((int) Config::DefaultTextFont, font);
  }

  saveFlag = TRUE;
  
  delete dialog;
}

void Text::load (QSqlQuery &q)
{
  id = q.value(0).toInt();
  exchange = q.value(1).toString();
  symbol = q.value(2).toString();
  indicator = q.value(3).toString();
  color.setNamedColor(q.value(5).toString()); // t1 field
  date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate); // t2 field
  label = q.value(7).toString(); // t3 field

  QStringList l = q.value(8).toString().split(","); // t4 field
  font.setFamily(l[0]);
  font.setPointSize(l[1].toInt());
  font.setWeight(l[2].toInt());
  font.setItalic(l[3].toInt());
  font.setBold(l[4].toInt());

  price = q.value(25).toDouble(); // d1 field
}

void Text::save ()
{
  if (! saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,t2,d1,t3,t4) VALUES (";
  s.append(QString::number(id));
  s.append(",'" + exchange + "'");
  s.append(",'" + symbol + "'");
  s.append(",'" + indicator + "'");
  s.append(",'" + plugin + "'");
  s.append(",'" + color.name() + "'");
  s.append(",'" + date.toString(Qt::ISODate) + "'");
  s.append("," + QString::number(price));
  s.append(",'" + label + "'");
  
  QStringList l;
  l << font.family();
  l << QString::number(font.pointSize());
  l << QString::number(font.weight());
  l << QString::number(font.italic ());
  l << QString::number(font.bold ());
  s.append(",'" + l.join(",") + "'");

  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  saveFlag = FALSE;
}

void Text::create ()
{
  emit signalMessage(QString(QObject::tr("Select point to place Text...")));
}

int Text::create2 (QDateTime &x, double y)
{
  saveFlag = TRUE;
  date = x;
  price = y;
  emit signalMessage(QString());
  return 0;
}

void Text::moving (QDateTime &x, double y, int)
{
  saveFlag = TRUE;
  date = x;
  price = y;
  
  emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
}

void Text::getIcon (QIcon &d)
{
  d = QIcon(text_xpm);
}

int Text::getHighLow (double &h, double &l)
{
  h = price;
  l = price;
  return 0;
}

int Text::inDateRange (PlotData &, QDateTime &startDate, QDateTime &endDate)
{
  int rc = FALSE;
  if (date >= startDate && date <= endDate)
    rc = TRUE;
  
  return rc;
}

