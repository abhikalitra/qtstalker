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

#include "VerticalLine.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qcolor.h>

VerticalLine::VerticalLine (QPixmap *p, QString indicator, QString name, QString date)
{
  buffer = p;
  
  settings.set("Type", "Vertical Line", Setting::None);
  settings.set("Date", date, Setting::None);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set("ObjectType", QString::number(ChartObject::VerticalLine), Setting::None);
}

VerticalLine::~VerticalLine ()
{
}

void VerticalLine::draw (int x, int)
{
  QPainter painter;
  painter.begin(buffer);

  QColor color(settings.getData(tr("Color")));
  painter.setPen(color);

  painter.drawLine (x, 0, x, buffer->height());
  
  QRegion r(x, 0, 1, buffer->height(), QRegion::Rectangle);
  area = r;
  
  if (status)
  {
    int t = (int) buffer->height() / 4;
    painter.fillRect(x - 3, 0, 6, 6, color);
    painter.fillRect(x - 3, t, 6, 6, color);
    painter.fillRect(x - 3, t * 2, 6, 6, color);
    painter.fillRect(x - 3, t * 3, 6, 6, color);
    painter.fillRect(x - 3, t * 4, 6, 6, color);
  }

  painter.end();
}

QString VerticalLine::getDate ()
{
  return settings.getDateTime("Date");
}

void VerticalLine::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Vertical Line"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), 1, QColor(settings.getData(tr("Color"))));
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QColor color = dialog->getColor(tr("Color"));
    settings.setData(tr("Color"), color.name());
    
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void VerticalLine::move (QString d, QString v)
{
  settings.setData("Date", d);
  settings.setData("Value", v);
  saveFlag = TRUE;
  emit signalDraw();
}

