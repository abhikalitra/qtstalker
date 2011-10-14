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

#include "CommandVIDYA.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataDouble.h"

#include <QtDebug>
#include <cmath>
#include <QVector>

#define PI 3.14159265

CommandVIDYA::CommandVIDYA (QObject *p) : Command (p)
{
  _name = "VIDYA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandVIDYA::CommandVIDYA: error on TA_Initialize");
}

int CommandVIDYA::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandVIDYA::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandVIDYA::runScript: invalid INPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD
  int period = 14;
  s = sg->value("PERIOD");
  if (vdi.toInteger(script, s, period))
  {
    qDebug() << "CommandVIDYA::runScript: invalid PERIOD, using default" << s;
    period = 14;
  }

  // PERIOD_VOLUME
  int vperiod = 10;
  s = sg->value("PERIOD_VOLUME");
  if (vdi.toInteger(script, s, vperiod))
  {
    qDebug() << "CommandVIDYA::runScript: invalid PERIOD_VOLUME, using default" << s;
    period = 10;
  }

  QList<Data *> list;
  list << in;

  Data *line = getVIDYA(list, period, vperiod);
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

Data * CommandVIDYA::getVIDYA (QList<Data *> &list, int period, int vperiod)
{
  if (! list.count())
    return 0;

  Data *cmo = getCMO(list, vperiod);
  if (! cmo)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;
  int size = keys.count();

  Data *out = new CurveData;

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
  Data *in = list.at(0);
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->toData(keys.at(loop));
    (*inSeries)[loop] = bar->toData(CurveBar::_VALUE)->toDouble();
  }

  keys = cmo->keys();

  int index = inSeries->size() -1;
  loop = keys.count() - 1;
  for (; loop > -1; loop--)
  {
    Data *bar = cmo->toData(keys.at(loop));
    (*absCmo)[index] = fabs(bar->toData(CurveBar::_VALUE)->toDouble() / 100);
    index--;
  }

  loop = vperiod + period;
  for (; loop < (int) inSeries->size(); loop++)         // period safty
  {
    (*vidya)[loop] = (inSeries->at(loop) * c * absCmo->at(loop)) + ((1 - absCmo->at(loop) * c) * vidya->at(loop - 1));

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(vidya->at(loop)));
    out->set(loop, b);
  }

  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;

  return out;
}

Data * CommandVIDYA::getCMO (QList<Data *> &list, int period)
{
  if (! list.count())
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill1(list, keys, &input[0]);
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
    qDebug() << _type << "::getCMO: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new CurveData;
  outs.append(c);

  TALibOutput tao;
  if (tao.fillDouble1(outs, keys, outNb, &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}
