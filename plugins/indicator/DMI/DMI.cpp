/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "DMI.h"
#include <math.h>

DMI::DMI ()
{
  pluginName = "DMI";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("+DM Color"), "green", Setting::Color);
  set(tr("+DM Line Type"), tr("Line"), Setting::LineType);
  set(tr("+DM Label"), "+DM", Setting::Text);
  set(tr("-DM Color"), "red", Setting::Color);
  set(tr("-DM Line Type"), tr("Line"), Setting::LineType);
  set(tr("-DM Label"), "-DM", Setting::Text);
  set(tr("ADX Color"), "yellow", Setting::Color);
  set(tr("ADX Line Type"), tr("Line"), Setting::LineType);
  set(tr("ADX Label"), "ADX", Setting::Text);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Smoothing"), "9", Setting::Integer);
  set(tr("Smoothing Type"), tr("SMA"), Setting::MAType);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  QStringList l;
  l.append(tr("Crossover"));
  l.append(tr("Extreme Point"));
  l.append(tr("Turning Point"));
  l.sort();
  set(tr("Alert Type"), tr("Crossover"), Setting::List);
  setList(tr("Alert Type"), l);

  about = "Directional Movement Index\n";
}

DMI::~DMI ()
{
}

void DMI::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *mdi = getMDI(period);
  int mdiLoop = mdi->getSize() - 1;

  PlotLine *pdi = getPDI(period);
  int pdiLoop = pdi->getSize() - 1;

  PlotLine *disum = new PlotLine;
  PlotLine *didiff = new PlotLine;

  while (pdiLoop > -1 && mdiLoop > -1)
  {
    disum->prepend(pdi->getData(pdiLoop) + mdi->getData(mdiLoop));
    didiff->prepend(fabs(pdi->getData(pdiLoop) - mdi->getData(mdiLoop)));
    pdiLoop--;
    mdiLoop--;
  }

  int sumLoop = disum->getSize() - 1;
  int diffLoop = didiff->getSize() - 1;

  PlotLine *dx = new PlotLine;

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

  PlotLine *adx = getMA(dx, getData(tr("Smoothing Type")), getInt(tr("Smoothing")));
  adx->setColor(getData(tr("ADX Color")));
  adx->setType(getData(tr("ADX Line Type")));
  adx->setLabel(getData(tr("ADX Label")));

  delete disum;
  delete didiff;
  delete dx;

  mdi->setColor(getData(tr("-DM Color")));
  mdi->setType(getData(tr("-DM Line Type")));
  mdi->setLabel(getData(tr("-DM Label")));
  output.append(mdi);

  pdi->setColor(getData(tr("+DM Color")));
  pdi->setType(getData(tr("+DM Line Type")));
  pdi->setLabel(getData(tr("+DM Label")));
  output.append(pdi);

  output.append(adx);
}

QMemArray<int> DMI::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 3)
    return alerts;

  QString s = getData(tr("Alert Type"));
  if (! s.compare(tr("Crossover")))
    alertCrossover();
  else
  {
    if (! s.compare(tr("Extreme Point")))
      alertExtremePoint();
    else
      alertTurningPoint();
  }

  return alerts;
}

void DMI::alertCrossover ()
{
  PlotLine *mdi = output.at(0);
  PlotLine *pdi = output.at(1);

  int loop;
  int dataLoop = data->count() - mdi->getSize();
  int status = 0;
  for (loop = 0; loop < (int) mdi->getSize(); loop++, dataLoop++)
  {
    if (pdi->getData(loop) > mdi->getData(loop))
      status = 1;
    else
    {
      if (mdi->getData(loop) > pdi->getData(loop))
        status = -1;
    }

    alerts[dataLoop] = status;
  }
}

void DMI::alertExtremePoint ()
{
  PlotLine *mdi = output.at(0);
  PlotLine *pdi = output.at(1);

  int loop;
  int dataLoop = data->count() - mdi->getSize();
  int status = 0;
  double point = 0;
  for (loop = 0; loop < (int) mdi->getSize(); loop++, dataLoop++)
  {
    Setting *r = data->at(dataLoop);

    if (pdi->getData(loop) > mdi->getData(loop))
    {
      if (status != 1)
      {
        if (point == 0)
          point = r->getFloat("High");
        else
        {
	  if (r->getFloat("Close") > point)
          {
            status = 1;
	    point = 0;
	  }
        }
      }
    }

    if (mdi->getData(loop) > pdi->getData(loop))
    {
      if (status != -1)
      {
        if (point == 0)
          point = r->getFloat("Low");
        else
        {
          if (r->getFloat("Close") < point)
	  {
	    status = -1;
	    point = 0;
	  }
	}
      }
    }

    alerts[dataLoop] = status;
  }
}

void DMI::alertTurningPoint ()
{
/*
  PlotLine *mdi = output.at(0);
  PlotLine *pdi = output.at(1);
  PlotLine *adx = output.at(2);

  int listLoop = data->count() - adx->getSize() + 1;
  int mdiLoop = mdi->getSize() - adx->getSize() + 1;
  int pdiLoop = pdi->getSize() - adx->getSize() + 1;
  int adxLoop;
  int status = 0;
  for (adxLoop = 1; adxLoop < (int) adx->getSize(); adxLoop++, mdiLoop++, pdiLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if ((pdi->getData(pdiLoop) > mdi->getData(mdiLoop)) && (adx->getData(adxLoop) > adx->getData(adxLoop - 1)))
	  status = 1;
	break;
      case 1:
        if ((pdi->getData(pdiLoop) < mdi->getData(mdiLoop)) && (adx->getData(adxLoop) > adx->getData(adxLoop - 1)))
	  status = -1;
	break;
      default:
        if ((pdi->getData(pdiLoop) > mdi->getData(mdiLoop)) && (adx->getData(adxLoop) > adx->getData(adxLoop - 1)))
	  status = 1;
	else
	{
          if ((pdi->getData(pdiLoop) < mdi->getData(mdiLoop)) && (adx->getData(adxLoop) > adx->getData(adxLoop - 1)))
	    status = -1;
	}
	break;
    }

    alerts[listLoop] = status;
  }
*/
}

/*
QMemArray<int> ADX::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *adx = output.at(0);
  int adxLoop;
  int listLoop = data->count() - adx->getSize() + 2;
  int status = 0;
  for (adxLoop = 2; adxLoop < (int) adx->getSize(); adxLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if ((adx->getData(adxLoop) > adx->getData(adxLoop - 1)) && (adx->getData(adxLoop - 1) <= adx->getData(adxLoop - 2)))
          status = 1;
	break;
      case 1:
        if ((adx->getData(adxLoop) < adx->getData(adxLoop - 1)) && (adx->getData(adxLoop - 1) >= adx->getData(adxLoop - 2)))
	  status = -1;
	break;
      default:
        if ((adx->getData(adxLoop) > adx->getData(adxLoop - 1)) && (adx->getData(adxLoop - 1) <= adx->getData(adxLoop - 2)))
	  status = 1;
	else
	{
          if ((adx->getData(adxLoop) < adx->getData(adxLoop - 1)) && (adx->getData(adxLoop - 1) >= adx->getData(adxLoop - 2)))
	    status = -1;
	}
	break;
    }

    alerts[listLoop] = status;
  }

  return alerts;
}
*/

PlotLine * DMI::getMDI (int period)
{
  PlotLine *mdm = new PlotLine();

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    set = data->at(loop - 1);
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

  PlotLine *tr = getTR();

  PlotLine *smamdm = getSMA(mdm, period);
  int mdmLoop = smamdm->getSize() - 1;

  PlotLine *smatr = getSMA(tr, period);
  int trLoop = smatr->getSize() - 1;

  PlotLine *mdi = new PlotLine();

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

PlotLine * DMI::getPDI (int period)
{
  PlotLine *pdm = new PlotLine();

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    set = data->at(loop - 1);
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

  PlotLine *tr = getTR();

  PlotLine *smapdm = getSMA(pdm, period);
  int pdmLoop = smapdm->getSize() - 1;

  PlotLine *smatr = getSMA(tr, period);
  int trLoop = smatr->getSize() - 1;

  PlotLine *pdi = new PlotLine();

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
  DMI *o = new DMI;
  return ((Plugin *) o);
}

