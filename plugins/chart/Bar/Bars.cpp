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
#include "QSMath.h"
#include <qpainter.h>
#include <qsettings.h>
#include <qmessagebox.h>

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
  PlotLine *line = 0;
  QSMath *t = new QSMath(data);
  QString err = t->calculateCustomFormula(formulaList, plotList);
  if (err.length())
  {
    QMessageBox::warning(0,
    			 QObject::tr("Qtstalker: Custom Function Error"),
			 err);
    delete t;
    return;
  }
  
  int loop;
  for (loop = plotList.count() - 1; loop > -1; loop--)
  {
    if (plotList[loop].toInt())
    {
      line = t->getCustomLine(loop + 1);
      break;
    }
  }
  delete t;
  
  if (! line)
    return;

  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  loop = startIndex;
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
    dialog->setLine(formulaList[loop] + "|" + plotList[loop]);
  
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
    plotList.clear();
    for (loop = 0; loop < (int) dialog->getLines(); loop++)
    {
      formulaList.append(dialog->getFunction(loop));
      plotList.append(dialog->getPlot(loop));
    }
        
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
  {
    formulaList.append(l[loop]);
    plotList.append(l[loop + 1]);
  }
    
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
  
  int loop;
  QString s;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
  {
    s.append("|");
    s.append(formulaList[loop]);
    s.append("|");
    s.append(plotList[loop]);
  }
  s.remove(0, 1);
  settings.writeEntry("/formula", s);
  
  settings.endGroup();
}

//*************************************************
//*************************************************
  
Plugin * create ()
{
  Bars *o = new Bars;
  return ((Plugin *) o);
}
