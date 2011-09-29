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

#include "CommandMessage.h"
#include "Globals.h"
#include "CommandFactory.h"
#include "Chart.h"
#include "ChartUpdate.h"

#include <QtDebug>

CommandMessage::CommandMessage ()
{
}

int CommandMessage::message (Data *d)
{
  CommandFactory fac;

  switch ((CommandFactory::Type) fac.stringToType(d->command()))
  {
    case CommandFactory::_CHART:
    {
      Chart chart;
      chart.run(d);
      delete d;
      break;
    }
    case CommandFactory::_CHART_PANEL_REFRESH:
      g_chartPanel->refresh();
      delete d;
      break;
    case CommandFactory::_CHART_UPDATE:
    {
      ChartUpdate cu;
      cu.run(d);
      break;
    }
    case CommandFactory::_GROUP_PANEL_REFRESH:
      g_groupPanel->updateGroups();
      delete d;
      break;
    default:
      delete d;
      return 1;
      break;
  }

  return 0;
}
