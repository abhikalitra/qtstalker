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
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("DMI Indicator"));
  dialog->setHelpFile(helpFile);
  dialog->createPage (QObject::tr("DMI"));
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("DMI"), period, 1, 99999999);
  dialog->addIntItem(QObject::tr("Smoothing"), QObject::tr("DMI"), smoothing, 1, 99999999);
  dialog->addComboItem(QObject::tr("Smoothing Type"), QObject::tr("DMI"), getMATypes(), maType);
  if (customFlag)
  {
    dialog->addTextItem(QObject::tr("Label"), QObject::tr("DMI"), label);
    dialog->addComboItem(QObject::tr("Plot"), QObject::tr("DMI"), lineList, lineRequest);
  }
  
  dialog->createPage (QObject::tr("+DM"));
  dialog->addColorItem(QObject::tr("+DM Color"), QObject::tr("+DM"), pdiColor);
  dialog->addTextItem(QObject::tr("+DM Label"), QObject::tr("+DM"), pdiLabel);
  dialog->addComboItem(QObject::tr("+DM Line Type"), QObject::tr("+DM"), lineTypes, pdiLineType);
  
  dialog->createPage (QObject::tr("-DM"));
  dialog->addColorItem(QObject::tr("-DM Color"), QObject::tr("-DM"), mdiColor);
  dialog->addTextItem(QObject::tr("-DM Label"), QObject::tr("-DM"), mdiLabel);
  dialog->addComboItem(QObject::tr("-DM Line Type"), QObject::tr("-DM"), lineTypes, mdiLineType);
  
  dialog->createPage (QObject::tr("ADX"));
  dialog->addColorItem(QObject::tr("ADX Color"), QObject::tr("ADX"), adxColor);
  dialog->addTextItem(QObject::tr("ADX Label"), QObject::tr("ADX"), adxLabel);
  dialog->addComboItem(QObject::tr("ADX Line Type"), QObject::tr("ADX"), lineTypes, adxLineType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(QObject::tr("Period"));
    smoothing = dialog->getInt(QObject::tr("Smoothing"));
    maType = dialog->getComboIndex(QObject::tr("Smoothing Type"));
    if (customFlag)
    {
      label = dialog->getText(QObject::tr("Label"));
      lineRequest = dialog->getCombo(QObject::tr("Plot"));    
    }
      
    pdiColor = dialog->getColor(QObject::tr("+DM Color"));
    pdiLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("+DM Line Type"));
    pdiLabel = dialog->getText(QObject::tr("+DM Label"));
    
    mdiColor = dialog->getColor(QObject::tr("-DM Color"));
    mdiLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("-DM Line Type"));
    mdiLabel = dialog->getText(QObject::tr("-DM Label"));
    
    adxColor = dialog->getColor(QObject::tr("ADX Color"));
    adxLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("ADX Line Type"));
    adxLabel = dialog->getText(QObject::tr("ADX Label"));
    
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

