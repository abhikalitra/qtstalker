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

#include "Line.h"
#include "LineDialog.h"
#include "QSMath.h"
#include <qpainter.h>
#include <qsettings.h>

Line::Line ()
{
  pluginName = "Line";
  startX = 0;
  indicatorFlag = FALSE;
  loadSettings();  
}

Line::~Line ()
{
}

void Line::drawChart (int startX, int startIndex, int pixelspace)
{
  PlotLine *line = 0;
  QSMath *t = new QSMath(data);
  QString err = t->calculateCustomFormula(formulaList, plotList);
  if (err.length())
  {
    qDebug("Line plugin: " + err);
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

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  loop = startIndex;
  int lineLoop = line->getSize() - data->count() + loop;

  painter.setPen(color);

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    if (lineLoop > -1 && lineLoop < line->getSize())
      y2 = scaler->convertToY(line->getData(lineLoop));
  
    if (y != -1)
      painter.drawLine (x, y, x2, y2);
    x = x2;
    y = y2;

    x2 = x2 + pixelspace;
    loop++;
    lineLoop++;
  }

  painter.end();
}

void Line::prefDialog ()
{
  LineDialog *dialog = new LineDialog();
  dialog->setColor(color);
  dialog->setSpacing(minPixelspace);
  
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    dialog->setLine(formulaList[loop] + "|" + plotList[loop]);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    minPixelspace = dialog->getSpacing();
    color = dialog->getColor();
    
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

void Line::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Line plugin");

  color.setNamedColor(settings.readEntry("/Color", "green"));
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 3);
  
  QString s = settings.readEntry("/formula", "REF,Close,0,green,Line,C|1");
  QStringList l = QStringList::split("|", s, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    formulaList.append(l[loop]);
    plotList.append(l[loop + 1]);
  }
  
  settings.endGroup();
}

void Line::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Line plugin");
  
  settings.writeEntry("/Color", color.name());
  settings.writeEntry("/minPixelspace", minPixelspace);
  
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
  Line *o = new Line;
  return ((Plugin *) o);
}

