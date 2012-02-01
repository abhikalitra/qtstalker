/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "ThreadMessage.h"
#include "GlobalData.h"
#include "TypeThreadMessage.h"
#include "GlobalParent.h"
#include "DataDialog.h"
#include "ChartNew.h"
#include "ChartUpdate.h"
#include "GlobalSidePanel.h"
#include "DialogMessage.h"
#include "DialogNew.h"
#include "ScriptRunDialog.h"
#include "DialogSelect.h"
#include "GlobalPlotGroup.h"
#include "DialogConfirm.h"
#include "ThreadMessageFunctions.h"

#include <QtDebug>

ThreadMessage::ThreadMessage ()
{
}

void ThreadMessage::getMessage (QString id)
{
  // get message from global area
  g_dataMutex.lock();
  Entity e = g_dataList.value(id);
  g_dataList.remove(id);
  g_dataMutex.unlock();

  Data td;
  if (e.toData(QString("MESSAGE"), td))
    return;

  switch ((TypeThreadMessage::Key) td.toInteger())
  {
    case TypeThreadMessage::_DIALOG:
    {
      DataDialog *dialog = new DataDialog(g_parent, id, e);
      dialog->show();
      break;
    }
    case TypeThreadMessage::_DIALOG_CONFIRM:
    {
      DialogConfirm *dialog = new DialogConfirm(g_parent, id, e);
      dialog->show();
      break;
    }
    case TypeThreadMessage::_DIALOG_NEW:
    {
      DialogNew *dialog = new DialogNew(g_parent, id, e);
      dialog->show();
      break;
    }
    case TypeThreadMessage::_DIALOG_SCRIPT:
    {
      ScriptRunDialog *dialog = new ScriptRunDialog(g_parent, QString(), QString(), id, e);
      dialog->show();
      break;
    }
    case TypeThreadMessage::_DIALOG_SELECT:
    {
      DialogSelect *dialog = new DialogSelect(g_parent, id, e);
      dialog->show();
      break;
    }
    default:
      runMessage(e, id);
      break;
  }
}

void ThreadMessage::runMessage (Entity &e, QString id)
{
  Data td;
  if (e.toData(QString("MESSAGE"), td))
    return;
  
  switch ((TypeThreadMessage::Key) td.toInteger())
  {
    case TypeThreadMessage::_CHART_CURVE:
    {
      ChartUpdate cu;
      cu.curve(e);
      break;
    }
    case TypeThreadMessage::_CHART_DATE:
    {
      ChartUpdate cu;
      cu.date(e);
      break;
    }
    case TypeThreadMessage::_CHART_LOAD_OBJECT:
    {
      ChartUpdate cu;
      cu.loadObjects(e);
      break;
    }
    case TypeThreadMessage::_CHART_NEW:
    {
      ChartNew cn;
      cn.run(e);
      break;
    }
    case TypeThreadMessage::_CHART_OBJECT:
    {
      ChartUpdate cu;
      cu.object(e);
      break;
    }
    case TypeThreadMessage::_CHART_OBJECT_REMOVE:
    {
      Data list;
      e.toData(QString("LIST"), list);
      QStringList l = list.toList();
      int loop = 0;
      for (; loop < l.size(); loop++)
      {
        Plot *p = g_plotGroup->plot(l.at(loop));
        if (! p)
          continue;
        p->deleteAllChartObjects();
      }
      break;
    }
    case TypeThreadMessage::_CHART_PANEL_REFRESH:
      g_sidePanel->chartPanel()->updateList();
      break;
    case TypeThreadMessage::_CHART_PANEL_SELECT:
    {
      ThreadMessageFunctions tmf;
      QStringList l;
      g_sidePanel->chartPanel()->selected(l);
      if (! l.size())
      {
	Entity te;
        tmf.sendRelease(id, te);
      }
      else
      {
        Data list;
        e.toData(QString("LIST"), list);
        list.set(l);
        e.set(QString("LIST"), list);
        tmf.sendRelease(id, e);
      }
      break;
    }
    case TypeThreadMessage::_CHART_REMOVE:
    {
      Data list;
      e.toData(QString("LIST"), list);
      QStringList l = list.toList();
      int loop = 0;
      for (; loop < l.size(); loop++)
        g_plotGroup->removePlot(l.at(loop));
      break;
    }
    case TypeThreadMessage::_CHART_UPDATE:
    {
      ChartUpdate cu;
      cu.update(e);
      break;
    }
    case TypeThreadMessage::_DIALOG_MESSAGE:
    {
      Data td;
      e.toData(QString("TEXT"), td);
      
      DialogMessage *md = new DialogMessage(g_parent);
      md->setMessage(td.toString());
      md->show();
      break;
    }
    case TypeThreadMessage::_GROUP_PANEL_REFRESH:
      g_sidePanel->groupPanel()->updateGroups();
      break;
    case TypeThreadMessage::_GROUP_PANEL_SELECT:
    {
      ThreadMessageFunctions tmf;
      QStringList l;
      g_sidePanel->groupPanel()->selected(l);
      if (! l.size())
      {
	Entity te;
        tmf.sendRelease(id, te);
      }
      else
      {
        Data list;
        e.toData(QString("LIST"), list);
        list.set(l);
        e.set(QString("LIST"), list);
        tmf.sendRelease(id, e);
      }
      break;
    }
    case TypeThreadMessage::_SCRIPT:
    {
      Data file, command;
      e.toData(QString("FILE"), file);
      e.toData(QString("COMMAND"), command);
    
      g_sidePanel->scriptPanel()->runScript(command.toString(), file.toString());
      break;
    }
    default:
      break;
  }
}
