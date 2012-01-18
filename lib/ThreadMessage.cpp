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
#include "ThreadMessageType.h"
#include "GlobalParent.h"
#include "DataDialog.h"
#include "ChartNew.h"
#include "ChartUpdate.h"
#include "GlobalSidePanel.h"

#include <QtDebug>
#include <QUuid>

ThreadMessage::ThreadMessage ()
{
}

void ThreadMessage::sendMessage (Entity &e, Script *script)
{
  QString id = QUuid::createUuid().toString();
  
  // put data into global area
  g_dataMutex.lock();
  g_dataList.insert(id, e);
  g_dataMutex.unlock();

  // emit the message signal
  script->threadMessage(id);
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

  if (td.toInteger() == ThreadMessageType::_DIALOG)
  {
    DataDialog *dialog = new DataDialog(g_parent, id, e);
    dialog->show();
  }
  else
    runMessage(e);    
}

void ThreadMessage::runMessage (Entity &e)
{
  Data td;
  if (e.toData(QString("MESSAGE"), td))
    return;
  
  switch ((ThreadMessageType::Type) td.toInteger())
  {
    case ThreadMessageType::_CHART_CURVE:
    {
      ChartUpdate cu;
      cu.curve(e);
      break;
    }
    case ThreadMessageType::_CHART_DATE:
    {
      ChartUpdate cu;
      cu.date(e);
      break;
    }
    case ThreadMessageType::_CHART_LOAD_OBJECT:
    {
      ChartUpdate cu;
      cu.loadObjects(e);
      break;
    }
    case ThreadMessageType::_CHART_NEW:
    {
      ChartNew cn;
      cn.run(e);
      break;
    }
    case ThreadMessageType::_CHART_OBJECT:
    {
      ChartUpdate cu;
      cu.object(e);
      break;
    }
    case ThreadMessageType::_CHART_PANEL_REFRESH:
      g_sidePanel->chartPanel()->updateList();
      break;
    case ThreadMessageType::_CHART_UPDATE:
    {
      ChartUpdate cu;
      cu.update(e);
      break;
    }
    case ThreadMessageType::_GROUP_PANEL_REFRESH:
      g_sidePanel->groupPanel()->updateList();
      break;
    default:
      break;
  }
}
