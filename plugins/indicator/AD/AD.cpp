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

#include "AD.h"
#include "PrefDialog.h"
#include <qdict.h>

AD::AD ()
{
  pluginName = "AD";
  plotFlag = FALSE;
  alertFlag = FALSE;
  setDefaults();
}

AD::~AD ()
{
}

void AD::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
}

void AD::calculate ()
{
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);

  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double volume = data->getVolume(loop);
    if (volume > 0)
    {
      double high = data->getHigh(loop);
      double low = data->getLow(loop);

      double t = high - low;

      if (t != 0)
      {
        double close = data->getClose(loop);
        double t2 = (close - low) - (high - close);
        accum = accum + ((t2 / t) * volume);
      }
    }

    line->append(accum);
  }

  output.append(line);
}

int AD::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("AD Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void AD::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["color"];
  if (s)
    color.setNamedColor(s->left(s->length()));
    
  s = dict["label"];
  if (s)
    label = s->left(s->length());
        
  s = dict["lineType"];
  if (s)
    lineType = (PlotLine::LineType) s->left(s->length()).toInt();
}

void AD::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("label", new QString(label));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}


Plugin * create ()
{
  AD *o = new AD;
  return ((Plugin *) o);
}



