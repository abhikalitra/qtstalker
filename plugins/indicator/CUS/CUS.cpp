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
#include <qmessagebox.h>

CUS::CUS ()
{
  pluginName = "CUS";
  plotFlag = FALSE;
  customFlag = TRUE;
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
    QStringList l = QStringList::split(",", formulaList[loop], FALSE);

    Config config;
    Plugin *plug = config.getPlugin(Config::IndicatorPluginPath, l[0]);
    if (! plug)
    {
      config.closePlugin(l[0]);
      continue;
    }
  
    plug->setIndicatorInput(data);
    plug->setCustomSettings(functionList[loop]);
    PlotLine *line = plug->calculateCustom(customLines);
    if (! line)
    {
      qDebug("CUS::calculate: no PlotLine returned");
      config.closePlugin(l[0]);
      continue;
    }

    PlotLine *nline = new PlotLine;
    nline->copy(line);
    customLines->replace(QString::number(loop + 1), nline);
    
    config.closePlugin(l[0]);
  }

  for (loop = 0; loop < (int) formulaList.count(); loop++)
  {
    if (plotList[loop].toInt())
    {
      PlotLine *pl = customLines->find(QString::number(loop + 1));
      if (pl)
        output.append(pl);
    }
  }

  delete customLines;
}

int CUS::indicatorPrefDialog (QWidget *)
{
  CUSDialog *dialog = new CUSDialog();
  
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    dialog->setLine(formulaList[loop]);
    
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    int max = dialog->getLines();
    int loop;
    bool flag = FALSE;
    functionList.clear();
    plotList.clear();
    formulaList.clear();
    for (loop = 0; loop < max; loop++)
    {
      plotList.append(dialog->getPlot(loop));
      formulaList.append(dialog->getLine(loop));
      functionList.append(dialog->getFunction(loop));
      
      if (plotList[loop].toInt())
        flag = TRUE;
    }
    
    if (! flag)
    {
      QMessageBox::information(0,
                               tr("Qtstalker: Error"),
			       tr("No step checked to plot."));
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
  plotList.clear();
  functionList.clear();
  
  QDict<QString> d = loadFile(file);
  if (! d.count())
    return;

  int loop = 1;
  while (loop)
  {
    QString *s = d[QString::number(loop)];
    if (s)
    {
      formulaList.append(s->left(s->length()));
      QStringList l = QStringList::split("|", formulaList[loop - 1], FALSE);
      functionList.append(l[0]);
      plotList.append(l[1]);
      loop++;
    }
    else
      break;
  }
}

void CUS::saveIndicatorSettings (QString file)
{
  QDict<QString>d;
  d.setAutoDelete(TRUE);
  
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    d.replace(QString::number(loop + 1), new QString(formulaList[loop]));

  d.replace("plugin", new QString(pluginName));

  saveFile(file, d);
}

void CUS::setCustomFunction (QString d)
{
  formulaList.clear();
  plotList.clear();
  functionList.clear();

  QStringList l = QStringList::split("|", d, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    formulaList.append(l[loop] + "|" + l[loop + 1]);
    functionList.append(l[loop]);
    plotList.append(l[loop + 1]);
  }
}

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

Plugin * create ()
{
  CUS *o = new CUS;
  return ((Plugin *) o);
}

