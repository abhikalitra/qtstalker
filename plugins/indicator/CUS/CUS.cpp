/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include <qfile.h>

CUS::CUS ()
{
  pluginName = "CUS";
  helpFile = "cus.html";
}

CUS::~CUS ()
{
}

void CUS::calculate ()
{
  clearOutput();

  Config config;
  QStringList varList;
  QDict<PlotLine> lines;
  lines.setAutoDelete(TRUE);
  
  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
  {
    // check if whole line is a comment
    QString s = formulaList[loop].left(2);
    if ( ! s.compare("//"))
      continue;

    if (formulaList[loop].contains(":="))
    {
      QStringList l = QStringList::split(":=", formulaList[loop], FALSE);
      if (l.count() != 2)
      {
        qDebug("CUS::calculate: line %i parm missing", loop);
        return;
      }
      
      QString var = l[0].stripWhiteSpace();
      if (varList.findIndex(var) != -1)
      {
        qDebug("CUS::calculate: line %i duplicate variable: %s", loop, var.latin1());
        return;
      }
      varList.append(var);

      QStringList l2 = QStringList::split("(", l[1], FALSE);
      if (l2.count() != 2)
      {
        qDebug("CUS::calculate: line %i bad indicator format", loop);
        return;
      }
      
      QString plugin = l2[0].stripWhiteSpace();
      QString parms = l2[1];
      parms.truncate(parms.find(")", 0, TRUE));
      parms = parms.stripWhiteSpace();
      while(parms.contains(" "))
        parms = parms.remove(parms.find(" ", 0, TRUE), 1);

      IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
      if (! plug)
      {
        qDebug("CUS::calculate: %s plugin not loaded", plugin.latin1());
        config.closePlugin(plugin);
        return;
      }
      plug->setIndicatorInput(data);

      l = QStringList::split(",", parms, FALSE);
      int loop2;
      QPtrList<PlotLine> inList;
      inList.setAutoDelete(FALSE);
      for (loop2 = 0; loop2 < (int) l.count(); loop2++)
      {
        if (varList.findIndex(l[loop2]) != -1)
          inList.append(lines.find(l[loop2]));
        else
        {
          int itype = inputTypeList.findIndex(l[loop2]);
          if (itype != -1)
          {
            PlotLine *in = data->getInput((BarData::InputType) itype);
            if (! in)
            {
              qDebug("CUS::calculate:line%i parm%i: input not found", loop, loop2);
              return;
            }

            lines.replace(l[loop2], in);
            inList.append(in);
          }
        }
      }
   
      if (! inList.count())
      {
        if (plugin.compare("TALIB"))
        {
          qDebug("CUS::calculate: no input array");
          config.closePlugin(plugin);
          return;
        }
      }

      PlotLine *out = plug->calculateCustom(parms, inList);
      if (! out)
      {
        qDebug("CUS::calculate: line %i: no PlotLine returned: %s", loop, parms.latin1());
        config.closePlugin(plugin);
        return;
      }

      PlotLine *pl = new PlotLine;
      lines.replace(var, pl);
      pl->copy(out);
    }

    if (formulaList[loop].contains("plot"))
    {
      QStringList l = QStringList::split("(", formulaList[loop], FALSE);
      if (l.count() != 2)
      {
        qDebug("CUS::calculate: line %i: bad plot format", loop);
        return;
      }

      QString parms = l[1];
      parms.truncate(parms.find(")", -1, TRUE));
      l = QStringList::split(",", parms, FALSE);
      if (l.count() != 4)
      {
        qDebug("CUS::calculate: line %i: missing plot parms", loop);
        return;
      }

      // 1.var name
      l[0] = l[0].stripWhiteSpace();
      PlotLine *pl = lines.find(l[0]);
      if (! pl)
      {
        qDebug("CUS::calculate: line %i: bad plot parm 1", loop);
        return;
      }

      // 2.color
      l[1] = l[1].stripWhiteSpace();
      pl->setColor(l[1]);

      // 3.label
      l[2] = l[2].stripWhiteSpace();
      pl->setLabel(l[2]);

      // 4.linetype
      l[3] = l[3].stripWhiteSpace();
      pl->setType(l[3]);

      PlotLine *tline = new PlotLine;
      tline->copy(pl);
      output->addLine(tline);
    }
  }
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
    dialog->getList(formulaList);

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void CUS::setCustomFunction (QStringList &d)
{
  formulaList.clear();
  formulaList = d;
}

int CUS::getMinBars ()
{
  int loop;
  int min = minBars;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
  {
    Setting set;
    set.parse(formulaList[loop]);

    Config config;
    QString plugin = set.getData("plugin");
    IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
    if (! plug)
    {
      qDebug("CUS::calculate: %s plugin not loaded", plugin.latin1());
      config.closePlugin(plugin);
      continue;
    }
  
    plug->setIndicatorSettings(set);
    
    min = min + plug->getMinBars();
    
    config.closePlugin(plugin);
  }

  return min;
}

void CUS::getIndicatorSettings (Setting &dict)
{
  dict.setData("script", formulaList.join("|"));
  dict.setData("plugin", pluginName);
}

void CUS::setIndicatorSettings (Setting &dict)
{
  formulaList.clear();

  if (! dict.count())
    return;
  
  QString s = dict.getData("script");
  if (s.length())
    formulaList = QStringList::split("|", s, FALSE);
}

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CUS *o = new CUS;
  return ((IndicatorPlugin *) o);
}

