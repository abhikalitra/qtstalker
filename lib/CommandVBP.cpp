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
#include "DataDouble.h"
#include "DataColor.h"

#include <QtDebug>
#include <QHash>

CommandVBP::CommandVBP (QObject *p) : Command (p)
{
  _name = "VBP";
}

int CommandVBP::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandVBP::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // CLOSE
  s = sg->value("CLOSE");
  Data *iclose = vdi.toCurve(script, s);
  if (! iclose)
  {
    qDebug() << "CommandVBP::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // VOLUME
  s = sg->value("VOLUME");
  Data *ivol = vdi.toCurve(script, s);
  if (! ivol)
  {
    qDebug() << "CommandVBP::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // COLOR_UP
  QColor upColor;
  s = sg->value("COLOR_UP");
  if (vdi.toColor(script, s, upColor))
  {
    qDebug() << "CommandVBP::runScript: invalid COLOR_UP" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // COLOR_DOWN
  QColor downColor;
  s = sg->value("COLOR_DOWN");
  if (vdi.toColor(script, s, downColor))
  {
    qDebug() << "CommandVBP::runScript: invalid COLOR_DOWN" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << iclose << ivol;

  Data *line = getVBP(list, upColor, downColor);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name, line);

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
    Data *set = new CurveBar;
    set->set(CurveBar::_HIGH, new DataDouble(top));
    set->set(CurveBar::_LOW, new DataDouble(bottom));
    set->set(CurveBar::_UP, new DataDouble(0));
    set->set(CurveBar::_DOWN, new DataDouble(0));
    hash.insert(loop, set);

    bottom = top + 0.01;
    top = bottom + range;
  }

  loop = 1;
  for (; loop < keys.count(); loop++)
  {
    Data *cb = close->toData(keys.at(loop));
    if (! cb)
      continue;

    Data *ycb = close->toData(keys.at(loop - 1));
    if (! ycb)
      continue;

    Data *vb = vol->toData(keys.at(loop));
    if (! vb)
      continue;

    int flag = 1;
    if (cb->toData(CurveBar::_VALUE)->toDouble() < ycb->toData(CurveBar::_VALUE)->toDouble())
      flag = 0;

    QHashIterator<int, Data *> it(hash);
    while (it.hasNext())
    {
      it.next();
      Data *set = it.value();

      if (cb->toData(CurveBar::_VALUE)->toDouble() >= set->toData(CurveBar::_LOW)->toDouble() && cb->toData(CurveBar::_VALUE)->toDouble() <= set->toData(CurveBar::_HIGH)->toDouble())
      {
        if (flag)
	  set->set(CurveBar::_UP, new DataDouble(set->toData(CurveBar::_UP)->toDouble() + vb->toData(CurveBar::_VALUE)->toDouble()));
	else
	  set->set(CurveBar::_DOWN, new DataDouble(set->toData(CurveBar::_DOWN)->toDouble() + vb->toData(CurveBar::_VALUE)->toDouble()));
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
    b->set(CurveBar::_HIGH, new DataDouble(set->toData(CurveBar::_HIGH)->toDouble()));
    b->set(CurveBar::_LOW, new DataDouble(set->toData(CurveBar::_LOW)->toDouble()));
    b->set(CurveBar::_OPEN, new DataDouble(set->toData(CurveBar::_UP)->toDouble()));
    b->set(CurveBar::_CLOSE, new DataDouble(set->toData(CurveBar::_DOWN)->toDouble()));
    b->set(CurveBar::_COLOR, new DataColor(upColor));
    line->set(pos++, b);

    b = new CurveBar;
    b->set(CurveBar::_HIGH, new DataDouble(set->toData(CurveBar::_HIGH)->toDouble()));
    b->set(CurveBar::_LOW, new DataDouble(set->toData(CurveBar::_LOW)->toDouble()));
    b->set(CurveBar::_OPEN, new DataDouble(set->toData(CurveBar::_UP)->toDouble()));
    b->set(CurveBar::_CLOSE, new DataDouble(set->toData(CurveBar::_DOWN)->toDouble()));
    b->set(CurveBar::_COLOR, new DataColor(downColor));
    line->set(pos++, b);
  }

  qDeleteAll(hash);

  return line;
}
