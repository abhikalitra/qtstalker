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
#include <qpainter.h>
#include <qsettings.h>

Candle::Candle ()
{
  pluginName = "Candle";
  startX = 2;
  indicatorFlag = FALSE;
  expandCandles = FALSE;
  dialog = 0;
  helpFile = "candlechartplugin.html";

  loadSettings();
}

Candle::~Candle ()
{
}

// find out which chart style to draw
void Candle::drawChart (int startX, int startIndex, int pixelspace)
{
  if (! style.compare("Candle"))
    drawCandles(startX, startIndex, pixelspace);
  else
  {
    if (! style.compare("Candle QS"))
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

void Candle::prefDialog (QWidget *w)
{
  QStringList l;
  l.append("Candle");
  l.append("Candle QS");
  l.append("Volume Candle");

  dialog = new PrefDialog(w);
  dialog->setCaption(tr("Candle Chart Prefs"));
  dialog->setHelpFile (helpFile);
  
  QString s = tr("Prefs");
  dialog->createPage(s);
  
  QString s2 = tr("Style");
  dialog->addComboItem(s2, s, l, style);
  QObject::connect(dialog->getComboWidget(s2),
                   SIGNAL(activated(const QString &)),
                   this,
		   SLOT(styleChanged(const QString &)));
  
  s2 = tr("Min Bar Spacing");
  dialog->addIntItem(s2, s, minPixelspace, 2, 99);
  
  s2 = tr("Expand Candles");
  dialog->addCheckItem(s2, s, expandCandles);
  
  styleChanged(style);

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    s = tr("Style");
    style = dialog->getCombo(s);
    s = tr("Min Bar Spacing");
    minPixelspace = dialog->getInt(s);
    s = tr("Expand Candles");
    expandCandles = dialog->getCheck(s);
    
    if (! style.compare("Candle"))
    {
      s = tr("Candle Color");
      candleColor = dialog->getColor(s);
    }
    else
    {
      if (! style.compare("Candle QS"))
      {
        s = tr("Neutral Color");
        qsNeutralColor = dialog->getColor(s);
	s = tr("Up Color");
        qsUpColor = dialog->getColor(s);
	s = tr("Down Color");
        qsDownColor = dialog->getColor(s);
      }
      else
      {
        s = tr("Volume MA Periods");
        vma = dialog->getInt(s);
	s = tr("Volume Slow factor");
        vr1 = dialog->getFloat(s);
	s = tr("Volume Active factor");
        vr2 = dialog->getFloat(s);
	s = tr("Volume Hot factor");
        vr3 = dialog->getFloat(s);
	s = tr("Volume Fire factor");
        vr4 = dialog->getFloat(s);
	s = tr("Volume Crazy factor");
        vr5 = dialog->getFloat(s);
    
	s = tr("Volume Slow color");
        c1 = dialog->getColor(s);
	s = tr("Volume Normal color");
        c0 = dialog->getColor(s);
	s = tr("Volume Active color");
        c2 = dialog->getColor(s);
	s = tr("Volume Hot color");
        c3 = dialog->getColor(s);
	s = tr("Volume Fire color");
        c4 = dialog->getColor(s);
	s = tr("Volume Crazy color");
        c5 = dialog->getColor(s);
	
	s = tr("Fixed Candle Radius (pixels)");
        fixedCandleRadius = dialog->getInt(s);
	s = tr("Minimum Candle Radius (pixels)");
        minCandleRadius = dialog->getInt(s);
	s = tr("Max Gap between Candles (pixels)");
        maxCandleGap = dialog->getInt(s);
      }
    }
    
    saveFlag = TRUE;
    saveSettings();
    emit draw();
  }
  
  delete dialog;
  dialog = 0;
}

void Candle::styleChanged (const QString &)
{
  if (! dialog)
    return;

  QString s = tr("Style");
  style = dialog->getCombo(s);
    
  if (! style.compare("Candle"))
  {
    s = tr("Volume Candle");
    dialog->deletePage(s);
    s = tr("Volume Colors");
    dialog->deletePage(s);
    s = tr("Color");
    dialog->deletePage(s);
    
    // candle settings  
    s = tr("Color");
    dialog->createPage(s);
    QString s2 = tr("Candle Color");
    dialog->addColorItem(s2, s, candleColor);
    return;
  }
  
  if (! style.compare("Candle QS"))
  {
    s = tr("Volume Candle");
    dialog->deletePage(s);
    s = tr("Volume Colors");
    dialog->deletePage(s);
    s = tr("Color");
    dialog->deletePage(s);
    
    // candleqs settings  
    s = tr("Color");
    dialog->createPage(s);
    QString s2 = tr("Neutral Color");
    dialog->addColorItem(s2, s, qsNeutralColor);
    s2 = tr("Up Color");
    dialog->addColorItem(s2, s, qsUpColor);
    s2 = tr("Down Color");
    dialog->addColorItem(s2, s, qsDownColor);
    return;
  }
  
  if (! style.compare("Volume Candle"))
  {
    s = tr("Color");
    dialog->deletePage(s);
    
    // volume settings
    s = tr("Volume Candle");
    dialog->createPage (s); 
    QString s2 = tr("Volume MA Periods");
    dialog->addIntItem (s2, s, vma, 2, 999); // ma periods
    s2 = tr("Volume Slow factor");
    dialog->addFloatItem(s2, s, vr1);
    s2 = tr("Volume Active factor");
    dialog->addFloatItem(s2, s, vr2);
    s2 = tr("Volume Hot factor");
    dialog->addFloatItem(s2, s, vr3); // trigger points
    s2 = tr("Volume Fire factor");
    dialog->addFloatItem(s2, s, vr4);
    s2 = tr("Volume Crazy factor");
    dialog->addFloatItem(s2, s, vr5);
    s2 = tr("Fixed Candle Radius (pixels)");
    dialog->addIntItem(s2, s, fixedCandleRadius, 2, 999);
    s2 = tr("Minimum Candle Radius (pixels)");
    dialog->addIntItem(s2, s, minCandleRadius, 1, 999);
    s2 = tr("Max Gap between Candles (pixels)");
    dialog->addIntItem(s2, s, maxCandleGap, 0, 999);
  
    // tab to set candle colors
    s = tr("Volume Colors");
    dialog->createPage(s);
    s2 = tr("Volume Normal color");
    dialog->addColorItem(s2, s, c0);
    s2 = tr("Volume Slow color");
    dialog->addColorItem(s2, s, c1);
    s2 = tr("Volume Active color");
    dialog->addColorItem(s2, s, c2); // candle colors
    s2 = tr("Volume Hot color");
    dialog->addColorItem(s2, s, c3);
    s2 = tr("Volume Fire color");
    dialog->addColorItem(s2, s, c4);
    s2 = tr("Volume Crazy color");
    dialog->addColorItem(s2, s, c5);
  }
}

void Candle::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");
  
  style = settings.readEntry("/style", "Candle");

  QString s = settings.readEntry("/expandCandles", "0");
  expandCandles = s.toInt();
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 2);
  
  // candle settings
  candleColor.setNamedColor(settings.readEntry("/candleColor", "green"));
  
  // qs settings
  qsNeutralColor.setNamedColor(settings.readEntry("/qsNeutralColor", "blue"));
  qsUpColor.setNamedColor(settings.readEntry("/qsUpColor", "green"));
  qsDownColor.setNamedColor(settings.readEntry("/qsDownColor", "red"));

  // volume settings  
  vma = settings.readNumEntry("/vma", 20);
  vr1 = settings.readDoubleEntry("/vr1", 0.5);
  vr2 = settings.readDoubleEntry("/vr2", 1.5);
  vr3 = settings.readDoubleEntry("/vr3", 3.0);
  vr4 = settings.readDoubleEntry("/vr4", 5.0);
  vr5 = settings.readDoubleEntry("/vr5", 10.0);
  
  c0.setNamedColor(settings.readEntry("/c0", "black"));
  c1.setNamedColor(settings.readEntry("/c1", "#5A5A5A"));
  c2.setNamedColor(settings.readEntry("/c2", "darkRed"));
  c3.setNamedColor(settings.readEntry("/c3", "red"));
  c4.setNamedColor(settings.readEntry("/c4", "yellow"));
  c5.setNamedColor(settings.readEntry("/c5", "white"));
  
  fixedCandleRadius = settings.readNumEntry("/fixedCandleRadius", 2);
  minCandleRadius = settings.readNumEntry("/minCandleRadius", 1);
  maxCandleGap = settings.readNumEntry("/maxCandleGap", 1);

  currentPixelspace = settings.readNumEntry("/pixelspace", 2);
    
  settings.endGroup();
}

void Candle::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");

  settings.writeEntry("/style", style);
  settings.writeEntry("/expandCandles", QString::number(expandCandles));
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

void Candle::savePixelspace ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Candle plugin");
  settings.writeEntry("/pixelspace", currentPixelspace);
  settings.endGroup();
}

//*************************************************
//*************************************************

ChartPlugin * createChartPlugin ()
{
  Candle *o = new Candle;
  return ((ChartPlugin *) o);
}

