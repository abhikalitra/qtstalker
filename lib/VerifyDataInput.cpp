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
#include "CurveData.h"
#include "CurveBar.h"
#include "DataBarLength.h"
#include "DataBool.h"
#include "DataColor.h"
#include "DataDateRange.h"
#include "DataDateTime.h"
#include "DataDouble.h"
#include "DataFile.h"
#include "DataFont.h"
#include "DataInteger.h"
#include "DataList.h"
#include "DataMA.h"
#include "DataOp.h"
#include "DataString.h"
#include "DataSymbol.h"

#include <QtDebug>

VerifyDataInput::VerifyDataInput ()
{
}

int VerifyDataInput::toString (Script *script, QString key, QString &data)
{
  if (key.isEmpty())
    return 1;

  data.clear();

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_STRING)
    {
      data = d->toString();
      return 0;
    }
    else
      return 1;
  }

  data = key;

  return 0;
}

int VerifyDataInput::toInteger (Script *script, QString key, int &data)
{
  if (key.isEmpty())
    return 1;

  data = 0;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_INTEGER)
    {
      data = d->toInteger();
      return 0;
    }
    else
      return 1;
  }

  DataInteger di;
  if (di.set(key))
    return 1;

  data = di.toInteger();

  return 0;
}

int VerifyDataInput::toDouble (Script *script, QString key, double &data)
{
  if (key.isEmpty())
    return 1;

  data = 0;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_DOUBLE)
    {
      data = d->toDouble();
      return 0;
    }
    else
      return 1;
  }

  DataDouble dd;
  if (dd.set(key))
    return 1;

  data = dd.toDouble();

  return 0;
}

int VerifyDataInput::toBool (Script *script, QString key, bool &data)
{
  if (key.isEmpty())
    return 1;

  data = FALSE;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_BOOL)
    {
      data = d->toBool();
      return 0;
    }
    else
      return 1;
  }

  DataBool db;
  if (db.set(key))
    return 1;

  data = db.toBool();

  return 0;
}

int VerifyDataInput::toColor (Script *script, QString key, QColor &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_COLOR)
    {
      data = d->toColor();
      return 0;
    }
    else
      return 1;
  }

  DataColor dc;
  if (dc.set(key))
    return 1;

  data = dc.toColor();

  return 0;
}

int VerifyDataInput::toBarLength (Script *script, QString key, int &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_BAR_LENGTH)
    {
      data = d->toInteger();
      return 0;
    }
    else
      return 1;
  }

  DataBarLength dbl;
  if (dbl.set(key))
    return 1;

  data = dbl.toInteger();

  return 0;
}

int VerifyDataInput::toDateRange (Script *script, QString key, int &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_DATE_RANGE)
    {
      data = d->toInteger();
      return 0;
    }
    else
      return 1;
  }

  DataDateRange ddr;
  if (ddr.set(key))
    return 1;

  data = ddr.toInteger();

  return 0;
}

int VerifyDataInput::toDateTime (Script *script, QString key, QDateTime &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_DATETIME)
    {
      data = d->toDateTime();
      return 0;
    }
    else
      return 1;
  }

  DataDateTime ddt;
  if (ddt.set(key))
    return 1;

  data = ddt.toDateTime();

  return 0;
}

int VerifyDataInput::toFile (Script *script, QString key, QStringList &data)
{
  if (key.isEmpty())
    return 1;

  data.clear();

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_FILE)
    {
      data = d->toList();
      return 0;
    }
    else
      return 1;
  }

  DataFile df;
  if (df.set(key))
    return 1;

  data = df.toList();

  return 0;
}

int VerifyDataInput::toFont (Script *script, QString key, QFont &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_FONT)
    {
      data = d->toFont();
      return 0;
    }
    else
      return 1;
  }

  DataFont df;
  if (df.set(key))
    return 1;

  data = df.toFont();

  return 0;
}

int VerifyDataInput::toMA (Script *script, QString key, int &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_MA)
    {
      data = d->toInteger();
      return 0;
    }
    else
      return 1;
  }

  DataMA dma;
  if (dma.set(key))
    return 1;

  data = dma.toInteger();

  return 0;
}

int VerifyDataInput::toOp (Script *script, QString key, int &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_OP)
    {
      data = d->toInteger();
      return 0;
    }
    else
      return 1;
  }

  DataOp dop;
  if (dop.set(key))
    return 1;

  data = dop.toInteger();

  return 0;
}

int VerifyDataInput::toSymbol (Script *script, QString key, QStringList &data)
{
  if (key.isEmpty())
    return 1;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_SYMBOL)
    {
      data = d->toList();
      return 0;
    }
    else
      return 1;
  }

  DataSymbol ds;
  if (ds.set(key))
    return 1;

  data = ds.toList();

  return 0;
}

int VerifyDataInput::toList (Script *script, QString key, QString &data)
{
  if (key.isEmpty())
    return 1;

  data.clear();

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_LIST)
    {
      data = d->toString();
      return 0;
    }
    else
      return 1;
  }

  data = key;

  return 0;
}

Data * VerifyDataInput::toCurve (Script *script, QString key)
{
  if (key.isEmpty())
    return 0;

  Data *d = script->data(key);
  if (d)
  {
    if (d->type() == DataFactory::_CURVE)
    {
      d->set(CurveData::_OFFSET, new DataInteger(0));
      return d;
    }
    else
      return 0;
  }

  DataDouble *dd = new DataDouble;
  if (! dd->set(key))
  {
    dd->setDeleteFlag(1);
    script->setTData(dd);
    return dd;
  }

  // verify if a curve offset version (ma.0 or ma.1)
  QStringList l = key.split(".");
  if (l.count() != 2)
    return 0;

  d = script->data(l.at(0));
  if (! d)
    return 0;

  if (d->type() != DataFactory::_CURVE)
    return 0;

  DataInteger *di = new DataInteger;
  if (di->set(l.at(1)))
  {
    delete di;
    return 0;
  }

  d->set(CurveData::_OFFSET, di);

  return d;
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

    if (curve->keyCount() < minKey)
    {
      minKey = curve->keyCount();
      keyData = curve;
    }
  }

  if (keyData)
  {
    keys = keyData->keys();
    return 0;
  }

  return 1;
}

int VerifyDataInput::curveValue (Data *in, QList<int> &keys, int index, int offset, double &v)
{
  v = 0;
  switch ((DataFactory::Type) in->type())
  {
    case DataFactory::_DOUBLE:
      v = in->toDouble();
      break;
    case DataFactory::_CURVE:
    {
      int pos = index;
      pos -= offset;
      if (pos < 0)
        return 1;

      Data *bar = in->toData(keys.at(pos));
      if (! bar)
        return 1;

      v = bar->toData(CurveBar::_VALUE)->toDouble();
      break;
    }
    default:
      return 1;
      break;
  }

  return 0;
}
