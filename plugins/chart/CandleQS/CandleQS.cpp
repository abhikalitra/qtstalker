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

#include "CandleQS.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>

CandleQS::CandleQS ()
{
  pluginName = "CandleQS";
//  minPixelspace = 6;
  startX = 2;
  indicatorFlag = FALSE;
  expandCandles = FALSE;

  loadSettings();  
}

CandleQS::~CandleQS ()
{
}

void CandleQS::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(neutralColor);

  int x = startX;
  int loop = startIndex;

  int w = (1 + pixelspace) / 2 - 1 ; // width of 1/2 candle
  if (expandCandles)
  {
    if (pixelspace < 5) 
      w = 1;
  }
  
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

  loop++;
  x = x + pixelspace;

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

    h = scaler->convertToY(data->getHigh(loop));
    l = scaler->convertToY(data->getLow(loop));
    c = scaler->convertToY(data->getClose(loop));
    o = scaler->convertToY(data->getOpen(loop));

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

void CandleQS::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("CandleQS Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Neutral Color"), 1, neutralColor);
  dialog->addColorItem(tr("Up Color"), 1, upColor);
  dialog->addColorItem(tr("Down Color"), 1, downColor);
  dialog->addCheckItem(tr("Expand Candles"), 1, expandCandles);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 2, 99);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    neutralColor = dialog->getColor(tr("Neutral Color"));
    upColor = dialog->getColor(tr("Up Color"));
    downColor = dialog->getColor(tr("Down Color"));
    expandCandles = dialog->getCheck(tr("Expand Candles"));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void CandleQS::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CandleQS plugin");

  QString s = settings.readEntry("/NeutralColor", "blue");
  neutralColor.setNamedColor(s);
  
  s = settings.readEntry("/UpColor", "green");
  upColor.setNamedColor(s);

  s = settings.readEntry("/DownColor", "red");
  downColor.setNamedColor(s);
  
  s = settings.readEntry("/ExpandCandles", "0");
  expandCandles = s.toInt();

  minPixelspace = settings.readNumEntry("/minPixelspace", 6);
    
  settings.endGroup();
}

void CandleQS::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/CandleQS plugin");
  
  settings.writeEntry("/NeutralColor", neutralColor.name());
  settings.writeEntry("/UpColor", upColor.name());
  settings.writeEntry("/DownColor", downColor.name());
  settings.writeEntry("/ExpandCandles", QString::number(expandCandles));
  settings.writeEntry("/minPixelspace", minPixelspace);
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  CandleQS *o = new CandleQS;
  return ((Plugin *) o);
}
