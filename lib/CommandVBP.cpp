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

#include "CommandVBP.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingColor.h"

#include <QtDebug>
#include <QHash>

CommandVBP::CommandVBP (QObject *p) : Command (p)
{
  _type = "VBP";
}

int CommandVBP::runScript (void *d)
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

  QString key = sg->get("CLOSE")->getString();
  QString s = script->setting(key)->getString();
  Curve *iclose = script->curve(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  key = sg->get("VOLUME")->getString();
  s = script->setting(key)->getString();
  Curve *ivol = script->curve(s);
  if (! ivol)
  {
    qDebug() << _type << "::runScript: invalid VOLUME" << s;
    return _ERROR;
  }

  QColor upColor = sg->get("COLOR_UP")->getColor();

  QColor downColor = sg->get("COLOR_DOWN")->getColor();

  QList<Curve *> list;
  list << iclose << ivol;
  line = getVBP(list, upColor, downColor);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandVBP::getVBP (QList<Curve *> &list, QColor upColor, QColor downColor)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  // get close high/low
  int loop = 0;
  double high = 0;
  double low = 0;
  Curve *close = list.at(0);
  Curve *vol = list.at(1);
  if (close->highLow(high, low))
    return 0;

  // get the price block range
  double range = (high - low) / 12;
  QHash<int, Setting *> hash;
  double bottom = low;
  double top = bottom + range;
  loop = 0;
  for (; loop < 12; loop++)
  {
    Setting *set = new Setting;
//    set->setData("HIGH", top);
//    set->setData("LOW", bottom);
//    set->setData("UP", 0);
//    set->setData("DOWN", 0);
    hash.insert(loop, set);

    bottom = top + 0.01;
    top = bottom + range;
  }

  loop = 1;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *cb = close->bar(keys.at(loop));
    if (! cb)
      continue;

    CurveBar *ycb = close->bar(keys.at(loop - 1));
    if (! ycb)
      continue;

    CurveBar *vb = vol->bar(keys.at(loop));
    if (! vb)
      continue;

    int flag = 1;
    if (cb->data() < ycb->data())
      flag = 0;

    QHashIterator<int, Setting *> it(hash);
    while (it.hasNext())
    {
      it.next();
//      Setting *set = it.value();

//      if (cb->data() >= set->getDouble("LOW") && cb->data() <= set->getDouble("HIGH"))
//      {
//        if (flag)
//	  set->setData("UP", set->getDouble("UP") + vb->data());
//	else
//	  set->setData("DOWN", set->getDouble("DOWN") + vb->data());
//	break;
//      }
    }
  }

  Curve *line = new Curve;
//  line->setType("HistogramHorizontal");

  loop = 0;
  int pos = 0;
  for (; loop < 12; loop++)
  {
    Setting *set = hash.value(loop);
    if (! set)
      continue;

    CurveBar *b = new CurveBar;
//    b->setData(0, set->getDouble("HIGH"));
//    b->setData(1, set->getDouble("LOW"));
//    b->setData(2, set->getDouble("UP"));
//    b->setData(3, set->getDouble("DOWN"));
    b->setColor(upColor);
    line->setBar(pos++, b);

    b = new CurveBar;
//    b->setData(0, set->getDouble("HIGH"));
//    b->setData(1, set->getDouble("LOW"));
//    b->setData(2, set->getDouble("UP"));
//    b->setData(3, set->getDouble("DOWN"));
    b->setColor(downColor);
    line->setBar(pos++, b);
  }

  qDeleteAll(hash);

  return line;
}

SettingGroup * CommandVBP::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("CLOSE");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("VOLUME");
  sg->set(ss);

  SettingColor *sc = new SettingColor(QColor("#00FF00"));
  sc->setKey("COLOR_UP");
  sg->set(sc);

  sc = new SettingColor(QColor("#FF0000"));
  sc->setKey("COLOR_DOWN");
  sg->set(sc);

  return sg;
}
