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

#include "PER.h"
#include "PrefDialog.h"
#include <qdict.h>

PER::PER ()
{
  pluginName = "PER";
  plotFlag = FALSE;
  alertFlag = FALSE;
  setDefaults();
}

PER::~PER ()
{
}

void PER::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  input = IndicatorPlugin::Close;
}

void PER::calculate ()
{
  PlotLine *in = getInput(input);

  PlotLine *per = new PlotLine();

  double base = in->getData(0);
  int loop;
  for (loop = 1; loop < (int) in->getSize(); loop++)
    per->append(((in->getData(loop) - base) / base) * 100);

  per->setColor(color);
  per->setType(lineType);
  per->setLabel(label);
  output.append(per);

  delete in;
}

int PER::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("PER Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addComboItem(tr("Input"), 1, getInputFields(), input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    input = (IndicatorPlugin::InputType) dialog->getComboIndex(tr("Input"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void PER::loadIndicatorSettings (QString file)
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

  s = dict["label"];
  if (s)
    label = s->left(s->length());
      
  s = dict["input"];
  if (s)
    input = (IndicatorPlugin::InputType) s->left(s->length()).toInt();
}

void PER::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("label", new QString(label));
  dict.replace("input", new QString(QString::number(input)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  PER *o = new PER;
  return ((Plugin *) o);
}






