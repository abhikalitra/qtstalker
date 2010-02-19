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

#include <QPainter>

Text::Text ()
{
  type = (int) COText;
  label = "Text";
}

void Text::draw (QPixmap &buffer, BarData *data, int startX, int pixelspace, int startIndex,
		     Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  painter.setFont(font);

  painter.setPen(color);

  int y = scaler.convertToY(price);

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

void Text::getInfo (Setting &info)
{
  QString k = tr("Type");
  QString d = tr("Text");
  info.setData(k, d);

  k = tr("Label");
  info.setData(k, label);
}

void Text::dialog ()
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

  s = tr("Price");
  dialog->addDoubleItem(pid++, page, s, price);

  s = tr("Label");
  dialog->addTextItem(pid++, page, s, label);

  s = tr("Font");
  dialog->addFontItem(pid++, page, s, font);

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
  price = dialog->getDouble(pid++);
  dialog->getText(pid++, label);
  dialog->getFont(pid++, font);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setBaseData((int) Config::DefaultTextColor, color);
    config.setData(Config::DefaultTextLabel, label);
    config.setBaseData((int) Config::DefaultTextFont, font);
  }

  setSaveFlag(TRUE);
  delete dialog;
}

void Text::setDate (QDateTime &d)
{
  date = d;
}

void Text::setPrice (double d)
{
  price = d;
}

void Text::setSettings (QSqlQuery &q)
{
  id = q.value(0).toString();
  symbol = q.value(1).toString();
  indicator = q.value(2).toString();
  color.setNamedColor(q.value(4).toString());
  date = q.value(5).toDateTime();
  price = q.value(7).toDouble();
  label = q.value(12).toString();

  QStringList l = q.value(11).toString().split(",");
  font.setFamily(l[0]);
  font.setPointSize(l[1].toInt());
  font.setWeight(l[2].toInt());
  font.setItalic(l[3].toInt());
  font.setBold(l[4].toInt());
}

void Text::getSettings (QString &set)
{
  QString f = font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic ()) + "," +
              QString::number(font.bold ());

  set = "INSERT OR REPLACE INTO chartObjects (id,symbol,indicator,type,color,date,price,label,font) VALUES (";
  set.append(id);
  set.append(",'" + symbol + "'");
  set.append(",'" + indicator + "'");
  set.append("," + QString::number(type));
  set.append(",'" + color.name() + "'");
  set.append(",'" + date.toString(Qt::ISODate) + "'");
  set.append("," + QString::number(price));
  set.append(",'" + label + "'");
  set.append(",'" + f + "'");
  set.append(")");
}

void Text::create (QString &_id, QString &_symbol, QString &_indicator)
{
  id = _id;
  symbol = _symbol;
  indicator = _indicator;
  emit signalMessage(QString(tr("Select point to place Text...")));
}

int Text::create2 (QDateTime &x, double y)
{
  setSaveFlag(TRUE);
  date = x;
  price = y;
  return 0;
}

void Text::moving (QDateTime &x, double y, int)
{
  date = x;
  price = y;
  setSaveFlag(TRUE);
  emit signalMessage(QString(date.toString(Qt::ISODate) + " " + QString::number(y)));
}

