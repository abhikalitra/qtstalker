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
  
  methodList.append("AD");
  methodList.append("WAD");
  
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
  method = "AD";
}

void AD::calculate ()
{
  if (! method.compare("AD"))
    calculateAD();
  else
    calculateWAD();
}

void AD::calculateAD ()
{
  PlotLine *line = new PlotLine();
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

void AD::calculateWAD ()
{
  PlotLine *wad = new PlotLine();
  wad->setColor(color);
  wad->setType(lineType);
  wad->setLabel(label);

  int loop;
  double accum = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double close = data->getClose(loop);
    double yclose = data->getClose(loop - 1);

    double h = high;
    if (yclose > h)
      h = yclose;

    double l = low;
    if (yclose < l)
      l = yclose;

    if (close > yclose)
      accum = accum + (close - l);
    else
    {
      if (yclose == close)
        ;
      else
        accum = accum - (h - close);
    }

    wad->append(accum);
  }

  output.append(wad);
}

int AD::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("AD Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addComboItem(tr("Method"), tr("Parms"), methodList, method);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    method = dialog->getCombo(tr("Method"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void AD::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void AD::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

PlotLine * AD::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

Setting AD::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("method", method);
  dict.setData("plugin", pluginName);
  return dict;
}

void AD::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("label");
  if (s.length())
    label = s;
        
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("method");
  if (s.length())
    method = s;
}

Plugin * create ()
{
  AD *o = new AD;
  return ((Plugin *) o);
}



