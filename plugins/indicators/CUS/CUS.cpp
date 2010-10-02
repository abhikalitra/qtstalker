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

#include "CUS.h"
#include "ExScript.h"
#include "CUSDialog.h"
#include "../../../lib/qtstalker_defines.h"

#include <QtDebug>
#include <QList>

CUS::CUS ()
{
  _indicator = "CUS";
}

int CUS::getIndicator (Indicator &ind, BarData &)
{
  Setting settings = ind.settings();

  QString s, s2;
  settings.getData(Command, s);
  settings.getData(Script, s2);
  s.append(" " + s2);

  ExScript script;
  script.setIndicator(ind);
  int rc = script.calculate(s);
  if (! rc)
    ind = script.indicator();

  return rc;
}

IndicatorPluginDialog * CUS::dialog (Indicator &i)
{
  return new CUSDialog(i);
}

void CUS::defaults (Indicator &i)
{
  Setting set;
  set.setData(Command, "perl");
  QString inputDir = QString("%1/qtstalker/indicator/").arg(INSTALL_DATA_DIR);
  set.setData(Script, inputDir);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CUS *o = new CUS;
  return ((IndicatorPlugin *) o);
}


