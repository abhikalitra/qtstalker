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
#include "Config.h"
#include "Indicator.h"
#include "IndicatorPlugin.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qsettings.h>
#include <qmessagebox.h>

Line::Line ()
{
  pluginName = "Line";
  startX = 0;
  indicatorFlag = FALSE;
  defaultFlag = TRUE;
  defaultFormula = "plot=1|lineType=4|period=0|plugin=REF|input=3|color=#ff0000|label=REF|scale=0";
  helpFile = "linechartplugin.html";
  line = 0;
  loadSettings();  
}

Line::~Line ()
{
  if (line)
    delete line;
}

void Line::drawChart (QPixmap &buffer, Scaler &scaler, int startX, int startIndex, int pixelspace)
{
  if (! line)
    return;

  QPainter painter;
  painter.begin(&buffer);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = startIndex;
  int lineLoop = line->getSize() - data->count() + loop;

  painter.setPen(color);

  while ((x < buffer.width()) && (loop < (int) data->count()))
  {
    if (lineLoop > -1 && lineLoop < line->getSize())
      y2 = scaler.convertToY(line->getData(lineLoop));
  
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

void Line::prefDialog (QWidget *)
{
  LineDialog *dialog = new LineDialog(helpFile);
  dialog->setColor(color);
  dialog->setSpacing(minPixelspace);
  dialog->setDefault(defaultFlag);
  
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    dialog->setLine(formulaList[loop]);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    minPixelspace = dialog->getSpacing();
    color = dialog->getColor();
    defaultFlag = dialog->getDefault();

    formulaList.clear();
    if (! defaultFlag)
    {
      int loop;
      bool flag = FALSE;
      for (loop = 0; loop < (int) dialog->getLines(); loop++)
      {
        formulaList.append(dialog->getLine(loop));
	
        Setting set;
	QString s = dialog->getLine(loop); 
        set.parse(s);
        if (set.getData("plot").toInt())
          flag = TRUE;
      }
  
      if (! flag && formulaList.count())
      {
        QMessageBox::information(0,
                                 tr("Qtstalker: Error"),
			         tr("Line chart: no step checked to plot."));
        delete dialog;
        saveFlag = TRUE;
        return;
      }
    }
    else
      formulaList = defaultFormula;
        
    getBoolLine();
    
    saveFlag = TRUE;
    saveSettings();
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
  
  defaultFlag = settings.readBoolEntry("/defaultFlag", TRUE);
  
  QString s = settings.readEntry("/formula");
  QStringList l = QStringList::split(",", s, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    formulaList.append(l[loop]);

  currentPixelspace = settings.readNumEntry("/pixelspace", 3);
      
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
  settings.writeEntry("/defaultFlag", defaultFlag);
  settings.writeEntry("/formula", formulaList.join(","));
  
  settings.endGroup();
}

void Line::getBoolLine ()
{
  if (line)
  {
    delete line;
    line = 0;
  }

  Config config;
  
  // open the CUS plugin
  QString plugin("CUS");
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return;
  }

  // load the CUS plugin and calculate
  int loop;
  for(loop = 0; loop < (int) formulaList.count(); loop++)
    plug->setCustomFunction(formulaList[loop]);
    
  plug->setIndicatorInput(data);
  plug->calculate();
  Indicator *i = plug->getIndicator();
  PlotLine *tline = i->getLine(0);
  if (! tline)
  {
    qDebug("Line::getBoolLine: no PlotLine returned");
    config.closePlugin(plugin);
    return;
  }
    
  line = new PlotLine;
  line->copy(tline);
  
  config.closePlugin(plugin);
}

void Line::savePixelspace ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Line plugin");
  settings.writeEntry("/pixelspace", currentPixelspace);
  settings.endGroup();
}

void Line::setChartInput (BarData *d)
{
  data = d;
  
  if (data)
  {
    if (data->count())
      getBoolLine();
  }
}


//*************************************************
//*************************************************

ChartPlugin * createChartPlugin ()
{
  Line *o = new Line;
  return ((ChartPlugin *) o);
}

