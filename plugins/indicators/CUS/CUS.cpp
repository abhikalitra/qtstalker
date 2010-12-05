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
#include "../../../lib/qtstalker_defines.h"
#include "IndicatorSettings.h"

#include <QtDebug>
#include <QList>

CUS::CUS ()
{
  _indicator = "CUS";
}

int CUS::getIndicator (Indicator &ind, BarData &data)
{
  IndicatorSettings settings = ind.settings();

  IndicatorSetting set = settings.setting(_Command);
  QString s = set.value();
  
  set = settings.setting(_Script);
  QString s2 = set.value();
  
  s.append(" " + s2);

  ExScript script;
  script.setIndicator(ind);
  script.setBarData(data);
  int rc = script.calculate(s);
  if (! rc)
    ind = script.indicator();

  return rc;
}

IndicatorPluginDialog * CUS::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);
  dialog->setHelpFile("cus.html");
  return dialog;
}

void CUS::defaults (Indicator &i)
{
  IndicatorSettings set;

  QStringList l;
  l << tr("General");
  set.setTabs(l);

  int page = 0;
  set.setText(_Command, page, tr("Command"), QString("perl"));

  QString inputDir = QString("%1/qtstalker/indicator/").arg(INSTALL_DATA_DIR);
  set.setFile(_Script, page, tr("Script"), inputDir);

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


