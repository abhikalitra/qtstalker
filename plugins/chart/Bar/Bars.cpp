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
#include "Config.h"
#include <qpainter.h>
#include <qsettings.h>

Bars::Bars ()
{
  pluginName = "Bar";
  startX = 2;
  indicatorFlag = FALSE;
  dialog = 0;

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
  Config *config = new Config;
  
  Setting *set = config->getIndicator(indicator);
  
  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, set->getData("plugin"));
  if (! plug)
  {
    delete config;
    delete set;
    return;
  }
  
  plug->setIndicatorInput(data);
  plug->loadIndicatorSettings(config->getData(Config::IndicatorPath) + "/" + indicator);
  plug->calculate();
  plug->getAlerts();
  paintBars = plug->getAlerts();
  delete set;
  delete config;

  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  
  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    switch (paintBars.at(loop))
    {
      case 1:
        painter.setPen(paintUpColor);
	break;
      case -1:
        painter.setPen(paintDownColor);
	break;
      default:
        break;
    }
    
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
  }

  painter.end();
}

void Bars::prefDialog ()
{
  QStringList l;
  l.append(tr("Bar"));
  l.append(tr("Paint Bar"));
  
  dialog = new PrefDialog();
  dialog->setCaption(tr("Bar Chart Prefs"));
  dialog->createPage (tr("Prefs"));
  
  dialog->addComboItem(tr("Style"), tr("Prefs"), l, style);
  QObject::connect(dialog->getComboWidget("Style"),
                   SIGNAL(activated(const QString &)),
                   this,
		   SLOT(styleChanged(const QString &)));
  
  dialog->addIntItem(tr("Min Bar Spacing"), tr("Prefs"), minPixelspace, 4, 99);
  
  styleChanged(style);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    style = dialog->getCombo(tr("Style"));
    
    if (! style.compare(tr("Bar")))
    {
      barNeutralColor = dialog->getColor(tr("Bar Neutral Color"));
      barUpColor = dialog->getColor(tr("Bar Up Color"));
      barDownColor = dialog->getColor(tr("Bar Down Color"));
    }
    else
    {
      paintUpColor = dialog->getColor(tr("Paint Bar Up Color"));
      paintDownColor = dialog->getColor(tr("Paint Bar Down Color"));
      indicator = dialog->getCombo(tr("Indicator"));
    }
    
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
  dialog = 0;
}

void Bars::styleChanged (const QString &)
{
  if (! dialog)
    return;

  style = dialog->getCombo(tr("Style"));
    
  if (! style.compare(tr("Bar")))
  {
    dialog->deletePage(tr("Parms"));
    
    dialog->createPage (tr("Parms"));
    dialog->addColorItem(tr("Bar Neutral Color"), tr("Parms"), barNeutralColor);
    dialog->addColorItem(tr("Bar Up Color"), tr("Parms"), barUpColor);
    dialog->addColorItem(tr("Bar Down Color"), tr("Parms"), barDownColor);
  }
  else
  {
    dialog->deletePage(tr("Parms"));
    
    dialog->createPage (tr("Parms"));
    dialog->addColorItem(tr("Paint Bar Up Color"), tr("Parms"), paintUpColor);
    dialog->addColorItem(tr("Paint Bar Down Color"), tr("Parms"), paintDownColor);
    
    Config *config = new Config;
    dialog->addComboItem(tr("Indicator"), tr("Parms"), config->getIndicators(), indicator);
    delete config;
  }
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
  indicator = settings.readEntry("/indicator");
  
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
  settings.writeEntry("/indicator", indicator);
  
  settings.endGroup();
}

//*************************************************
//*************************************************
  
Plugin * create ()
{
  Bars *o = new Bars;
  return ((Plugin *) o);
}
