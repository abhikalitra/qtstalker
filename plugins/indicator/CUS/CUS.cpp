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
#include <qdict.h>

CUS::CUS ()
{
  pluginName = "CUS";
  plotFlag = FALSE;
}

CUS::~CUS ()
{
}

void CUS::calculate ()
{
  QSMath *t = new QSMath(data);
  t->calculateCustomFormula(functionList, plotList);
  
  int loop;
  for (loop = 0; loop < (int) functionList.count(); loop++)
  {
    if (plotList[loop].toInt())
      output.append(t->getCustomLine(loop + 1));
  }
  
  delete t;
}

int CUS::indicatorPrefDialog ()
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
    functionList.clear();
    plotList.clear();
    formulaList.clear();
    for (loop = 0; loop < max; loop++)
    {
      functionList.append(dialog->getFunction(loop));
      plotList.append(dialog->getPlot(loop));
      formulaList.append(dialog->getLine(loop));
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
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
    
  int loop = 1;
  while (loop)
  {
    QString *s = dict[QString::number(loop)];
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
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);
  
  int loop;
  for (loop = 0; loop < (int) functionList.count(); loop++)
    dict.replace(QString::number(loop + 1), new QString(formulaList[loop]));

  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  CUS *o = new CUS;
  return ((Plugin *) o);
}



