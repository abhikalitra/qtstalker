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

#include "WILLR.h"
#include "PrefDialog.h"
#include <qdict.h>

WILLR::WILLR ()
{
  pluginName = "WILLR";
  setDefaults();
}

WILLR::~WILLR ()
{
}

void WILLR::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 10;
}

void WILLR::calculate ()
{
  PlotLine *willr = new PlotLine();
  willr->setColor(color);
  willr->setType(lineType);
  willr->setLabel(label);

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    int loop2;
    double lw;
    double hg;
    for (loop2 = 0, lw = 9999999, hg = 0; loop2 < period; loop2++)
    {
      double high = data->getHigh(loop - loop2);
      double low = data->getLow(loop - loop2);

      if (high > hg)
        hg = high;

      if (low < lw)
        lw = low;
    }

    double t = ((hg - data->getClose(loop)) / (hg - lw)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;
    t *= -1;

    willr->append(t);
  }

  output.append(willr);
}

int WILLR::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("WILLR Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void WILLR::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void WILLR::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void WILLR::setIndicatorSettings (Setting dict)
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

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
}

Setting WILLR::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * WILLR::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

Plugin * create ()
{
  WILLR *o = new WILLR;
  return ((Plugin *) o);
}




