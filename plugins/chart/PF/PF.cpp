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

#include "PF.h"
#include "PrefDialog.h"
#include <qsettings.h>
#include <qpainter.h>

PF::PF ()
{
  pluginName = "PF";
//  minPixelspace = 4;
  startX = 0;
  indicatorFlag = TRUE;

  loadSettings();  
}

PF::~PF ()
{
}

void PF::drawChart (int startX, int startIndex, int pixelspace)
{
  QArray<double> scaleArray = scaler->getScaleArray();
  
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int x2 = startX;
  int loop = startIndex;
  if (size == 0)
    size = (scaleArray[1] - scaleArray[0]) / 4.0;
  int symbol;

  double ph = data->getHigh(loop);
  double pl = data->getLow(loop);
  double t2 = data->getClose(loop);
  if (((ph - pl) / 2) + pl > t2)
    symbol = TRUE;
  else
    symbol = FALSE;

  int t = (int) (ph / size);
  ph = t * size;

  t = (int) (pl / size);
  pl = t * size;

  loop++;
  x2 = x2 + pixelspace;

  while ((x2 < buffer->width()) && (loop < (int) data->count()))
  {
    double h = data->getHigh(loop);
    double l = data->getLow(loop);

    if (! symbol)
    {
      if (l <= (pl - size))
      {
        t = (int) (l / size);
        pl = t * size;
      }
      else
      {
        if (h >= (pl + ((PAFReversal + 1) * size)))
        {
	  int y = scaler->convertToY(ph);
	  int y2= scaler->convertToY(pl);
          painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, downColor);

	  double val = ph - size;
	  while (val > pl)
	  {
	    y = scaler->convertToY(val);
            painter.drawLine (x, y, x2 + pixelspace, y);
            val = val - size;
	  }

	  x = x2 + pixelspace;

          symbol = TRUE;
          pl = pl + size;
          int t = (int) (h / size);
          ph = t * size;
        }
      }
    }
    else
    {
      if (h >= (ph + size))
      {
        int t = (int) (h / size);
        ph = t * size;
      }
      else
      {
        if (l <= (ph - ((PAFReversal + 1) * size)))
        {
	  int y = scaler->convertToY(ph);
	  int y2= scaler->convertToY(pl);
          painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, upColor);

	  double val = ph - size;
	  while (val > pl)
	  {
	    y = scaler->convertToY(val);
            painter.drawLine (x, y, x2 + pixelspace, y);
            val = val - size;
	  }

	  x = x2 + pixelspace;

          symbol = FALSE;
          ph = ph - size;
          t = (int) (l / size);
          pl = t * size;
        }
      }
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  int y = scaler->convertToY(ph);
  int y2= scaler->convertToY(pl);
  if (! symbol)
    painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, downColor);
  else
    painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, upColor);
  double val = ph - size;
  while (val > pl)
  {
    y = scaler->convertToY(val);
    painter.drawLine (x, y, x2 + pixelspace, y);
    val = val - size;
  }

  painter.end();
}

void PF::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("PF Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Up Color"), 1, upColor);
  dialog->addColorItem(tr("Down Color"), 1, downColor);
  
  dialog->createPage (tr("Box Options"));
  dialog->addFloatItem(tr("Box Size"), 2, size);
  dialog->addIntItem(tr("Reversal"), 2, PAFReversal);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 4, 99);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    upColor = dialog->getColor(tr("Up Color"));
    downColor = dialog->getColor(tr("Down Color"));
    size = dialog->getFloat(tr("Box Size"));
    PAFReversal = dialog->getInt(tr("Reversal"));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void PF::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/PF plugin");

  QString s = settings.readEntry("/UpColor", "green");
  upColor.setNamedColor(s);
  
  s = settings.readEntry("/DownColor", "red");
  downColor.setNamedColor(s);
  
  s = settings.readEntry("/BoxSize", "0");
  size = s.toFloat();

  s = settings.readEntry("/Reversal", "3");
  PAFReversal = s.toInt();

  minPixelspace = settings.readNumEntry("/minPixelspace", 4);
    
  settings.endGroup();
}

void PF::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/PF plugin");
  
  settings.writeEntry("/UpColor", upColor.name());
  settings.writeEntry("/DownColor", downColor.name());
  settings.writeEntry("/BoxSize", QString::number(size));
  settings.writeEntry("/Reversal", QString::number(PAFReversal));
  settings.writeEntry("/minPixelspace", minPixelspace);
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  PF *o = new PF;
  return ((Plugin *) o);
}
