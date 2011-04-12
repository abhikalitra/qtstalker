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
#include "VIDYADialog.h"

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

int VIDYA::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int period = settings->getInt("PERIOD");
  int vperiod = settings->getInt("VPERIOD");

  Curve *in = i->line(settings->data("INPUT"));
  if (! in)
  {
    InputType it;
    in = it.input(bd, settings->data("INPUT"));
    if (! in)
    {
      qDebug() << _plugin << "::calculate: no input" << settings->data("INPUT");
      return 1;
    }

    in->setZ(-1);
    i->setLine(settings->data("INPUT"), in);
  }

  Curve *line = getVIDYA(in, period, vperiod);
  if (! line)
    return 1;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);
  
  return 0;  
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

  if (in->count() < period || in->count() < vperiod)
    return 1;

  line = getVIDYA(in, period, vperiod);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * VIDYA::getVIDYA (Curve *in, int period, int vperiod)
{
  Curve *cmo = getCMO(in, vperiod);
  if (! cmo)
    return 0;

  Curve *out = new Curve;

  int size = in->count();
  QVector<double> *inSeries = new QVector<double>(size);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(size);
  offset->fill(0.0);
  QVector<double> *absCmo = new QVector<double>(size);
  absCmo->fill(0.0);
  QVector<double> *vidya = new QVector<double>(size);
  vidya->fill(0.0);

  double c = 2 / (double) period + 1;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
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

Curve * VIDYA::getCMO (Curve *in, int period)
{
  if (in->count() < period)
    return 0;

  TA_Real input[in->count()];
  TA_Real out[in->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_CMO(0,
                         keys.count() - 1,
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

  Curve *line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

QWidget * VIDYA::dialog (QWidget *p, Setting *set)
{
  return new VIDYADialog(p, set);
}

void VIDYA::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("yellow"));
  set->setData("STYLE", QString("Line"));
  set->setData("PERIOD", 10);
  set->setData("VPERIOD", 10);
  set->setData("INPUT", QString("Close"));
  set->setData("Z", 0);
  set->setData("OUTPUT", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  VIDYA *o = new VIDYA;
  return ((Plugin *) o);
}
