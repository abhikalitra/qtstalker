/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "PrefDialog.h"
#include <qdict.h>

DMI::DMI ()
{
  pluginName = "DMI";
  plotFlag = FALSE;
  alertFlag = TRUE;
  setDefaults();

  alertList.append(tr("Crossover"));
  alertList.append(tr("Extreme Point"));
  alertList.append(tr("Turning Point"));
  alertList.sort();
}

DMI::~DMI ()
{
}

void DMI::setDefaults ()
{
  mdiColor.setNamedColor("red");
  pdiColor.setNamedColor("green");
  adxColor.setNamedColor("yellow");
  pdiLineType = PlotLine::Line;
  mdiLineType = PlotLine::Line;
  adxLineType = PlotLine::Line;
  pdiLabel = "+DM";
  mdiLabel = "-DM";
  adxLabel = "ADX";
  period = 14;
  smoothing = 9;
  maType = IndicatorPlugin::SMA;  
  alertType = tr("Crossover");
}

void DMI::calculate ()
{
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

  PlotLine *adx = getMA(dx, maType, smoothing);
  adx->setColor(adxColor);
  adx->setType(adxLineType);
  adx->setLabel(adxLabel);

  delete disum;
  delete didiff;
  delete dx;

  mdi->setColor(mdiColor);
  mdi->setType(mdiLineType);
  mdi->setLabel(mdiLabel);
  output.append(mdi);

  pdi->setColor(pdiColor);
  pdi->setType(pdiLineType);
  pdi->setLabel(pdiLabel);
  output.append(pdi);

  output.append(adx);
}

QMemArray<int> DMI::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 3)
    return alerts;

  if (! alertType.compare(tr("Crossover")))
    alertCrossover();
  else
  {
    if (! alertType.compare(tr("Extreme Point")))
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
    if (pdi->getData(loop) > mdi->getData(loop))
    {
      if (status != 1)
      {
        if (point == 0)
          point = data->getHigh(dataLoop);
        else
        {
	  if (data->getClose(dataLoop) > point)
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
          point = data->getLow(dataLoop);
        else
        {
          if (data->getClose(dataLoop) < point)
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
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double yhigh = data->getHigh(loop - 1);
    double ylow = data->getLow(loop - 1);
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
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double yhigh = data->getHigh(loop - 1);
    double ylow = data->getLow(loop - 1);
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

int DMI::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("DMI Indicator"));
  
  dialog->createPage (tr("DMI"));
  dialog->addIntItem(tr("Period"), 1, period, 1, 99999999);
  dialog->addIntItem(tr("Smoothing"), 1, smoothing, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), 1, getMATypes(), maType);
  dialog->addComboItem(tr("Alert"), 1, alertList, alertType);
  
  dialog->createPage (tr("+DM"));
  dialog->addColorItem(tr("+DM Color"), 2, pdiColor);
  dialog->addTextItem(tr("+DM Label"), 2, pdiLabel);
  dialog->addComboItem(tr("+DM Line Type"), 2, lineTypes, pdiLineType);
  
  dialog->createPage (tr("-DM"));
  dialog->addColorItem(tr("-DM Color"), 3, mdiColor);
  dialog->addTextItem(tr("-DM Label"), 3, mdiLabel);
  dialog->addComboItem(tr("-DM Line Type"), 3, lineTypes, mdiLineType);
  
  dialog->createPage (tr("ADX"));
  dialog->addColorItem(tr("ADX Color"), 4, adxColor);
  dialog->addTextItem(tr("ADX Label"), 4, adxLabel);
  dialog->addComboItem(tr("ADX Line Type"), 4, lineTypes, adxLineType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(tr("Period"));
    smoothing = dialog->getInt(tr("Smoothing"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    alertType = dialog->getCombo(tr("Alert"));
    
    pdiColor = dialog->getColor(tr("+DM Color"));
    pdiLineType = (PlotLine::LineType) dialog->getComboIndex(tr("+DM Line Type"));
    pdiLabel = dialog->getText(tr("+DM Label"));
    
    mdiColor = dialog->getColor(tr("-DM Color"));
    mdiLineType = (PlotLine::LineType) dialog->getComboIndex(tr("-DM Line Type"));
    mdiLabel = dialog->getText(tr("-DM Label"));
    
    adxColor = dialog->getColor(tr("ADX Color"));
    adxLineType = (PlotLine::LineType) dialog->getComboIndex(tr("ADX Line Type"));
    adxLabel = dialog->getText(tr("ADX Label"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void DMI::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["pdiColor"];
  if (s)
    pdiColor.setNamedColor(s->left(s->length()));
    
  s = dict["mdiColor"];
  if (s)
    mdiColor.setNamedColor(s->left(s->length()));
  
  s = dict["adxColor"];
  if (s)
    adxColor.setNamedColor(s->left(s->length()));
    
  s = dict["period"];
  if (s)
    period = (PlotLine::LineType) s->left(s->length()).toInt();
    
  s = dict["smoothing"];
  if (s)
    smoothing = (PlotLine::LineType) s->left(s->length()).toInt();
    
  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
    
  s = dict["alertType"];
  if (s)
    alertType = s->left(s->length());

  s = dict["pdiLabel"];
  if (s)
    pdiLabel = s->left(s->length());
    
  s = dict["mdiLabel"];
  if (s)
    mdiLabel = s->left(s->length());
    
  s = dict["adxLabel"];
  if (s)
    adxLabel = s->left(s->length());
        
  s = dict["pdiLineType"];
  if (s)
    pdiLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["mdiLineType"];
  if (s)
    mdiLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["adxLineType"];
  if (s)
    adxLineType = (PlotLine::LineType) s->left(s->length()).toInt();
}

void DMI::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);
  
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("alertType", new QString(alertType));
  dict.replace("pdiColor", new QString(pdiColor.name()));
  dict.replace("mdiColor", new QString(mdiColor.name()));
  dict.replace("adxColor", new QString(adxColor.name()));
  dict.replace("mdiLineType", new QString(QString::number(mdiLineType)));
  dict.replace("pdiLineType", new QString(QString::number(pdiLineType)));
  dict.replace("adxLineType", new QString(QString::number(adxLineType)));
  dict.replace("pdiLabel", new QString(pdiLabel));
  dict.replace("mdiLabel", new QString(mdiLabel));
  dict.replace("adxLabel", new QString(adxLabel));

  saveFile(file, dict);
}

Plugin * create ()
{
  DMI *o = new DMI;
  return ((Plugin *) o);
}

