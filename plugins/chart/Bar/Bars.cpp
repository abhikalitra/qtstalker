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
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>

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
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  
  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
//    QColor *color = paintBars.at(loop);
//    painter.setPen(QColor(color->red(), color->green(), color->blue()));

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
  
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Bar Chart Prefs"));
  dialog->createPage (tr("Prefs"));
  dialog->addComboItem(tr("Style"), 1, l, style);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 4, 99);
  
  dialog->createPage (tr("Bar Colors"));
  dialog->addColorItem(tr("Bar Neutral Color"), 2, barNeutralColor);
  dialog->addColorItem(tr("Bar Up Color"), 2, barUpColor);
  dialog->addColorItem(tr("Bar Down Color"), 2, barDownColor);
  
  dialog->createPage (tr("Paint Bar Colors"));
  dialog->addColorItem(tr("Paint Bar Up Color"), 3, paintUpColor);
  dialog->addColorItem(tr("Paint Bar Down Color"), 3, paintDownColor);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    style = dialog->getCombo(tr("Style"));
    
    barNeutralColor = dialog->getColor(tr("Bar Neutral Color"));
    barUpColor = dialog->getColor(tr("Bar Up Color"));
    barDownColor = dialog->getColor(tr("Bar Down Color"));
    
    paintUpColor = dialog->getColor(tr("Paint Bar Up Color"));
    paintDownColor = dialog->getColor(tr("Paint Bar Down Color"));
    
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void Bars::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Bar plugin");

  QString s = settings.readEntry("/barNeutralColor", "blue");
  barNeutralColor.setNamedColor(s);
  
  s = settings.readEntry("/barUpColor", "green");
  barUpColor.setNamedColor(s);

  s = settings.readEntry("/barDownColor", "red");
  barDownColor.setNamedColor(s);
  
  s = settings.readEntry("/paintUpColor", "green");
  paintUpColor.setNamedColor(s);

  s = settings.readEntry("/paintDownColor", "red");
  paintDownColor.setNamedColor(s);
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 4);
  
  style = settings.readEntry("/style", tr("Bar"));
  
  settings.endGroup();
}

void Bars::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Bar plugin");
  
  settings.writeEntry("/barNeutralColor", barNeutralColor.name());
  settings.writeEntry("/barUpColor", barUpColor.name());
  settings.writeEntry("/barDownColor", barDownColor.name());
  settings.writeEntry("/paintUpColor", paintUpColor.name());
  settings.writeEntry("/paintDownColor", paintDownColor.name());
  settings.writeEntry("/minPixelspace", minPixelspace);
  settings.writeEntry("/style", style);
  
  settings.endGroup();
}

//*************************************************
//*************************************************
  
Plugin * create ()
{
  Bars *o = new Bars;
  return ((Plugin *) o);
}
