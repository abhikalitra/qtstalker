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
#include <qcolor.h>
#include <qfontmetrics.h>

Text::Text (Scaler *s, QPixmap *p, QString indicator, QString name, QString date, QString value)
{
  scaler = s;
  buffer = p;
  
  settings.set("Type", "Text", Setting::None);
  settings.set("Date", date, Setting::None);
  settings.set("Value", value, Setting::None);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set(tr("Label"), tr("Text"), Setting::Text);
//  settings.set(tr("Font"), " ", Setting::Font);
  settings.set("ObjectType", QString::number(ChartObject::Text), Setting::None);
}

Text::~Text ()
{
}

void Text::draw (int x, int)
{
  QPainter painter;
  painter.begin(buffer);
//  painter.setFont(plotFont);

  int y = scaler->convertToY(settings.getFloat(tr("Value")));

  QColor color(settings.getData(tr("Color")));
  painter.setPen(color);

  QString s = settings.getData(tr("Label"));
  painter.drawText(x, y, s);
  
  QFontMetrics fm = painter.fontMetrics();
  QRegion r(x, y - fm.height(), fm.width(s, -1), fm.height(), QRegion::Rectangle);
  area = r;
  
  if (status)
  {
    QRect r = area.boundingRect();
    painter.drawRect(r.topLeft().x() - 2, r.topLeft().y() - 2, r.width() + 4, r.height() + 4);
  }

  painter.end();
}

QString Text::getDate ()
{
  return settings.getDateTime(tr("Date"));
}

void Text::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Text"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), 1, QColor(settings.getData(tr("Color"))));
  dialog->addTextItem(tr("Label"), 1, settings.getData(tr("Label")));
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QColor color = dialog->getColor(tr("Color"));
    settings.setData(tr("Color"), color.name());
    
    settings.setData(tr("Label"), dialog->getText(tr("Label")));
    
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void Text::move (QString d, QString v)
{
  settings.setData("Date", d);
  settings.setData("Value", v);
  saveFlag = TRUE;
  emit signalDraw();
}

