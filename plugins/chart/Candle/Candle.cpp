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
//  minPixelspace = 2;
  startX = 2;
  indicatorFlag = FALSE;
  expandCandles = FALSE;

  loadSettings();
}

Candle::~Candle ()
{
}

void Candle::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(color);

  int x = startX;
  int loop = startIndex;
  
  int w = (1 + pixelspace) / 2 - 1 ; // width of 1/2 candle
  if (expandCandles)
  {
    if (pixelspace < 5) 
      w = 1;
  }
  
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
        if (expandCandles)
          painter.drawRect(x - w, c, 1+2*w, o - c);
	else
          painter.drawRect(x - 2, c, 5, o - c);
	
        painter.drawLine (x, h, x, c);
        painter.drawLine (x, o, x, l);
      }
      else
      {
        painter.drawLine (x, h, x, l);
      
        if (c == o)
	{
	  if (expandCandles)
            painter.drawLine (x - w, o, x + w, o);
	  else
            painter.drawLine (x - 2, o, x + 2, o);
	}
        else
	{
	  if (expandCandles)
            painter.fillRect(x - w, o, 1+2*w, c - o, painter.pen().color());
	  else
            painter.fillRect(x - 2, o, 5, c - o, painter.pen().color());
	}
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
  dialog->addCheckItem(tr("Expand Candles"), 1, expandCandles);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 2, 99);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Candle Color"));
    expandCandles = dialog->getCheck(tr("Expand Candles"));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    
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
  
  s = settings.readEntry("/ExpandCandles", "0");
  expandCandles = s.toInt();
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 6);
  
  settings.endGroup();
}

void Candle::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");
  settings.writeEntry("/Color", color.name());
  settings.writeEntry("/ExpandCandles", QString::number(expandCandles));
  settings.writeEntry("/minPixelspace", minPixelspace);
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  Candle *o = new Candle;
  return ((Plugin *) o);
}

