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

#include "MACD.h"
#include "PrefDialog.h"
#include <qdict.h>

MACD::MACD ()
{
  pluginName = "MACD";
  plotFlag = FALSE;
  setDefaults();
}

MACD::~MACD ()
{
}

void MACD::setDefaults ()
{
  macdColor.setNamedColor("red");
  trigColor.setNamedColor("yellow");
  oscColor.setNamedColor("blue");
  macdLineType = PlotLine::Line;
  trigLineType = PlotLine::Dash;
  oscLineType = PlotLine::Histogram;
  macdLabel = "MACD";
  trigLabel = "Trig";
  oscLabel = "Osc";
  fastPeriod = 12;
  slowPeriod = 26;
  trigPeriod = 9;
  macdMAType = IndicatorPlugin::EMA;  
  macdInput = BarData::Close;
  oscScaleFlag = FALSE;
}

void MACD::calculate ()
{
  PlotLine *d = 0;
  if (customFlag)
    d = getInputLine(customInput);
  else
    d = data->getInput(macdInput);
  if (! d)
  {
    qDebug("MACD::calculate: no input");
    return;
  }
  
  PlotLine *fma = getMA(d, macdMAType, fastPeriod);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(d, macdMAType, slowPeriod);
  int smaLoop = sma->getSize() - 1;
  
  if (! customFlag)
    delete d;

  PlotLine *macd = new PlotLine();
  macd->setColor(macdColor);
  macd->setType(macdLineType);
  macd->setLabel(macdLabel);
  
  while (fmaLoop > -1 && smaLoop > -1)
  {
    macd->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }
  delete fma;
  delete sma;
  
  PlotLine *signal = getMA(macd, macdMAType, trigPeriod);
  signal->setColor(trigColor);
  signal->setType(trigLineType);
  signal->setLabel(trigLabel);

  PlotLine *osc = new PlotLine();
  osc->setColor(oscColor);
  osc->setType(oscLineType);
  osc->setLabel(oscLabel);
  osc->setScaleFlag(oscScaleFlag);

  int floop = macd->getSize() - 1;
  int sloop = signal->getSize() - 1;

  while (floop > -1 && sloop > -1)
  {
    osc->prepend((macd->getData(floop) - signal->getData(sloop)));
    floop--;
    sloop--;
  }

  output.append(osc);
  output.append(macd);
  output.append(signal);
}

int MACD::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MACD Indicator"));
  
  dialog->createPage (tr("MACD"));
  dialog->addColorItem(tr("MACD Color"), tr("MACD"), macdColor);
  dialog->addIntItem(tr("Fast Period"), tr("MACD"), fastPeriod, 1, 99999999);
  dialog->addIntItem(tr("Slow Period"), tr("MACD"), slowPeriod, 1, 99999999);
  dialog->addTextItem(tr("MACD Label"), tr("MACD"), macdLabel);
  dialog->addComboItem(tr("MACD Line Type"), tr("MACD"), lineTypes, macdLineType);
  dialog->addComboItem(tr("MACD MA Type"), tr("MACD"), maTypeList, macdMAType);
  if (customFlag)
    dialog->addFormulaInputItem(tr("MACD Input"), tr("MACD"), FALSE, customInput);
  else
    dialog->addComboItem(tr("MACD Input"), tr("MACD"), inputTypeList, macdInput);
  
  dialog->createPage (tr("Trigger"));
  dialog->addColorItem(tr("Trigger Color"), tr("Trigger"), trigColor);
  dialog->addIntItem(tr("Trigger Period"), tr("Trigger"), trigPeriod, 1, 99999999);
  dialog->addTextItem(tr("Trigger Label"), tr("Trigger"), trigLabel);
  dialog->addComboItem(tr("Trigger Line Type"), tr("Trigger"), lineTypes, trigLineType);
  
  dialog->createPage (tr("Osc"));
  dialog->addColorItem(tr("Osc Color"), tr("Osc"), oscColor);
  dialog->addTextItem(tr("Osc Label"), tr("Osc"), oscLabel);
  dialog->addComboItem(tr("Osc Line Type"), tr("Osc"), lineTypes, oscLineType);
  dialog->addCheckItem(tr("Osc Scaling Max"), tr("Osc"), oscScaleFlag);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    macdColor = dialog->getColor(tr("MACD Color"));
    fastPeriod = dialog->getInt(tr("Fast Period"));
    slowPeriod = dialog->getInt(tr("Slow Period"));
    macdLabel = dialog->getText(tr("MACD Label"));
    macdLineType = (PlotLine::LineType) dialog->getComboIndex(tr("MACD Line Type"));
    macdMAType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MACD MA Type"));
    if (customFlag)
      customInput = dialog->getFormulaInput(tr("MACD Input"));
    else
      macdInput = (BarData::InputType) dialog->getComboIndex(tr("MACD Input"));
    
    trigColor = dialog->getColor(tr("Trigger Color"));
    trigPeriod = dialog->getInt(tr("Trigger Period"));
    trigLabel = dialog->getText(tr("Trigger Label"));
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Trigger Line Type"));
    
    oscColor = dialog->getColor(tr("Osc Color"));
    oscLabel = dialog->getText(tr("Osc Label"));
    oscLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Osc Line Type"));
    oscScaleFlag = dialog->getCheck(tr("Osc Scaling Max"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MACD::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["macdColor"];
  if (s)
    macdColor.setNamedColor(s->left(s->length()));
    
  s = dict["fastPeriod"];
  if (s)
    fastPeriod = s->left(s->length()).toInt();
	
  s = dict["slowPeriod"];
  if (s)
    slowPeriod = s->left(s->length()).toInt();
  
  s = dict["macdLabel"];
  if (s)
    macdLabel = s->left(s->length());
        
  s = dict["macdLineType"];
  if (s)
    macdLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["macdMAType"];
  if (s)
    macdMAType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
        
  s = dict["macdInput"];
  if (s)
    macdInput = (BarData::InputType) s->left(s->length()).toInt();

  s = dict["trigColor"];
  if (s)
    trigColor.setNamedColor(s->left(s->length()));
  
  s = dict["trigPeriod"];
  if (s)
    trigPeriod = s->left(s->length()).toInt();
  
  s = dict["trigLabel"];
  if (s)
    trigLabel = s->left(s->length());
        
  s = dict["trigLineType"];
  if (s)
    trigLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["oscColor"];
  if (s)
    oscColor.setNamedColor(s->left(s->length()));
  
  s = dict["oscLabel"];
  if (s)
    oscLabel = s->left(s->length());
        
  s = dict["oscLineType"];
  if (s)
    oscLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["oscScaleFlag"];
  if (s)
    oscScaleFlag = s->left(s->length()).toInt();
}

void MACD::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("macdColor", new QString(macdColor.name()));
  dict.replace("fastPeriod", new QString(QString::number(fastPeriod)));
  dict.replace("slowPeriod", new QString(QString::number(slowPeriod)));
  dict.replace("macdLabel", new QString(macdLabel));
  dict.replace("macdLineType", new QString(QString::number(macdLineType)));
  dict.replace("macdMAType", new QString(QString::number(macdMAType)));
  dict.replace("macdInput", new QString(QString::number(macdInput)));
  
  dict.replace("trigColor", new QString(trigColor.name()));
  dict.replace("trigPeriod", new QString(QString::number(trigPeriod)));
  dict.replace("trigLabel", new QString(trigLabel));
  dict.replace("trigLineType", new QString(QString::number(trigLineType)));
  
  dict.replace("oscColor", new QString(oscColor.name()));
  dict.replace("oscLabel", new QString(oscLabel));
  dict.replace("oscLineType", new QString(QString::number(oscLineType)));
  dict.replace("oscScaleFlag", new QString(QString::number(oscScaleFlag)));
  
  dict.replace("plugin", new QString(pluginName));
  
  saveFile(file, dict);
}

PlotLine * MACD::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output.at(1);
}

QString MACD::getCustomSettings ()
{
  QString s("MACD");
  s.append("," + QString::number(macdMAType));
  s.append("," + customInput);
  s.append("," + QString::number(fastPeriod));
  s.append("," + QString::number(slowPeriod));
  s.append("," + macdColor.name());
  s.append("," + QString::number(macdLineType));
  s.append("," + macdLabel);
  return s;
}

void MACD::setCustomSettings (QString d)
{
  customFlag = TRUE;
  QStringList l = QStringList::split(",", d, FALSE);
  macdMAType = (IndicatorPlugin::MAType) l[1].toInt();
  customInput = l[2];
  fastPeriod = l[3].toInt();
  slowPeriod = l[4].toInt();
  macdColor.setNamedColor(l[5]);
  macdLineType = (PlotLine::LineType) l[6].toInt();
  macdLabel = l[7];
}

Plugin * create ()
{
  MACD *o = new MACD;
  return ((Plugin *) o);
}

