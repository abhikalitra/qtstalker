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

#include "ROC.h"
#include "PrefDialog.h"
#include <qdict.h>

ROC::ROC ()
{
  pluginName = "ROC";
  plotFlag = FALSE;
  setDefaults();
}

ROC::~ROC ()
{
}

void ROC::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  period = 14;
  smoothing = 10;
  maType = IndicatorPlugin::SMA;  
  input = BarData::Close;
}

void ROC::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("ROC::calculate: no input");
    return;
  }

  PlotLine *roc = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    roc->append(((in->getData(loop) / in->getData(loop - period)) * 100) - 100);

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(roc, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output.append(ma);
    delete roc;
  }
  else
  {
    roc->setColor(color);
    roc->setType(lineType);
    roc->setLabel(label);
    output.append(roc);
  }

  if (! customFlag)
    delete in;
}

int ROC::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("ROC Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("Parms"), maTypeList, maType);
  if (customFlag)
    dialog->addFormulaInputItem(tr("Input"), tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(tr("Input"), tr("Parms"), inputTypeList, input);
  dialog->addIntItem(tr("Smoothing"), tr("Parms"), smoothing, 0, 99999999);
  
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
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ROC::loadIndicatorSettings (QString file)
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
}

void ROC::saveIndicatorSettings (QString file)
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
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

PlotLine * ROC::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output.at(0);
}

QString ROC::getCustomSettings ()
{
  QString s("ROC");
  s.append("," + QString::number(maType));
  s.append("," + customInput);
  s.append("," + QString::number(period));
  s.append("," + QString::number(smoothing));
  s.append("," + color.name());
  s.append("," + QString::number(lineType));
  s.append("," + label);
  return s;
}

void ROC::setCustomSettings (QString d)
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
  ROC *o = new ROC;
  return ((Plugin *) o);
}




