/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "CUS.h"
#include "CUSDialog.h"
#include "Config.h"
#include "IndicatorPlugin.h"
#include <qmessagebox.h>
#include <qobject.h>

CUS::CUS ()
{
  pluginName = "CUS";
  customFlag = TRUE;
  helpFile = "cus.html";
}

CUS::~CUS ()
{
}

void CUS::calculate ()
{
  int loop;
  customLines = new QDict<PlotLine>;
  customLines->setAutoDelete(FALSE);
  
  for (loop = 0; loop < (int) formulaList.count(); loop++)
  {
    Setting set;
    set.parse(formulaList[loop]);

    Config config;
    IndicatorPlugin *plug = config.getIndicatorPlugin(set.getData("plugin"));
    if (! plug)
    {
      qDebug("CUS::calculate: %s plugin not loaded", set.getData("plugin").latin1());
      config.closePlugin(set.getData("plugin"));
      continue;
    }
  
    plug->setCustomFlag(TRUE);
    plug->setIndicatorInput(data);
    plug->setIndicatorSettings(set);
    PlotLine *line = plug->calculateCustom(customLines);
    if (! line)
    {
      qDebug("CUS::calculate: no PlotLine returned");
      config.closePlugin(set.getData("plugin"));
      continue;
    }

    PlotLine *nline = new PlotLine;
    nline->copy(line);
    customLines->replace(QString::number(loop + 1), nline);
    
    config.closePlugin(set.getData("plugin"));
  }

  for (loop = 0; loop < (int) formulaList.count(); loop++)
  {
    Setting set;
    set.parse(formulaList[loop]);
    if (set.getData("plot").toInt())
    {
      PlotLine *pl = customLines->find(QString::number(loop + 1));
      if (pl)
        output->addLine(pl);
    }
  }

  delete customLines;
}

int CUS::indicatorPrefDialog (QWidget *)
{
  CUSDialog *dialog = new CUSDialog(helpFile);
  
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    dialog->setLine(formulaList[loop]);
    
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    int max = dialog->getLines();
    int loop;
    bool flag = FALSE;
    formulaList.clear();
    for (loop = 0; loop < max; loop++)
    {
      formulaList.append(dialog->getLine(loop));
      
      Setting set;
      set.parse(dialog->getLine(loop));
      if (set.getData("plot").toInt())
        flag = TRUE;
    }
    
    if (! flag)
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
			       QObject::tr("No step checked to plot."));
    }
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void CUS::loadIndicatorSettings (QString file)
{
  formulaList.clear();
  
  Setting set = loadFile(file);
  if (! set.count())
    return;

  int loop = 1;
  while (loop)
  {
    QString s = set.getData(QString::number(loop));
    if (s.length())
    {
      formulaList.append(s);
      loop++;
    }
    else
      break;
  }
  
  plotType = set.getData("plotType").toInt();
}

void CUS::saveIndicatorSettings (QString file)
{
  Setting d;
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    d.setData(QString::number(loop + 1), formulaList[loop]);

  d.setData("plugin", pluginName);
  d.setData("plotType", QString::number(plotType));

  saveFile(file, d);
}

void CUS::setCustomFunction (QString d)
{
  formulaList.append(d);
}

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CUS *o = new CUS;
  return ((IndicatorPlugin *) o);
}
