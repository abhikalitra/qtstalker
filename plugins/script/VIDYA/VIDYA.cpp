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

#include "VIDYA.h"
#include "Globals.h"
#include "ta_libc.h"
#include "InputType.h"

#include <QtDebug>
#include <cmath>
#include <QVector>

#define PI 3.14159265

VIDYA::VIDYA ()
{
  _plugin = "VIDYA";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("VIDYA::VIDYA: error on TA_Initialize");
}

int VIDYA::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // PERIOD
  // PERIOD_VOLUME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  int vperiod = command->parm("PERIOD_VOLUME").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_VOLUME" << command->parm("PERIOD_VOLUME");
    return 1;
  }

  QList<Curve *> list;
  list << in;
  line = getVIDYA(list, period, vperiod);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * VIDYA::getVIDYA (QList<Curve *> &list, int period, int vperiod)
{
  if (! list.count())
    return 0;
  
  Curve *cmo = getCMO(list, vperiod);
  if (! cmo)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;
  int size = keys.count();

  Curve *out = new Curve;

  QVector<double> *inSeries = new QVector<double>(size);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(size);
  offset->fill(0.0);
  QVector<double> *absCmo = new QVector<double>(size);
  absCmo->fill(0.0);
  QVector<double> *vidya = new QVector<double>(size);
  vidya->fill(0.0);

  double c = 2 / (double) period + 1;

  int loop = 0;
  Curve *in = list.at(0);
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    (*inSeries)[loop] = bar->data();
  }

  keys.clear();
  cmo->keys(keys);

  int index = inSeries->size() -1;
  loop = keys.count() - 1;
  for (; loop > -1; loop--)
  {
    CurveBar *bar = cmo->bar(keys.at(loop));
    (*absCmo)[index] = fabs(bar->data() / 100);
    index--;
  }

  loop = vperiod + period;
  for (; loop < (int) inSeries->size(); loop++)         // period safty
  {
    (*vidya)[loop] = (inSeries->at(loop) * c * absCmo->at(loop)) + ((1 - absCmo->at(loop) * c) * vidya->at(loop - 1));
    out->setBar(loop, new CurveBar(vidya->at(loop)));
  }

  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;

  return out;
}

Curve * VIDYA::getCMO (QList<Curve *> &list, int period)
{
  if (! list.count())
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input[0], &input[0], &input[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_CMO(0,
                         size - 1,
                         &input[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getCMO: TA-Lib error" << rc;
    return 0;
  }

  QList<Curve *> outs;
  Curve *c = new Curve;
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

void VIDYA::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME" << "INPUT" << "PERIOD" << "PERIOD_VOLUME";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME", _plugin);
  set->setData("NAME:TYPE", QString("TEXT"));
  set->setData("INPUT", QString("Close"));
  set->setData("INPUT:TYPE", QString("TEXT"));
  set->setData("PERIOD", 10);
  set->setData("PERIOD:TYPE", QString("INTEGER"));
  set->setData("PERIOD_VOLUME", 10);
  set->setData("PERIOD_VOLUME:TYPE", QString("INTEGER"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  VIDYA *o = new VIDYA;
  return ((Plugin *) o);
}
