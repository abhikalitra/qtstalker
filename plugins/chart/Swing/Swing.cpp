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

#include "Swing.h"
#include "PrefDialog.h"
#include <qsettings.h>
#include <qpainter.h>

Swing::Swing ()
{
  pluginName = "Swing";
//  minPixelspace = 4;
  startX = 0;
  indicatorFlag = TRUE;

  loadSettings();  
}

Swing::~Swing ()
{
}

void Swing::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(neutralColor);

  int status = 0;
  int x = startX;
  int loop = startIndex;
  int h;
  int l;
  double high = -99999999;
  double low = 99999999;

  int oldx = x;
  loop++;
  x = x + pixelspace;

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    switch (status)
    {
      case 1:
        if (data->getHigh(loop) < data->getHigh(loop - 1) && data->getLow(loop) < data->getLow(loop - 1))
	{
	  painter.setPen(upColor);
          h = scaler->convertToY(high);
          l = scaler->convertToY(low);
          painter.drawLine (x, h, x, l);

          painter.drawLine (oldx, l, x, l);

	  status = -1;
	  oldx = x;
          low = data->getLow(loop);
	}
	else
	{
          if (data->getHigh(loop) > high)
           high = data->getHigh(loop);
	}
	break;
      case -1:
        if (data->getHigh(loop) > data->getHigh(loop - 1) && data->getLow(loop) > data->getLow(loop - 1))
	{
	  painter.setPen(downColor);
          h = scaler->convertToY(high);
          l = scaler->convertToY(low);
          painter.drawLine (x, h, x, l);

          painter.drawLine (oldx, h, x, h);

	  status = 1;
	  oldx = x;
          high = data->getHigh(loop);
	}
	else
	{
          if (data->getLow(loop) < low)
           low = data->getLow(loop);
        }
	break;
      default:
        if (data->getHigh(loop) < data->getHigh(loop - 1) && data->getLow(loop) < data->getLow(loop - 1))
	{
	  status = -1;
	  oldx = x;
          high = data->getHigh(loop);
          low = data->getLow(loop);
	}
	else
	{
          if (data->getHigh(loop) > data->getHigh(loop - 1) && data->getLow(loop) > data->getLow(loop - 1))
	  {
	    status = 1;
	    oldx = x;
            high = data->getHigh(loop);
            low = data->getLow(loop);
	  }
        }
	break;
    }

    x = x + pixelspace;
    loop++;
  }

  // draw the leftover
  switch (status)
  {
    case 1:
      painter.setPen(upColor);
      h = scaler->convertToY(high);
      l = scaler->convertToY(low);
      painter.drawLine (x, h, x, l);
      painter.drawLine (oldx, l, x, l);
        break;
    case -1:
      painter.setPen(downColor);
      h = scaler->convertToY(high);
      l = scaler->convertToY(low);
      painter.drawLine (x, h, x, l);
      painter.drawLine (oldx, h, x, h);
      break;
    default:
      break;
  }

  painter.end();
}

void Swing::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Swing Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Up Color"), 1, upColor);
  dialog->addColorItem(tr("Down Color"), 1, downColor);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 4, 99);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    upColor = dialog->getColor(tr("Up Color"));
    downColor = dialog->getColor(tr("Down Color"));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    
    saveFlag = TRUE;
    
    emit draw();
  }
  
  delete dialog;
}

void Swing::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Swing plugin");

  QString s = settings.readEntry("/NeutralColor", "blue");
  neutralColor.setNamedColor(s);
  
  s = settings.readEntry("/UpColor", "green");
  upColor.setNamedColor(s);

  s = settings.readEntry("/DownColor", "red");
  downColor.setNamedColor(s);
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 4);
  
  settings.endGroup();
}

void Swing::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Swing plugin");
  
  settings.writeEntry("/NeutralColor", neutralColor.name());
  settings.writeEntry("/UpColor", upColor.name());
  settings.writeEntry("/DownColor", downColor.name());
  settings.writeEntry("/minPixelspace", minPixelspace);
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  Swing *o = new Swing;
  return ((Plugin *) o);
}
