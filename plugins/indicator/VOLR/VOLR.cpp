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

#include "VOLR.h"
#include "PrefDialog.h"
#include <qdict.h>

VOLR::VOLR ()
{
  pluginName = "VOLR";
  setDefaults();
}

VOLR::~VOLR ()
{
}

void VOLR::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 14;
}

void VOLR::calculate ()
{
  PlotLine *volr = new PlotLine();
  volr->setColor(color);
  volr->setType(lineType);
  volr->setLabel(label);

  PlotLine *trg = getTR();
  int trgLoop = trg->getSize() - 1;

  PlotLine *ma = getMA(trg, IndicatorPlugin::EMA, period);
  int maLoop = ma->getSize() - 1;

  while (maLoop > -1 && trgLoop > -1)
  {
    volr->prepend(trg->getData(trgLoop) / ma->getData(maLoop));
    maLoop--;
    trgLoop--;
  }

  delete trg;
  delete ma;

  output.append(volr);
}

int VOLR::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("VOLR Indicator"));
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

void VOLR::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void VOLR::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void VOLR::setIndicatorSettings (Setting dict)
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

Setting VOLR::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * VOLR::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

PlotLine * VOLR::getTR ()
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

Plugin * create ()
{
  VOLR *o = new VOLR;
  return ((Plugin *) o);
}

