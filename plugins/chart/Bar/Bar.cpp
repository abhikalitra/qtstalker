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

#include "Bar.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>

Bar::Bar ()
{
  pluginName = "Bar";
  minPixelspace = 4;
  startX = 2;
  indicatorFlag = FALSE;

  loadSettings();  
}

Bar::~Bar ()
{
  if (saveFlag)
    saveSettings();
}

void Bar::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  
  // set first bar as neutral
  painter.setPen(neutralColor);

  double t = data->getOpen(loop);
  int y;
  if (t)
  {
    y = scaler->convertToY(t);
    painter.drawLine (x - 2, y, x, y);
  }

  y = scaler->convertToY(data->getClose(loop));
  painter.drawLine (x + 2, y, x, y);

  int h = scaler->convertToY(data->getHigh(loop));

  int l = scaler->convertToY(data->getLow(loop));
  painter.drawLine (x, h, x, l);

  x = x + pixelspace;
  loop++;

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    if (data->getClose(loop) > data->getClose(loop - 1))
      painter.setPen(upColor);
    else
    {
      if (data->getClose(loop) < data->getClose(loop - 1))
        painter.setPen(downColor);
      else
        painter.setPen(neutralColor);
    }

    t = data->getOpen(loop);
    if (t)
    {
      y = scaler->convertToY(t);
      painter.drawLine (x - 2, y, x, y);
    }

    y = scaler->convertToY(data->getClose(loop));
    painter.drawLine (x + 2, y, x, y);

    h = scaler->convertToY(data->getHigh(loop));
    l = scaler->convertToY(data->getLow(loop));
    painter.drawLine (x, h, x, l);

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Bar::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Bar Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Neutral Color"), 1, neutralColor);
  dialog->addColorItem(tr("Up Color"), 1, upColor);
  dialog->addColorItem(tr("Down Color"), 1, downColor);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    neutralColor = dialog->getColor(tr("Neutral Color"));
    upColor = dialog->getColor(tr("Up Color"));
    downColor = dialog->getColor(tr("Down Color"));
    
    saveFlag = TRUE;
    
    emit draw();
  }
  
  delete dialog;
}

void Bar::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Bar plugin");

  QString s = settings.readEntry("/NeutralColor", "blue");
  neutralColor.setNamedColor(s);
  
  s = settings.readEntry("/UpColor", "green");
  upColor.setNamedColor(s);

  s = settings.readEntry("/DownColor", "red");
  downColor.setNamedColor(s);
  
  settings.endGroup();
}

void Bar::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Bar plugin");
  
  settings.writeEntry("/NeutralColor", neutralColor.name());
  settings.writeEntry("/UpColor", upColor.name());
  settings.writeEntry("/DownColor", downColor.name());
  
  settings.endGroup();
}

//*************************************************
//*************************************************
  
Plugin * create ()
{
  Bar *o = new Bar;
  return ((Plugin *) o);
}
