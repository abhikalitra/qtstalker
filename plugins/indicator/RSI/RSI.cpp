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

#include "RSI.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>

RSI::RSI ()
{
  pluginName = "RSI";
  plotFlag = FALSE;
  setDefaults();
}

RSI::~RSI ()
{
}

void RSI::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 14;
  smoothing = 10;  
  maType = IndicatorPlugin::SMA;  
  input = BarData::Close;
  buyLine = 30;
  sellLine = 70;
}

void RSI::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("RSI::calculate: no input");
    return;
  }

  PlotLine *rsi = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double loss = 0;
    double gain = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - in->getData(loop - loop2 - 1);
      if (t > 0)
        gain = gain + t;
      if (t < 0)
        loss = loss + fabs(t);
    }

    double again = gain / period;
    double aloss = loss / period;
    double rs = again / aloss;
    double t = 100 - (100 / (1 + rs));
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    rsi->append(t);
  }
  
  if (smoothing > 1)
  {
    PlotLine *ma = getMA(rsi, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output.append(ma);
    delete rsi;
  }
  else
  {
    rsi->setColor(color);
    rsi->setType(lineType);
    rsi->setLabel(label);
    output.append(rsi);
  }

  if (! customFlag)
    delete in;
}

int RSI::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("RSI Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("Parms"), maTypeList, maType);
  dialog->addIntItem(tr("Smoothing"), tr("Parms"), smoothing, 0, 99999999);
  if (customFlag)
    dialog->addFormulaInputItem(tr("Input"), tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(tr("Input"), tr("Parms"), inputTypeList, input);
  dialog->addFloatItem(tr("Buy Line"), tr("Parms"), buyLine, 0, 100);
  dialog->addFloatItem(tr("Sell Line"), tr("Parms"), sellLine, 0, 100);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    smoothing = dialog->getInt(tr("Smoothing"));
    if (customFlag)
      customInput = dialog->getFormulaInput(tr("Input"));
    else
      input = (BarData::InputType) dialog->getComboIndex(tr("Input"));
    buyLine = dialog->getFloat(tr("Buy Line"));
    sellLine = dialog->getFloat(tr("Sell Line"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void RSI::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["color"];
  if (s)
    color.setNamedColor(s->left(s->length()));
    
  s = dict["lineType"];
  if (s)
    lineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["period"];
  if (s)
    period = s->left(s->length()).toInt();

  s = dict["label"];
  if (s)
    label = s->left(s->length());
      
  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
    
  s = dict["input"];
  if (s)
    input = (BarData::InputType) s->left(s->length()).toInt();
    
  s = dict["smoothing"];
  if (s)
    smoothing = s->left(s->length()).toInt();

  s = dict["buyLine"];
  if (s)
    buyLine = s->left(s->length()).toFloat();

  s = dict["sellLine"];
  if (s)
    sellLine = s->left(s->length()).toFloat();
}

void RSI::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("label", new QString(label));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("input", new QString(QString::number(input)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));
  dict.replace("buyLine", new QString(QString::number(buyLine)));
  dict.replace("sellLine", new QString(QString::number(sellLine)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

PlotLine * RSI::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output.at(0);
}

QString RSI::getCustomSettings ()
{
  QString s("RSI");
  s.append("," + QString::number(maType));
  s.append("," + customInput);
  s.append("," + QString::number(period));
  s.append("," + QString::number(smoothing));
  s.append("," + color.name());
  s.append("," + QString::number(lineType));
  s.append("," + label);
  return s;
}

void RSI::setCustomSettings (QString d)
{
  customFlag = TRUE;
  QStringList l = QStringList::split(",", d, FALSE);
  maType = (IndicatorPlugin::MAType) l[1].toInt();
  customInput = l[2];
  period = l[3].toInt();
  smoothing = l[4].toInt();
  color.setNamedColor(l[5]);
  lineType = (PlotLine::LineType) l[6].toInt();
  label = l[7];
}

Plugin * create ()
{
  RSI *o = new RSI;
  return ((Plugin *) o);
}



