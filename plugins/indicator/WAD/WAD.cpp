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

#include "WAD.h"
#include "PrefDialog.h"
#include <qdict.h>

WAD::WAD ()
{
  pluginName = "WAD";
  plotFlag = FALSE;
  setDefaults();
}

WAD::~WAD ()
{
}

void WAD::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
}

void WAD::calculate ()
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

int WAD::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("WAD Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  
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

void WAD::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void WAD::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void WAD::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
}

Setting WAD::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * WAD::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

Plugin * create ()
{
  WAD *o = new WAD;
  return ((Plugin *) o);
}



