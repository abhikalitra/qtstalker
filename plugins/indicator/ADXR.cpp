/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "ADXR.h"
#include <math.h>

ADXR::ADXR ()
{
  pluginName = "ADXR";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Average Directional Index Smoothed\n";
}

ADXR::~ADXR ()
{
}

void ADXR::calculate ()
{
  SettingItem *set = getItem(tr("Period"));
  int period = set->data.toInt();

  Output *mdi = getMDI(period);
  int mdiLoop = mdi->getSize() - 1;

  Output *pdi = getPDI(period);
  int pdiLoop = pdi->getSize() - 1;

  Output *disum = new Output;
  Output *didiff = new Output;

  while (pdiLoop > -1 && mdiLoop > -1)
  {
    disum->prepend(pdi->getData(pdiLoop) + mdi->getData(mdiLoop));
    didiff->prepend(fabs(pdi->getData(pdiLoop) - mdi->getData(mdiLoop)));
    pdiLoop--;
    mdiLoop--;
  }

  int sumLoop = disum->getSize() - 1;
  int diffLoop = didiff->getSize() - 1;

  Output *dx = new Output;

  while (sumLoop > -1 && diffLoop > -1)
  {
    int t = (int) ((didiff->getData(diffLoop) / disum->getData(sumLoop)) * 100);
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    dx->prepend(t);

    sumLoop--;
    diffLoop--;
  }

  Output *adx = getSMA(dx, period);

  if (period >= (int) adx->getSize())
  {
    delete mdi;
    delete pdi;
    delete disum;
    delete didiff;
    delete dx;
    delete adx;
    return;
  }

  Output *adxr = new Output;

  int adxLoop = adx->getSize() - 1;

  while (adxLoop >= period)
  {
    double t = (int) ((adx->getData(adxLoop) + adx->getData(adxLoop - period)) / 2);
    adxr->prepend(t);
    adxLoop--;
  }

  delete mdi;
  delete pdi;
  delete disum;
  delete didiff;
  delete dx;
  delete adx;

  output.append(adxr);
}

QMemArray<int> ADXR::getAlerts ()
{
  alerts.fill(0, data.count());

  if (! output.count())
    return alerts;

  Output *adxr = output.at(0);
  int adxrLoop;
  int listLoop = data.count() - adxr->getSize() + 2;
  int status = 0;
  for (adxrLoop = 2; adxrLoop < (int) adxr->getSize(); adxrLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if ((adxr->getData(adxrLoop) > adxr->getData(adxrLoop - 1)) && (adxr->getData(adxrLoop - 1) <= adxr->getData(adxrLoop - 2)))
          status = 1;
	break;
      case 1:
        if ((adxr->getData(adxrLoop) < adxr->getData(adxrLoop - 1)) && (adxr->getData(adxrLoop - 1) >= adxr->getData(adxrLoop - 2)))
	  status = -1;
	break;
      default:
        if ((adxr->getData(adxrLoop) > adxr->getData(adxrLoop - 1)) && (adxr->getData(adxrLoop - 1) <= adxr->getData(adxrLoop - 2)))
	  status = 1;
	else
	{
          if ((adxr->getData(adxrLoop) < adxr->getData(adxrLoop - 1)) && (adxr->getData(adxrLoop - 1) >= adxr->getData(adxrLoop - 2)))
	    status = -1;
	}
	break;
    }

    alerts[listLoop] = status;
  }

  return alerts;
}

Output * ADXR::getMDI (int period)
{
  Output *mdm = new Output();

  int loop;
  for (loop = 1; loop < (int) data.count(); loop++)
  {
    Setting *set = data.at(loop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    set = data.at(loop - 1);
    double yhigh = set->getFloat("High");
    double ylow = set->getFloat("Low");
    double t = 0;

    if (high > yhigh)
      t = 0;
    else
    {
      if (low < ylow)
        t = ylow - low;
      else
	t = 0;
    }

    mdm->append(t);
  }

  Output *tr = getTR();

  Output *smamdm = getSMA(mdm, period);
  int mdmLoop = smamdm->getSize() - 1;

  Output *smatr = getSMA(tr, period);
  int trLoop = smatr->getSize() - 1;

  Output *mdi = new Output();

  while (mdmLoop > -1 && trLoop > -1)
  {
    int t = (int) ((smamdm->getData(mdmLoop) / smatr->getData(trLoop)) * 100);
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    mdi->prepend(t);

    mdmLoop--;
    trLoop--;
  }

  delete mdm;
  delete tr;
  delete smamdm;
  delete smatr;

  return mdi;
}

Output * ADXR::getPDI (int period)
{
  Output *pdm = new Output();

  int loop;
  for (loop = 1; loop < (int) data.count(); loop++)
  {
    Setting *set = data.at(loop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    set = data.at(loop - 1);
    double yhigh = set->getFloat("High");
    double ylow = set->getFloat("Low");
    double t = 0;

    if (high > yhigh)
      t = high - yhigh;
    else
    {
      if (low < ylow)
	t = 0;
      else
      	t = 0;
    }

    pdm->append(t);
  }

  Output *tr = getTR();

  Output *smapdm = getSMA(pdm, period);
  int pdmLoop = smapdm->getSize() - 1;

  Output *smatr = getSMA(tr, period);
  int trLoop = smatr->getSize() - 1;

  Output *pdi = new Output();

  while (pdmLoop > -1 && trLoop > -1)
  {
    int t = (int) ((smapdm->getData(pdmLoop) / smatr->getData(trLoop)) * 100);
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    pdi->prepend(t);

    pdmLoop--;
    trLoop--;
  }

  delete pdm;
  delete tr;
  delete smapdm;
  delete smatr;

  return pdi;
}

Plugin * create ()
{
  ADXR *o = new ADXR;
  return ((Plugin *) o);
}

