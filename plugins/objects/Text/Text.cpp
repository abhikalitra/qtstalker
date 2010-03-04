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


#include "Text.h"
#include "PrefDialog.h"
#include "Config.h"
#include "text.xpm"

#include <QObject>

Text::Text ()
{
}

void Text::draw (ChartObject *co, QPixmap &buffer, DateBar &data, int startX, int pixelspace,
		 int startIndex, Scaler &scaler)
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

  QFont font;
  co->getFont(ChartObject::ParmFont, font);
  painter.setFont(font);

  QColor color;
  co->getColor(ChartObject::ParmColor, color);
  painter.setPen(color);

  double price = co->getDouble(ChartObject::ParmPrice);
  int y = scaler.convertToY(price);

  QString label;
  co->getData(ChartObject::ParmLabel, label);
  painter.drawText(x, y, label);

  QFontMetrics fm = painter.fontMetrics();
  co->clearSelectionArea();
  co->setSelectionArea(new QRegion(x,
		       y - fm.height(),
		       fm.width(label, -1),
		       fm.height(),
		       QRegion::Rectangle));

  if (co->getInt(ChartObject::ParmSelected))
  {
    co->clearGrabHandles();

    co->setGrabHandle(new QRegion(x - handleWidth - 1,
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

void Text::getInfo (ChartObject *co, Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Text");
  info->setData(k, d);

  k = QObject::tr("Label");
  co->getData(ChartObject::ParmLabel, d);
  info->setData(k, d);
}

void Text::dialog (ChartObject *co)
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit VLine");
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

  s = QObject::tr("Font");
  QFont font;
  co->getFont(ChartObject::ParmFont, font);
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
  co->setData(ChartObject::ParmColor, color);
  
  price = dialog->getDouble(pid++);
  co->setData(ChartObject::ParmPrice, price);
  
  dialog->getText(pid++, label);
  co->setData(ChartObject::ParmLabel, label);

  dialog->getFont(pid++, font);
  co->setData(ChartObject::ParmFont, font);

  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultTextColor, color);
    config.setData(Config::DefaultTextLabel, label);
    config.setBaseData((int) Config::DefaultTextFont, font);
  }

  co->setData(ChartObject::ParmSave, TRUE);
  
  delete dialog;
}

void Text::setSettings (ChartObject *co, QSqlQuery &q)
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

  s = q.value(6).toString(); // t3 field
  co->setData(ChartObject::ParmLabel, s);

  s = q.value(7).toString(); // t4 field
  co->setData(ChartObject::ParmFont, s);

  s = q.value(24).toString(); // d1 field
  co->setData(ChartObject::ParmPrice, s);
}

void Text::getSettings (ChartObject *co, QString &set)
{
  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,plugin,t1,t2,d1,t3,t4) VALUES (";
  
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
  
  co->getData(ChartObject::ParmLabel, s);
  set.append(",'" + s + "'");

  co->getData(ChartObject::ParmFont, s);
  set.append(",'" + s + "'");

  set.append(")");
}

void Text::create (ChartObject *co)
{
  QString s = "Text";
  co->setData(ChartObject::ParmPlugin, s);

  co->setData(ChartObject::ParmLabel, s);

  s = "white";
  co->setData(ChartObject::ParmColor, s);

  s = QObject::tr("Select point to place Text...");
  co->message(s);
}

int Text::create2 (ChartObject *co, QDateTime &x, double y)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmDate, x);
  co->setData(ChartObject::ParmPrice, y);
  return 0;
}

void Text::moving (ChartObject *co, QDateTime &x, double y, int)
{
  co->setData(ChartObject::ParmSave, TRUE);
  co->setData(ChartObject::ParmDate, x);
  co->setData(ChartObject::ParmPrice, y);
  
  QString s = x.toString(Qt::ISODate) + " " + QString::number(y);
  co->message(s);
}

void Text::getIcon (QIcon &d)
{
  d = QIcon(text_xpm);
}

int Text::getHighLow (ChartObject *co)
{
  double price = co->getDouble(ChartObject::ParmPrice);
  co->setData(ChartObject::ParmHigh, price);
  co->setData(ChartObject::ParmLow, price);
  
  return 0;
}

int Text::inDateRange (ChartObject *co, QDateTime &startDate, QDateTime &endDate)
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
  Text *o = new Text;
  return ((COPlugin *) o);
}


