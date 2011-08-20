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

#include "CommandPlotOHLC.h"
#include "CurveOHLC.h"
#include "InputType.h"
#include "SettingInteger.h"
#include "SettingString.h"
#include "SettingColor.h"

#include <QtDebug>

CommandPlotOHLC::CommandPlotOHLC (QObject *p) : Command (p)
{
  _type = "PLOT_OHLC";
}

int CommandPlotOHLC::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString name = sg->get("NAME")->getString();
  Curve *line = script->curve(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate name" << name;
    return _ERROR;
  }

  QString key = sg->get("DATE")->getString();
  QString s = script->setting(key)->getString();
  Curve *idate = script->curve(s);
  if (! idate)
  {
    qDebug() << _type << "::runScript: invalid DATE" << s;
    return _ERROR;
  }

  key = sg->get("OPEN")->getString();
  s = script->setting(key)->getString();
  Curve *iopen = script->curve(s);
  if (! iopen)
  {
    qDebug() << _type << "::runScript: invalid OPEN" << s;
    return _ERROR;
  }

  key = sg->get("HIGH")->getString();
  s = script->setting(key)->getString();
  Curve *ihigh = script->curve(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  key = sg->get("LOW")->getString();
  s = script->setting(key)->getString();
  Curve *ilow = script->curve(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  key = sg->get("CLOSE")->getString();
  s = script->setting(key)->getString();
  Curve *iclose = script->curve(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << idate << iopen << ihigh << ilow << iclose;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  // chart
  key = sg->get("CHART")->getString();
  QString chart = script->setting(key)->getString();

  // color
  QColor color = sg->get("COLOR")->getColor();

  // Z
  int z = sg->get("Z")->getInteger();

  // PEN
  int pen = sg->get("PEN")->getInteger();

  line = new CurveOHLC;
  line->setZ(z);
  line->setPenWidth(pen);
  line->setLabel(name);
  line->setPlotName(chart);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *dbar = idate->bar(keys.at(loop));
    if (! dbar)
      continue;

    CurveBar *obar = iopen->bar(keys.at(loop));
    if (! obar)
      continue;

    CurveBar *hbar = ihigh->bar(keys.at(loop));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(loop));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(loop));
    if (! cbar)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, obar->data());
    bar->setData(1, hbar->data());
    bar->setData(2, lbar->data());
    bar->setData(3, cbar->data());
    bar->setColor(color);
    bar->setDateTime(dbar->dateTime());

    line->setBar(keys.at(loop), bar);
  }

  script->setCurve(name, line);

  return _OK;
}

SettingGroup * CommandPlotOHLC::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_CHART, Setting::_NONE, QString());
  ss->setKey("CHART");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("ohlc"));
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("DATE");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("OPEN");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("HIGH");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("LOW");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("CLOSE");
  sg->set(ss);

  SettingColor *sc = new SettingColor(QColor(Qt::red));
  sc->setKey("COLOR");
  sg->set(sc);

  SettingInteger *si = new SettingInteger(0, 0, 0, 99, 0);
  si->setKey("Z");
  sg->set(si);

  si = new SettingInteger(0, 0, 1, 99, 1);
  si->setKey("PEN");
  sg->set(si);

  return sg;
}
