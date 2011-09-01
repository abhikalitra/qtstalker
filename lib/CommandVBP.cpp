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
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>
#include <QHash>

CommandVBP::CommandVBP (QObject *p) : Command (p)
{
  _type = "VBP";
}

int CommandVBP::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("CLOSE");
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  s = sg->get("VOLUME");
  Data *ivol = script->data(s);
  if (! ivol)
  {
    qDebug() << _type << "::runScript: invalid VOLUME" << s;
    return _ERROR;
  }

  QColor upColor = sg->getColor("COLOR_UP");

  QColor downColor = sg->getColor("COLOR_DOWN");

  QList<Data *> list;
  list << iclose << ivol;
  line = getVBP(list, upColor, downColor);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandVBP::getVBP (QList<Data *> &list, QColor upColor, QColor downColor)
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
    set->set("HIGH", top);
    set->set("LOW", bottom);
    set->set("UP", 0);
    set->set("DOWN", 0);
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
    if (cb->getDouble(CurveBar::_VALUE) < ycb->getDouble(CurveBar::_VALUE))
      flag = 0;

    QHashIterator<int, Data *> it(hash);
    while (it.hasNext())
    {
      it.next();
      Data *set = it.value();

      if (cb->getDouble(CurveBar::_VALUE) >= set->getDouble("LOW") && cb->getDouble(CurveBar::_VALUE) <= set->getDouble("HIGH"))
      {
        if (flag)
	  set->set("UP", set->getDouble("UP") + vb->getDouble(CurveBar::_VALUE));
	else
	  set->set("DOWN", set->getDouble("DOWN") + vb->getDouble(CurveBar::_VALUE));
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
    b->set(CurveBar::_HIGH, set->getDouble("HIGH"));
    b->set(CurveBar::_LOW, set->getDouble("LOW"));
    b->set(CurveBar::_OPEN, set->getDouble("UP"));
    b->set(CurveBar::_CLOSE, set->getDouble("DOWN"));
    b->set(CurveBar::_COLOR, upColor);
    line->set(pos++, b);

    b = new CurveBar;
    b->set(CurveBar::_HIGH, set->getDouble("HIGH"));
    b->set(CurveBar::_LOW, set->getDouble("LOW"));
    b->set(CurveBar::_OPEN, set->getDouble("UP"));
    b->set(CurveBar::_CLOSE, set->getDouble("DOWN"));
    b->set(CurveBar::_COLOR, downColor);
    line->set(pos++, b);
  }

  qDeleteAll(hash);

  return line;
}

Data * CommandVBP::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("CLOSE", QString());
  sg->set("VOLUME", QString());
  sg->set("COLOR_UP", QColor(Qt::green));
  sg->set("COLOR_DOWN", QColor(Qt::red));
  return sg;
}
