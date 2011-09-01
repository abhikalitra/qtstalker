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

#include "CommandVFI.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>
#include <cmath>

CommandVFI::CommandVFI (QObject *p) : Command (p)
{
  _type = "VFI";
}

int CommandVFI::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("HIGH");
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW");
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  s = sg->get("CLOSE");
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

  int period = sg->getInteger("PERIOD");

  QList<Data *> list;
  list << ihigh << ilow << iclose << ivol;
  line = getVFI(list, period);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandVFI::getVFI (QList<Data *> &list, int period)
{
  if (list.count() != 4)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Data *vfi = new CurveData;
  Data *ihigh = list.at(0);
  Data *ilow = list.at(1);
  Data *iclose = list.at(2);
  Data *ivol = list.at(3);
  int loop = period;
  for (; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->getData(keys.at(loop - period));
    if (! hbar)
      continue;

    Data *lbar = ilow->getData(keys.at(loop - period));
    if (! lbar)
      continue;

    Data *cbar = iclose->getData(keys.at(loop - period));
    if (! cbar)
      continue;

    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = cbar->getDouble(CurveBar::_VALUE);
    double high = hbar->getDouble(CurveBar::_VALUE);
    double low = lbar->getDouble(CurveBar::_VALUE);
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->getData(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->getData(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->getData(keys.at(i));
      if (! cbar)
        continue;

      Data *vbar = ivol->getData(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      close = cbar->getDouble(CurveBar::_VALUE);
      high = hbar->getDouble(CurveBar::_VALUE);
      low = lbar->getDouble(CurveBar::_VALUE);
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += vbar->getDouble(CurveBar::_VALUE);
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    hbar = ihigh->getData(keys.at(loop - period));
    if (! hbar)
      continue;

    lbar = ilow->getData(keys.at(loop - period));
    if (! lbar)
      continue;

    cbar = iclose->getData(keys.at(loop - period));
    if (! cbar)
      continue;

    close = cbar->getDouble(CurveBar::_VALUE);
    high = hbar->getDouble(CurveBar::_VALUE);
    low = lbar->getDouble(CurveBar::_VALUE);
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->getData(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->getData(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->getData(keys.at(i));
      if (! cbar)
        continue;

      Data *vbar = ivol->getData(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      double volume = vbar->getDouble(CurveBar::_VALUE);
      close = cbar->getDouble(CurveBar::_VALUE);
      high = hbar->getDouble(CurveBar::_VALUE);
      low = lbar->getDouble(CurveBar::_VALUE);
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, t);
    vfi->set(keys.at(loop), b);
  }

  return vfi;
}

Data * CommandVFI::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("HIGH", QString());
  sg->set("LOW", QString());
  sg->set("CLOSE", QString());
  sg->set("VOLUME", QString());
  sg->set("PERIOD", 10);
  return sg;
}
