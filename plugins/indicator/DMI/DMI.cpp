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
#include <qobject.h>

DMI::DMI ()
{
  pluginName = "DMI";
  
  lineList.append("ADX");
  lineList.append("MDI");
  lineList.append("PDI");
  
  helpFile = "dmi.html";
  
  setDefaults();
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
  maType = 1;
  lineRequest = "ADX";
  label = pluginName;
}

void DMI::calculate ()
{
  getDI(period);
  getADX(maType, smoothing);
}

int DMI::indicatorPrefDialog (QWidget *w)
{
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("DMI Indicator"));
  dialog->setHelpFile(helpFile);
  
  QString pl = "DMI";
  dialog->createPage (pl);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addIntItem(sl, pl, smoothing, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  if (customFlag)
  {
    QString t = QObject::tr("Label");
    dialog->addTextItem(t, pl, label);
    t = QObject::tr("Plot");
    dialog->addComboItem(t, pl, lineList, lineRequest);
  }
  
  pl = "+DM";
  dialog->createPage (pl);
  QString t = QObject::tr("+DM Color");
  dialog->addColorItem(t, pl, pdiColor);
  t =  QObject::tr("+DM Label");
  dialog->addTextItem(t, pl, pdiLabel);
  t = QObject::tr("+DM Line Type");
  dialog->addComboItem(t, pl, lineTypes, pdiLineType);
  
  pl = "-DM";
  dialog->createPage (pl);
  t = QObject::tr("-DM Color");
  dialog->addColorItem(t, pl, mdiColor);
  t = QObject::tr("-DM Label");
  dialog->addTextItem(t, pl, mdiLabel);
  t = QObject::tr("-DM Line Type");
  dialog->addComboItem(t, pl, lineTypes, mdiLineType);
  
  pl = "ADX";
  dialog->createPage (pl);
  t = QObject::tr("ADX Color");
  dialog->addColorItem(t, pl, adxColor);
  t = QObject::tr("ADX Label");
  dialog->addTextItem(t, pl, adxLabel);
  t = QObject::tr("ADX Line Type");
  dialog->addComboItem(t, pl, lineTypes, adxLineType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(perl);
    smoothing = dialog->getInt(sl);
    maType = dialog->getComboIndex(stl);
    if (customFlag)
    {
      t = QObject::tr("Label");
      label = dialog->getText(t);
      t = QObject::tr("Plot");
      lineRequest = dialog->getCombo(t);    
    }
    
    t = QObject::tr("+DM Color");
    pdiColor = dialog->getColor(t);
    t = QObject::tr("+DM Line Type");
    pdiLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("+DM Label");
    pdiLabel = dialog->getText(t);
    
    t = QObject::tr("-DM Color");
    mdiColor = dialog->getColor(t);
    t = QObject::tr("-DM Line Type");
    mdiLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("-DM Label");
    mdiLabel = dialog->getText(t);
    
    t = QObject::tr("ADX Color");
    adxColor = dialog->getColor(t);
    t = QObject::tr("ADX Line Type");
    adxLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("ADX Label");
    adxLabel = dialog->getText(t);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void DMI::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("pdiColor");
  if (s.length())
    pdiColor.setNamedColor(s);
    
  s = dict.getData("mdiColor");
  if (s.length())
    mdiColor.setNamedColor(s);
  
  s = dict.getData("adxColor");
  if (s.length())
    adxColor.setNamedColor(s);
    
  s = dict.getData("period");
  if (s.length())
    period = (PlotLine::LineType) s.toInt();
    
  s = dict.getData("smoothing");
  if (s.length())
    smoothing = (PlotLine::LineType) s.toInt();
    
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
    
  s = dict.getData("pdiLabel");
  if (s.length())
    pdiLabel = s;
    
  s = dict.getData("mdiLabel");
  if (s.length())
    mdiLabel = s;
    
  s = dict.getData("adxLabel");
  if (s.length())
    adxLabel = s;
        
  s = dict.getData("pdiLineType");
  if (s.length())
    pdiLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("mdiLineType");
  if (s.length())
    mdiLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("adxLineType");
  if (s.length())
    adxLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;

  s = dict.getData("lineRequest");
  if (s.length())
    lineRequest = s;
}

void DMI::getIndicatorSettings (Setting &dict)
{
  dict.setData("period", QString::number(period));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("maType", QString::number(maType));
  dict.setData("pdiColor", pdiColor.name());
  dict.setData("mdiColor", mdiColor.name());
  dict.setData("adxColor", adxColor.name());
  dict.setData("mdiLineType", QString::number(mdiLineType));
  dict.setData("pdiLineType", QString::number(pdiLineType));
  dict.setData("adxLineType", QString::number(adxLineType));
  dict.setData("pdiLabel", pdiLabel);
  dict.setData("mdiLabel", mdiLabel);
  dict.setData("adxLabel", adxLabel);
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  dict.setData("lineRequest", lineRequest);
}

PlotLine * DMI::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  if (! lineRequest.compare("MDI"))
    return output->getLine(0);
  else
  {
    if (! lineRequest.compare("PDI"))
      return output->getLine(1);
    else
      return output->getLine(2);
  }
}

void DMI::getDI (int period)
{
  PlotLine *mdm = new PlotLine();
  PlotLine *pdm = new PlotLine();
  
  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double hdiff = data->getHigh(loop) - data->getHigh(loop - 1);
    double ldiff = data->getLow(loop - 1) - data->getLow(loop);
    double p = 0;
    double m = 0;
      
    if ((hdiff < 0 && ldiff < 0) || (hdiff == ldiff))
    {
      p = 0;
      m = 0;
    }
    else
    {
      if (hdiff > ldiff)
      {
        p = hdiff;
	m = 0;
      }
      else
      {
	if (hdiff < ldiff)
	{
	  p = 0;
	  m = ldiff;
	}
      }
    }
    
    mdm->append(m);
    pdm->append(p);
  }

  PlotLine *tr = getTR();

  PlotLine *smamdm = getMA(mdm, 1, period);
  int mdmLoop = smamdm->getSize() - 1;

  PlotLine *smapdm = getMA(pdm, 1, period);
  int pdmLoop = smapdm->getSize() - 1;
  
  PlotLine *smatr = getMA(tr, 1, period);
  int trLoop = smatr->getSize() - 1;

  PlotLine *mdi = new PlotLine();
  PlotLine *pdi = new PlotLine();

  while (mdmLoop > -1 && trLoop > -1)
  {
    int m = (int) ((smamdm->getData(mdmLoop) / smatr->getData(trLoop)) * 100);
    int p = (int) ((smapdm->getData(pdmLoop) / smatr->getData(trLoop)) * 100);
    
    if (m > 100)
      m = 100;
    if (m < 0)
      m = 0;

    if (p > 100)
      p = 100;
    if (p < 0)
      p = 0;
      
    mdi->prepend(m);
    pdi->prepend(p);

    mdmLoop--;
    pdmLoop--;
    trLoop--;
  }

  delete mdm;
  delete pdm;
  delete tr;
  delete smamdm;
  delete smapdm;
  delete smatr;
  
  mdi->setColor(mdiColor);
  mdi->setType(mdiLineType);
  mdi->setLabel(mdiLabel);
  output->addLine(mdi);
  
  pdi->setColor(pdiColor);
  pdi->setType(pdiLineType);
  pdi->setLabel(pdiLabel);
  output->addLine(pdi);
}

void DMI::getADX (int type, int period)
{
  PlotLine *mdi = output->getLine(0);
  if (! mdi)
    return;
    
  PlotLine *pdi = output->getLine(1);
  if (! pdi)
    return;
  
  int mdiLoop = mdi->getSize() - 1;
  int pdiLoop = pdi->getSize() - 1;

  PlotLine *dx = new PlotLine;

  while (pdiLoop > -1 && mdiLoop > -1)
  {
    double m = fabs(pdi->getData(pdiLoop) - mdi->getData(mdiLoop));
    double p = pdi->getData(pdiLoop) + mdi->getData(mdiLoop);
    int t = (int) ((m / p) * 100);
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    dx->prepend(t);
	       
    pdiLoop--;
    mdiLoop--;
  }

  PlotLine *adx = getMA(dx, type, period);
  adx->setColor(adxColor);
  adx->setType(adxLineType);
  adx->setLabel(adxLabel);
  output->addLine(adx);
  
  delete dx;
}

PlotLine * DMI::getTR ()
{
  PlotLine *tr = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double close;
    if (loop > 0)
      close = data->getClose(loop - 1);
    else
      close = high;

    double t = high - low;

    double t2 = fabs(high - close);
    if (t2 > t)
      t = t2;

    t2 = fabs(low - close);
    if (t2 > t)
      t = t2;

    tr->append(t);
  }
  
  return tr;
}

int DMI::getMinBars ()
{
  int t = minBars + 1 + period + smoothing;
  return t;
}

//*********************************************************
//*********************************************************
//*********************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  DMI *o = new DMI;
  return ((IndicatorPlugin *) o);
}

