/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include <qpainter.h>
#include <qfontmetrics.h>

Text::Text (Scaler *s, QPixmap *p, QString indicator, QString n, QDateTime d, double v)
{
  scaler = s;
  buffer = p;
  type = ChartObject::Text;
  plot = indicator;
  name = n;
  date = d;
  value = v;
  color.setNamedColor("white");
  label = tr("Text");
  
  menu->insertItem(tr("Edit Text"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Delete Text"), this, SLOT(remove()));
}

Text::~Text ()
{
}

void Text::draw (int x, int)
{
  QPainter painter;
  painter.begin(buffer);
//  painter.setFont(plotFont);
  painter.setPen(color);

  int y = scaler->convertToY(value);

  painter.drawText(x, y, label);
  
  QFontMetrics fm = painter.fontMetrics();
  QRegion r(x, y - fm.height(), fm.width(label, -1), fm.height(), QRegion::Rectangle);
  area = r;
  
  if (status)
  {
    QRect r = area.boundingRect();
    painter.drawRect(r.topLeft().x() - 2, r.topLeft().y() - 2, r.width() + 4, r.height() + 4);
  }

  painter.end();
}

void Text::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Text"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addTextItem(tr("Label"), 1, label);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    label = dialog->getText(tr("Label"));
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void Text::move (QDateTime d, double v)
{
  date = d;
  value = v;
  saveFlag = TRUE;
  emit signalDraw();
  
  QString s = d.toString("yyyyMMdd ");
  s.append(QString::number(v));
  emit message(s);
}

Setting * Text::getSettings ()
{
  Setting *set = new Setting;
  set->set("Date", date.toString("yyyy-MM-dd00:00:00"), Setting::None);
  set->set("Value", QString::number(value), Setting::None);
  set->set("Color", color.name(), Setting::Color);
  set->set("Plot", plot, Setting::None);
  set->set("Name", name, Setting::None);
  set->set("ObjectType", QString::number(type), Setting::None);
  set->set("Label", label, Setting::None);
  return set;
}

void Text::setSettings (Setting *set)
{
  date = QDateTime::fromString(set->getData("Date"), Qt::ISODate);
  value = set->getFloat("Value");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
  label = set->getData("Label");
}

