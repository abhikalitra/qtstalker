/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "Line.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>

Line::Line ()
{
  pluginName = "Line";
  minPixelspace = 3;
  startX = 0;
  indicatorFlag = FALSE;

  loadSettings();  
}

Line::~Line ()
{
  if (saveFlag)
    saveSettings();
}

void Line::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = startIndex;

  painter.setPen(color);

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    y2 = scaler->convertToY(data->getClose(loop));
    if (y != -1)
      painter.drawLine (x, y, x2, y2);
    x = x2;
    y = y2;

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();
}

void Line::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Line Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Line Color"), 1, color);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Line Color"));
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void Line::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Line plugin");

  QString s = settings.readEntry("/Color", "green");
  color.setNamedColor(s);
  
  settings.endGroup();
}

void Line::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Line plugin");
  
  settings.writeEntry("/Color", color.name());
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  Line *o = new Line;
  return ((Plugin *) o);
}

