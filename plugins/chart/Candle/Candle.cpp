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
  startX = 2;
  indicatorFlag = FALSE;
  expandCandles = FALSE;

  loadSettings();
}

Candle::~Candle ()
{
}

// find out which chart style to draw
void Candle::drawChart (int startX, int startIndex, int pixelspace)
{
  if (! style.compare(tr("Candle")))
    drawCandles(startX, startIndex, pixelspace);
  else
  {
    if (! style.compare(tr("Candle QS")))
      drawQSCandles(startX, startIndex, pixelspace);
    else
      drawVolumeCandles(startX, startIndex, pixelspace);
  }
}

// draw traditional candles
void Candle::drawCandles (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(candleColor);

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

// draw qs candles 
void Candle::drawQSCandles (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(qsNeutralColor);

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
      painter.setPen(qsUpColor);
    else
    {
      if (data->getClose(loop) < data->getClose(loop - 1))
        painter.setPen(qsDownColor);
      else
        painter.setPen(qsNeutralColor);
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

//***********************************************************
//***************** VOLUME CANDLE FUNCTIONS *****************
//***********************************************************

QColor Candle::volumeColor (int index) // function that returns color to use for each candle
{
  int loop;
  
  double v, vm, vr; // today's volume, ma of volume, and ratio of v/vm
  QColor c;        // stores return value for this function
  
  v = data->getVolume(index); // today's volume
  
  // calculate the ma of volume  
  vm = v;
  loop = 1;
  while ( (loop < vma) && ( (index-loop)>0) ) // make sure not to go back too far
  {
    vm = vm + data->getVolume(index - loop);
    loop++;
  }
  vm = vm/loop; // total cumulative volume over period  - note that loop may be less than vma
  
  if (vm == 0) 
    vr = 1;
  else 
    vr = v/vm;    // we now have our ratio - need now to compare to trigger points and asign a color
  
  // assign the appropriate color to this candle
  c = c0;  // normal default color
  if (vr < vr1) c = c1;
  if (vr > vr2) c = c2;
  if (vr > vr3) c = c3;
  if (vr > vr4) c = c4;
  if (vr > vr5) c = c5;
  
  return c;
}

void Candle::drawVolumeCandles (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  
  int w; // candle radius
  
  w = fixedCandleRadius;
  
  if (pixelspace < minPixelspace)
    pixelspace = minPixelspace;
  
  if (expandCandles)
  {
    w = minCandleRadius;
    while ((pixelspace-(1+2*w)) > maxCandleGap) w++;
  }
  
  painter.setPen(c0);

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    int h = scaler->convertToY(data->getHigh(loop));
    int l = scaler->convertToY(data->getLow(loop));
    int c = scaler->convertToY(data->getClose(loop));
    int o = scaler->convertToY(data->getOpen(loop));
    
    painter.setPen(volumeColor(loop));

    if (data->getOpen(loop) != 0)
    {
      if (c < o)
      {
        painter.drawRect(x - w, c, 1+2*w, o - c);
        painter.drawLine (x, h, x, c);
        painter.drawLine (x, o, x, l);
        
      }
      else
      {
        painter.drawLine (x, h, x, l);
      
        if (c == o)
          painter.drawLine (x - w, o, x + w, o);
        else
          painter.fillRect(x - w, o, 1+2*w, c - o, painter.pen().color());
       }
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

//******************************************************************
//******************** PREFS FUNCTIONS *****************************
//******************************************************************

void Candle::prefDialog ()
{
  QStringList l;
  l.append(tr("Candle"));
  l.append(tr("Candle QS"));
  l.append(tr("Volume Candle"));

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Candle Chart Prefs"));
  dialog->createPage (tr("Prefs"));
  dialog->addComboItem(tr("Style"), 1, l, style);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 2, 99);
  dialog->addCheckItem(tr("Expand Candles"), 1, expandCandles);

  // candle settings  
  dialog->createPage (tr("Candle Colors"));
  dialog->addColorItem(tr("Candle Color"), 2, candleColor);
  dialog->addColorItem(tr("Neutral Color"), 2, qsNeutralColor);
  dialog->addColorItem(tr("Up Color"), 2, qsUpColor);
  dialog->addColorItem(tr("Down Color"), 2, qsDownColor);
  
  // volume settings
  dialog->createPage (tr("Volume Candle"));                         
  dialog->addIntItem (tr("Volume MA Periods"), 3, vma);        // ma periods
  dialog->addFloatItem (tr("Volume Slow factor"   ), 3, vr1 );
  dialog->addFloatItem (tr("Volume Active factor" ), 3, vr2 );
  dialog->addFloatItem (tr("Volume Hot factor"    ), 3, vr3 );            // trigger points
  dialog->addFloatItem (tr("Volume Fire factor"   ), 3, vr4 );
  dialog->addFloatItem (tr("Volume Crazy factor"  ), 3, vr5 );
  dialog->addIntItem (tr("Fixed Candle Radius (pixels)"), 3, fixedCandleRadius);
  dialog->addIntItem (tr("Minimum Candle Radius (pixels)"), 3, minCandleRadius);
  dialog->addIntItem (tr("Max Gap between Candles (pixels)"), 3, maxCandleGap);
  
  // tab to set candle colors
  dialog->createPage (tr("Volume Colors"));                            
  dialog->addColorItem(tr("Volume Normal color" ), 4, c0 );
  dialog->addColorItem(tr("Volume Slow color"   ), 4, c1 );
  dialog->addColorItem(tr("Volume Active color" ), 4, c2 );                // candle colors
  dialog->addColorItem(tr("Volume Hot color"    ), 4, c3 );
  dialog->addColorItem(tr("Volume Fire color"   ), 4, c4 );
  dialog->addColorItem(tr("Volume Crazy color"  ), 4, c5 );

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    style = dialog->getCombo(tr("Style"));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    expandCandles = dialog->getCheck(tr("Expand Candles"));
  
    // candle settings
    candleColor = dialog->getColor(tr("Candle Color"));
    qsNeutralColor = dialog->getColor(tr("Neutral Color"));
    qsUpColor = dialog->getColor(tr("Up Color"));
    qsDownColor = dialog->getColor(tr("Down Color"));
    
    // volume settings
    vma = dialog->getInt(tr("Volume MA Periods")); if (vma < 2) vma=2;
    vr1 = dialog->getFloat(tr("Volume Slow factor"));
    vr2 = dialog->getFloat(tr("Volume Active factor"));
    vr3 = dialog->getFloat(tr("Volume Hot factor"));
    vr4 = dialog->getFloat(tr("Volume Fire factor"));
    vr5 = dialog->getFloat(tr("Volume Crazy factor"));
    
    c1 = dialog->getColor(tr("Volume Slow color"));
    c0 = dialog->getColor(tr("Volume Normal color"));
    c2 = dialog->getColor(tr("Volume Active color"));
    c3 = dialog->getColor(tr("Volume Hot color"));
    c4 = dialog->getColor(tr("Volume Fire color"));
    c5 = dialog->getColor(tr("Volume Crazy color"));
    
    fixedCandleRadius = dialog->getInt(tr("Fixed Candle Radius (pixels)"));
    minCandleRadius   = dialog->getInt(tr("Minimum Candle Radius (pixels)"));
    maxCandleGap      = dialog->getInt(tr("Max Gap between Candles (pixels)"));
    
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void Candle::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");

  QString s = settings.readEntry("/ExpandCandles", "0");
  expandCandles = s.toInt();
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 2);
  
  // candle settings
  s = settings.readEntry("/candleColor", "green");
  candleColor.setNamedColor(s);
  
  // qs settings
  s = settings.readEntry("/qsNeutralColor", "blue");
  qsNeutralColor.setNamedColor(s);
  
  s = settings.readEntry("/qsUpColor", "green");
  qsUpColor.setNamedColor(s);

  s = settings.readEntry("/qsDownColor", "red");
  qsDownColor.setNamedColor(s);

  // volume settings  
  vma = settings.readNumEntry("/vma", 20);
  vr1 = settings.readDoubleEntry("/vr1", 0.5);
  vr2 = settings.readDoubleEntry("/vr2", 1.5);
  vr3 = settings.readDoubleEntry("/vr3", 3.0);
  vr4 = settings.readDoubleEntry("/vr4", 5.0);
  vr5 = settings.readDoubleEntry("/vr5", 10.0);
  
  s = settings.readEntry("/c0", "black");   c0.setNamedColor(s);
  s = settings.readEntry("/c1", "#5A5A5A"); c1.setNamedColor(s);
  s = settings.readEntry("/c2", "darkRed"); c2.setNamedColor(s);
  s = settings.readEntry("/c3", "red");     c3.setNamedColor(s);
  s = settings.readEntry("/c4", "yellow");  c4.setNamedColor(s);
  s = settings.readEntry("/c5", "white");   c5.setNamedColor(s);
  
  fixedCandleRadius = settings.readNumEntry("/fixedCandleRadius",2);  if(fixedCandleRadius < 1) fixedCandleRadius = 1;
  minCandleRadius   = settings.readNumEntry("/minCandleRadius",1);    if(minCandleRadius < 1) minCandleRadius = 1;
  maxCandleGap      = settings.readNumEntry("/maxCandleGap",1);       if(maxCandleGap < 0) maxCandleGap = 0;
  
  settings.endGroup();
}

void Candle::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");

  settings.writeEntry("/ExpandCandles", QString::number(expandCandles));
  settings.writeEntry("/minPixelspace", minPixelspace);
    
  //****** candle settings
  settings.writeEntry("/candleColor", candleColor.name());
  
  // ********* qs settings
  settings.writeEntry("/qsNeutralColor", qsNeutralColor.name());
  settings.writeEntry("/qsUpColor", qsUpColor.name());
  settings.writeEntry("/qsDownColor", qsDownColor.name());
  
  //******* volume settings
  settings.writeEntry("/vma", vma);
  settings.writeEntry("/vr1", vr1);
  settings.writeEntry("/vr2", vr2);
  settings.writeEntry("/vr3", vr3);
  settings.writeEntry("/vr4", vr4);
  settings.writeEntry("/vr5", vr5);
  
  settings.writeEntry("/c0", c0.name());
  settings.writeEntry("/c1", c1.name());
  settings.writeEntry("/c2", c2.name());
  settings.writeEntry("/c3", c3.name());
  settings.writeEntry("/c4", c4.name());
  settings.writeEntry("/c5", c5.name());
  
  settings.writeEntry("/minCandleRadius"  , minCandleRadius);
  settings.writeEntry("/fixedCandleRadius", fixedCandleRadius);
  settings.writeEntry("/maxCandleGap"     , maxCandleGap);
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  Candle *o = new Candle;
  return ((Plugin *) o);
}

