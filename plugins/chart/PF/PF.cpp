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
  startX = 0;
  indicatorFlag = TRUE;
  helpFile = "pfchartplugin.html";

  loadSettings();  
}

PF::~PF ()
{
}

void PF::drawChart (int startX, int startIndex, int pixelspace)
{
  QMemArray<double> scaleArray;
  scaler->getScaleArray(scaleArray);
  
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

void PF::prefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("PF Chart Prefs"));
  dialog->setHelpFile (helpFile);
  
  QString s = tr("Details");
  dialog->createPage(s);
  QString s2 = tr("Up Color");
  dialog->addColorItem(s2, s, upColor);
  s2 = tr("Down Color");
  dialog->addColorItem(s2, s, downColor);
  s2 = tr("Box Size");
  dialog->addFloatItem(s2, s, size);
  s2 = tr("Reversal");
  dialog->addIntItem(s2, s, PAFReversal);
  s2 = tr("Min Bar Spacing");
  dialog->addIntItem(s2, s, minPixelspace, 4, 99);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    s = tr("Up Color");
    upColor = dialog->getColor(s);
    s = tr("Down Color");
    downColor = dialog->getColor(s);
    s = tr("Box Size");
    size = dialog->getFloat(s);
    s = tr("Reversal");
    PAFReversal = dialog->getInt(s);
    s = tr("Min Bar Spacing");
    minPixelspace = dialog->getInt(s);
    
    saveFlag = TRUE;
    saveSettings();
    emit draw();
  }
  
  delete dialog;
}

void PF::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/PF plugin");

  upColor.setNamedColor(settings.readEntry("/UpColor", "green"));
  downColor.setNamedColor(settings.readEntry("/DownColor", "red"));
  
  QString s = settings.readEntry("/BoxSize", "0");
  size = s.toFloat();

  s = settings.readEntry("/Reversal", "3");
  PAFReversal = s.toInt();

  minPixelspace = settings.readNumEntry("/minPixelspace", 4);

  currentPixelspace = settings.readNumEntry("/pixelspace", 4);
      
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

void PF::savePixelspace ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/PF plugin");
  settings.writeEntry("/pixelspace", currentPixelspace);
  settings.endGroup();
}

//*************************************************
//*************************************************

ChartPlugin * createChartPlugin ()
{
  PF *o = new PF;
  return ((ChartPlugin *) o);
}
