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

#include "CommandPlotLine.h"
#include "Script.h"
#include "CurveLine.h"
#include "InputType.h"
#include "SettingInteger.h"
#include "SettingString.h"
#include "SettingColor.h"

#include <QtDebug>

CommandPlotLine::CommandPlotLine (QObject *p) : Command (p)
{
  _type = "PLOT_LINE";
}

int CommandPlotLine::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  // date
  QString key = sg->get("DATE")->getString();
  QString s = script->setting(key)->getString();
  Curve *idate = script->curve(s);
  if (! idate)
  {
    qDebug() << _type << "::runScript: invalid DATE" << s;
    return _ERROR;
  }

  // input
  key = sg->get("INPUT")->getString();
  s = script->setting(key)->getString();
  Curve *in = script->curve(s);
  if (! in)
  {
    qDebug() << _type << "::run: INPUT not found" << s;
    return _ERROR;
  }

  // chart
  key = sg->get("CHART")->getString();
  QString chart = script->setting(key)->getString();

  // label
  QString label = sg->get("NAME")->getString();
  if (label.isEmpty())
  {
    qDebug() << _type << "::run: invalid NAME" << label;
    return _ERROR;
  }

  Curve *tline = script->curve(label);
  if (tline)
  {
    qDebug() << _type << "::run: duplicate LABEL" << label;
    return _ERROR;
  }

  QList<Curve *> list;
  list << idate << in;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  // color
  QColor color = sg->get("COLOR")->getColor();

  Curve *line = new CurveLine(Qt::SolidLine);

  // Z
  line->setZ(sg->get("Z")->getInteger());

  // PEN
  line->setPenWidth(sg->get("PEN")->getInteger());

  line->setLabel(label);
  line->setPlotName(chart);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *dbar = idate->bar(keys.at(loop));
    if (! dbar)
      continue;

    CurveBar *ibar = in->bar(keys.at(loop));
    if (! ibar)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(ibar->data());
    bar->setColor(color);
    bar->setDateTime(dbar->dateTime());

    line->setBar(keys.at(loop), bar);
  }

  script->setCurve(label, line);

  return _OK;
}

SettingGroup * CommandPlotLine::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_CHART, Setting::_NONE, QString());
  ss->setKey("CHART");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("DATE");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString());
  ss->setKey("NAME");
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
