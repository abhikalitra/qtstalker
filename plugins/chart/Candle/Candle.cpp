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

#include "Candle.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>

Candle::Candle ()
{
  pluginName = "Candle";
  minPixelspace = 6;
  startX = 2;
  indicatorFlag = FALSE;

  loadSettings();  
}

Candle::~Candle ()
{
  if (saveFlag)
    saveSettings();
}

void Candle::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;

  painter.setPen(color);

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    int h = scaler->convertToY(data->getHigh(loop));
    int l = scaler->convertToY(data->getLow(loop));
    int c = scaler->convertToY(data->getClose(loop));
    int o = scaler->convertToY(data->getOpen(loop));

    if (data->getOpen(loop) != 0)
    {
      if (c < o)
      {
        painter.drawRect(x - 2, c, 5, o - c);
        painter.drawLine (x, h, x, c);
        painter.drawLine (x, o, x, l);
      }
      else
      {
        painter.drawLine (x, h, x, l);
      
        if (c == o)
          painter.drawLine (x - 2, o, x + 2, o);
        else
          painter.fillRect(x - 2, o, 5, c - o, painter.pen().color());
      }
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Candle::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Candle Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Candle Color"), 1, color);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Candle Color"));
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void Candle::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");

  QString s = settings.readEntry("/Color", "green");
  color.setNamedColor(s);
  
  settings.endGroup();
}

void Candle::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");
  settings.writeEntry("/Color", color.name());
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  Candle *o = new Candle;
  return ((Plugin *) o);
}

