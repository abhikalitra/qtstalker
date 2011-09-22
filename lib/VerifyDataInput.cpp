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

#include "VerifyDataInput.h"
#include "DataFactory.h"
#include "DataSetting.h"
#include "SettingFactory.h"
#include "SettingDouble.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

VerifyDataInput::VerifyDataInput ()
{
}

Data * VerifyDataInput::curve (Script *script, QString key)
{
  Data *in = script->data(key);
  if (in)
  {
    if (in->type() == DataFactory::_CURVE)
    {
      in->setOffset(0);
      return in;
    }

    return 0;
  }

  // verify if an curve offset version (ma.0 or ma.1)
  QStringList l = key.split(".");
  if (l.count() != 2)
    return 0;

  in = script->data(l.at(0));
  if (! in)
    return 0;

  if (in->type() != DataFactory::_CURVE)
    return 0;

  bool ok;
  int offset = l.at(1).toInt(&ok);
  if (! ok)
    return 0;

  in->setOffset(offset);

  return in;
}

Data * VerifyDataInput::curveAll (Script *script, QString key)
{
  Data *in = curve(script, key);
  if (! in)
  {
    in = script->data(key);
    if (! in)
    {
      SettingFactory fac;
      Setting *setting = fac.setting(SettingFactory::_DOUBLE);
      if (! setting)
        return 0;

      if (setting->set(key))
      {
        delete setting;
        return 0;
      }

      DataFactory dfac;
      in = dfac.data(DataFactory::_DATA_SETTING);
      in->set(DataSetting::_VALUE, setting);

      // will delete when script ends
      script->setTData(in);
    }
  }

  return in;
}

Setting * VerifyDataInput::setting (int type, Script *script, QString key)
{
  Setting *set = dataSetting(type, script, key);
  if (set)
    return set;

  SettingFactory fac;
  Setting *setting = fac.setting(type);
  if (! setting)
    return 0;

  if (setting->set(key))
  {
    delete setting;
    return 0;
  }

  // will delete when scipt ends
  script->setTSetting(setting);

  return setting;
}

Setting * VerifyDataInput::dataSetting (int type, Script *script, QString key)
{
  Data *in = script->data(key);
  if (in)
  {
    if (in->type() == DataFactory::_DATA_SETTING)
    {
      Setting *setting = in->get(DataSetting::_VALUE);
      if (! setting)
        return 0;

      if (setting->type() != (SettingFactory::Type) type)
        return 0;

      return setting;
    }

    return 0;
  }

  return 0;
}

int VerifyDataInput::curveKeys (QList<Data *> &list, QList<int> &keys)
{
  keys.clear();

  Data *keyData = 0;
  int minKey = 99999999;
  int loop = 0;
  for (; loop < list.count(); loop++)
  {
    Data *curve = list.at(loop);
    if (curve->type() != DataFactory::_CURVE)
      continue;

    if (curve->barKeyCount() < minKey)
    {
      minKey = curve->barKeyCount();
      keyData = curve;
    }
  }

  if (keyData)
  {
    keys = keyData->barKeys();
    return 0;
  }

  return 1;
}

int VerifyDataInput::curveValue (Data *in, QList<int> &keys, int index, int offset, double &v)
{
  v = 0;
  if (in->type() == DataFactory::_DATA_SETTING)
    v = in->get(DataSetting::_VALUE)->toDouble();
  else
  {
    int i = index - offset;
    if (i < 0)
      return 1;

    Data *bar = in->getData(keys.at(i));
    if (! bar)
      return 1;

    v = bar->get(CurveBar::_VALUE)->toDouble();
  }

  return 0;
}
