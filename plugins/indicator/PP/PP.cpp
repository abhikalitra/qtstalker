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

#include "PP.h"
#include "PrefDialog.h"
#include <qdict.h>

PP::PP ()
{
  pluginName = "PP";
  setDefaults();
}

PP::~PP ()
{
}

void PP::setDefaults ()
{
  resColor.setNamedColor("red");
  supColor.setNamedColor("yellow");
  resLineType = PlotLine::Horizontal;
  supLineType = PlotLine::Horizontal;
  supLabel = tr("PP FS");
  supLabel2 = tr("PP SS");
  supLabel3 = tr("PP TS");
  resLabel = tr("PP FR");
  resLabel2 = tr("PP SR");
  resLabel3 = tr("PP TR");
}

void PP::calculate ()
{
  double high = data->getHigh(data->count() - 1);
  double low = data->getLow(data->count() - 1);
  double close = data->getClose(data->count() - 1);
  
  PlotLine *fr = new PlotLine();
  fr->setColor(resColor);
  fr->setType(resLineType);
  fr->setLabel(resLabel);
  double pp = (high + low + close) / 3;
  double t = (2 * pp) - low;
  fr->append(t);

  PlotLine *sr = new PlotLine();
  sr->setColor(resColor);
  sr->setType(resLineType);
  sr->setLabel(resLabel2);
  pp = (high + low + close) / 3;
  t = pp + (high - low);
  sr->append(t);

  PlotLine *thr = new PlotLine();
  thr->setColor(resColor);
  thr->setType(resLineType);
  thr->setLabel(resLabel3);
  pp = (high + low + close) / 3;
  t = (2 * pp) + (high - (2 * low));
  thr->append(t);

  PlotLine *fs = new PlotLine();
  fs->setColor(supColor);
  fs->setType(supLineType);
  fs->setLabel(supLabel);
  pp = (high + low + close) / 3;
  t = (2 * pp) - high;
  fs->append(t);

  PlotLine *ss = new PlotLine();
  ss->setColor(supColor);
  ss->setType(supLineType);
  ss->setLabel(supLabel2);
  pp = (high + low + close) / 3;
  t = pp - (high - low);
  ss->append(t);

  PlotLine *ts = new PlotLine();
  ts->setColor(supColor);
  ts->setType(supLineType);
  ts->setLabel(supLabel3);
  pp = (high + low + close) / 3;
  t = (2 * pp) - ((2 * high) - low);
  ts->append(t);

  output.append(ts);
  output.append(ss);
  output.append(fs);
  output.append(fr);
  output.append(sr);
  output.append(thr);
}

int PP::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("PP Indicator"));
  dialog->createPage (tr("Support"));
  dialog->addColorItem(tr("Support Color"), tr("Support"), supColor);
  dialog->addComboItem(tr("Support Line Type"), tr("Support"), lineTypes, supLineType);
  dialog->addTextItem(tr("Label First Support"), tr("Support"), supLabel);
  dialog->addTextItem(tr("Label Second Support"), tr("Support"), supLabel2);
  dialog->addTextItem(tr("Label Third Support"), tr("Support"), supLabel3);
  
  dialog->createPage (tr("Resistance"));
  dialog->addColorItem(tr("Resistance Color"), tr("Resistance"), resColor);
  dialog->addComboItem(tr("Resistance Line Type"), tr("Resistance"), lineTypes, resLineType);
  dialog->addTextItem(tr("Label First Resistance"), tr("Resistance"), resLabel);
  dialog->addTextItem(tr("Label Second Resistance"), tr("Resistance"), resLabel2);
  dialog->addTextItem(tr("Label Third Resistance"), tr("Resistance"), resLabel3);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    supColor = dialog->getColor(tr("Support Color"));
    resColor = dialog->getColor(tr("Resistance Color"));
    supLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Support Line Type"));
    resLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Resistance Line Type"));
    supLabel = dialog->getText(tr("Label First Support"));
    supLabel2 = dialog->getText(tr("Label Second Support"));
    supLabel3 = dialog->getText(tr("Label Third Support"));
    resLabel = dialog->getText(tr("Label First Resistance"));
    resLabel2 = dialog->getText(tr("Label Second Resistance"));
    resLabel3 = dialog->getText(tr("Label Third Resistance"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void PP::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void PP::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void PP::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("resColor");
  if (s.length())
    resColor.setNamedColor(s);
    
  s = dict.getData("supColor");
  if (s.length())
    supColor.setNamedColor(s);
  
  s = dict.getData("resLineType");
  if (s.length())
    resLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("supLineType");
  if (s.length())
    supLineType = (PlotLine::LineType) s.toInt();
  
  s = dict.getData("resLabel");
  if (s.length())
    resLabel = s;
      
  s = dict.getData("resLabel2");
  if (s.length())
    resLabel2 = s;
  
  s = dict.getData("resLabel3");
  if (s.length())
    resLabel3 = s;

  s = dict.getData("supLabel");
  if (s.length())
    supLabel = s;

  s = dict.getData("supLabel2");
  if (s.length())
    supLabel2 = s;

  s = dict.getData("supLabel3");
  if (s.length())
    supLabel3 = s;
}

Setting PP::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("resColor", resColor.name());
  dict.setData("supColor", supColor.name());
  dict.setData("resLineType", QString::number(resLineType));
  dict.setData("supLineType", QString::number(supLineType));
  dict.setData("resLabel", resLabel);
  dict.setData("resLabel2", resLabel2);
  dict.setData("resLabel3", resLabel3);
  dict.setData("supLabel", supLabel);
  dict.setData("supLabel2", supLabel2);
  dict.setData("supLabel3", supLabel3);
  dict.setData("plugin", pluginName);
  return dict;
}

Plugin * create ()
{
  PP *o = new PP;
  return ((Plugin *) o);
}


