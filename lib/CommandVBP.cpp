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
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "SettingDouble.h"
#include "SettingColor.h"

#include <QtDebug>
#include <QHash>

CommandVBP::CommandVBP (QObject *p) : Command (p)
{
  _name = "VBP";
}

int CommandVBP::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *name = vdi.setting(SettingFactory::_STRING, script, s);
  if (! name)
  {
    _message << "invalid OUTPUT " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("CLOSE");
  Data *iclose = vdi.curve(script, s);
  if (! iclose)
  {
    _message << "invalid CLOSE " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("VOLUME");
  Data *ivol = vdi.curve(script, s);
  if (! ivol)
  {
    _message << "invalid VOLUME " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("COLOR_UP");
  Setting *upColor = vdi.setting(SettingFactory::_COLOR, script, s);
  if (! upColor)
  {
    _message << "invalid COLOR_UP " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("COLOR_DOWN");
  Setting *downColor = vdi.setting(SettingFactory::_COLOR, script, s);
  if (! downColor)
  {
    _message << "invalid COLOR_DOWN " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << iclose << ivol;

  Data *line = getVBP(list, upColor->toColor(), downColor->toColor());
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name->toString(), line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandVBP::getVBP (QList<Data *> &list, QColor upColor, QColor downColor)
{
  if (list.count() != 2)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  // get close high/low
  int loop = 0;
  double high = 0;
  double low = 0;
  Data *close = list.at(0);
  Data *vol = list.at(1);
  if (close->highLow(high, low))
    return 0;

  // get the price block range
  double range = (high - low) / 12;
  QHash<int, Data *> hash;
  double bottom = low;
  double top = bottom + range;
  loop = 0;
  for (; loop < 12; loop++)
  {
    Data *set = new Data;
    set->set("HIGH", new SettingDouble(top));
    set->set("LOW", new SettingDouble(bottom));
    set->set("UP", new SettingDouble(0));
    set->set("DOWN", new SettingDouble(0));
    hash.insert(loop, set);

    bottom = top + 0.01;
    top = bottom + range;
  }

  loop = 1;
  for (; loop < keys.count(); loop++)
  {
    Data *cb = close->getData(keys.at(loop));
    if (! cb)
      continue;

    Data *ycb = close->getData(keys.at(loop - 1));
    if (! ycb)
      continue;

    Data *vb = vol->getData(keys.at(loop));
    if (! vb)
      continue;

    int flag = 1;
    if (cb->get(CurveBar::_VALUE)->toDouble() < ycb->get(CurveBar::_VALUE)->toDouble())
      flag = 0;

    QHashIterator<int, Data *> it(hash);
    while (it.hasNext())
    {
      it.next();
      Data *set = it.value();

      if (cb->get(CurveBar::_VALUE)->toDouble() >= set->get("LOW")->toDouble() && cb->get(CurveBar::_VALUE)->toDouble() <= set->get("HIGH")->toDouble())
      {
        if (flag)
	  set->set("UP", new SettingDouble(set->get("UP")->toDouble() + vb->get(CurveBar::_VALUE)->toDouble()));
	else
	  set->set("DOWN", new SettingDouble(set->get("DOWN")->toDouble() + vb->get(CurveBar::_VALUE)->toDouble()));
	break;
      }
    }
  }

  Data *line = new CurveData;
//  line->setType("HistogramHorizontal");

  loop = 0;
  int pos = 0;
  for (; loop < 12; loop++)
  {
    Data *set = hash.value(loop);
    if (! set)
      continue;

    Data *b = new CurveBar;
    b->set(CurveBar::_HIGH, new SettingDouble(set->get("HIGH")->toDouble()));
    b->set(CurveBar::_LOW, new SettingDouble(set->get("LOW")->toDouble()));
    b->set(CurveBar::_OPEN, new SettingDouble(set->get("UP")->toDouble()));
    b->set(CurveBar::_CLOSE, new SettingDouble(set->get("DOWN")->toDouble()));
    b->set(CurveBar::_COLOR, new SettingColor(upColor));
    line->set(pos++, b);

    b = new CurveBar;
    b->set(CurveBar::_HIGH, new SettingDouble(set->get("HIGH")->toDouble()));
    b->set(CurveBar::_LOW, new SettingDouble(set->get("LOW")->toDouble()));
    b->set(CurveBar::_OPEN, new SettingDouble(set->get("UP")->toDouble()));
    b->set(CurveBar::_CLOSE, new SettingDouble(set->get("DOWN")->toDouble()));
    b->set(CurveBar::_COLOR, new SettingColor(downColor));
    line->set(pos++, b);
  }

  qDeleteAll(hash);

  return line;
}
