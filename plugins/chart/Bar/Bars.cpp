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

#include "Bars.h"
#include "BarDialog.h"
#include "Config.h"
#include <qpainter.h>
#include <qsettings.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>

Bars::Bars ()
{
  pluginName = "Bar";
  startX = 2;
  indicatorFlag = FALSE;

  loadSettings();  
}

Bars::~Bars ()
{
}

void Bars::drawChart (int startX, int startIndex, int pixelspace)
{
  if (! style.compare(tr("Bar")))
    drawBars(startX, startIndex, pixelspace);
  else
    drawPaintBars(startX, startIndex, pixelspace);
}

void Bars::drawBars (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  
  // set first bar as neutral
  painter.setPen(barNeutralColor);

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
      painter.setPen(barUpColor);
    else
    {
      if (data->getClose(loop) < data->getClose(loop - 1))
        painter.setPen(barDownColor);
      else
        painter.setPen(barNeutralColor);
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

void Bars::drawPaintBars (int startX, int startIndex, int pixelspace)
{
  PlotLine *line = getBoolLine();
  if (! line)
    return;

  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  int lineLoop = line->getSize() - data->count() + loop;
  
  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    if (lineLoop > -1 && lineLoop < line->getSize())
    {
      if (line->getData(lineLoop))
        painter.setPen(paintUpColor);
      else
        painter.setPen(paintDownColor);
    }
    else
      painter.setPen(barNeutralColor);
    
    int y;
    if (data->getOpen(loop) != 0)
    {
      y = scaler->convertToY(data->getOpen(loop));
      painter.drawLine (x - 2, y, x, y);
    }

    y = scaler->convertToY(data->getClose(loop));
    painter.drawLine (x + 2, y, x, y);

    int h = scaler->convertToY(data->getHigh(loop));
    int l = scaler->convertToY(data->getLow(loop));
    painter.drawLine (x, h, x, l);

    x = x + pixelspace;
    loop++;
    lineLoop++;
  }

  painter.end();
  
  delete line;
}

void Bars::prefDialog ()
{
  BarDialog *dialog = new BarDialog();
  dialog->setBarColors(barUpColor, barDownColor, barNeutralColor);
  dialog->setPaintBarColors(paintUpColor, paintDownColor);
  dialog->setStyle(style);
  dialog->setSpacing(minPixelspace);
  
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    dialog->setLine(formulaList[loop]);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    minPixelspace = dialog->getSpacing();
    style = dialog->getStyle();
    barUpColor = dialog->getBarUpColor();
    barDownColor = dialog->getBarDownColor();
    barNeutralColor = dialog->getBarNeutralColor();
    paintUpColor = dialog->getPaintUpColor();
    paintDownColor = dialog->getPaintDownColor();
    
    int loop;
    formulaList.clear();
    for (loop = 0; loop < (int) dialog->getLines(); loop++)
      formulaList.append(dialog->getLine(loop));
        
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void Bars::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Bar plugin");

  minPixelspace = settings.readNumEntry("/minPixelspace", 4);
  style = settings.readEntry("/style", tr("Bar"));

  // bar settings  
  barNeutralColor.setNamedColor(settings.readEntry("/barNeutralColor", "blue"));
  barUpColor.setNamedColor(settings.readEntry("/barUpColor", "green"));
  barDownColor.setNamedColor(settings.readEntry("/barDownColor", "red"));

  // paint bar settings  
  paintUpColor.setNamedColor(settings.readEntry("/paintUpColor", "green"));
  paintDownColor.setNamedColor(settings.readEntry("/paintDownColor", "red"));

  QString s = settings.readEntry("/formula");
  QStringList l = QStringList::split("|", s, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
    formulaList.append(l[loop] + "|" + l[loop + 1]);
    
  settings.endGroup();
}

void Bars::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Bar plugin");
  
  settings.writeEntry("/minPixelspace", minPixelspace);
  settings.writeEntry("/style", style);

  // bar settengs  
  settings.writeEntry("/barNeutralColor", barNeutralColor.name());
  settings.writeEntry("/barUpColor", barUpColor.name());
  settings.writeEntry("/barDownColor", barDownColor.name());
  
  // paint bar settings
  settings.writeEntry("/paintUpColor", paintUpColor.name());
  settings.writeEntry("/paintDownColor", paintDownColor.name());
  
  settings.writeEntry("/formula", formulaList.join("|"));
  
  settings.endGroup();
}

PlotLine * Bars::getBoolLine ()
{
  PlotLine *line = 0;
  Config config;
  
  // open the CUS plugin   
  Plugin *plug = config.getPlugin(Config::IndicatorPluginPath, "CUS");
  if (! plug)
  {
    config.closePlugin("CUS");
    return line;
  }

  // load the CUS plugin and calculate
  plug->setCustomFunction(formulaList.join("|"));
  plug->setIndicatorInput(data);
  plug->calculate();
  line = plug->getIndicatorLine(0);
  if (! line)
  {
    qDebug("Bars::getBoolLine: no PlotLine returned");
    config.closePlugin("CUS");
    return 0;
  }
    
  PlotLine *nline = new PlotLine;
  nline->copy(line);
  
  config.closePlugin("CUS");
  
  return nline;
}

//*************************************************
//*************************************************
  
Plugin * create ()
{
  Bars *o = new Bars;
  return ((Plugin *) o);
}
