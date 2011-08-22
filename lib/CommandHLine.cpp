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

#include "CommandHLine.h"
#include "SettingColor.h"
#include "SettingDouble.h"
#include "SettingInteger.h"
#include "SettingString.h"
#include "ChartObjectHLine.h"
#include "Script.h"

#include <QtDebug>

CommandHLine::CommandHLine (QObject *p) : Command (p)
{
  _type = "CHART_OBJECT_HLINE";
}

int CommandHLine::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QColor color = sg->get("COLOR")->getColor();

  double price = sg->get("PRICE")->getDouble();

  QString key = sg->get("CHART")->getString();
  QString chart = script->setting(key)->getString();

  int z = sg->get("Z")->getInteger();

  QString id = script->nextROID();

  ChartObjectHLine *co = new ChartObjectHLine;
  co->setID(id);
  co->setPlotName(chart);
  co->setReadOnly(TRUE);
  co->setZ(z);

  SettingGroup *coset = co->settings();

  Setting *set = coset->get("PRICE");
  set->setDouble(price);

  set = coset->get("COLOR");
  set->setColor(color);

  script->setChartObject(id, co);

  return _OK;
}

SettingGroup * CommandHLine::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_CHART, Setting::_NONE, QString());
  ss->setKey("CHART");
  sg->set(ss);

  SettingDouble *sd = new SettingDouble(0);
  sd->setKey("PRICE");
  sg->set(sd);

  SettingColor *sc = new SettingColor(QColor(Qt::red));
  sc->setKey("COLOR");
  sg->set(sc);

  SettingInteger *si = new SettingInteger(0, 0, 1, 99, 0);
  si->setKey("Z");
  sg->set(si);

  return sg;
}
