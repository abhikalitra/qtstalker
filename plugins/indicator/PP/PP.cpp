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
#include <qobject.h>

PP::PP ()
{
  pluginName = "PP";
  helpFile = "pp.html";
  
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
  supLabel = QObject::tr("PP FS");
  supLabel2 = QObject::tr("PP SS");
  supLabel3 = QObject::tr("PP TS");
  resLabel = QObject::tr("PP FR");
  resLabel2 = QObject::tr("PP SR");
  resLabel3 = QObject::tr("PP TR");
  label = pluginName;
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

  output->addLine(ts);
  output->addLine(ss);
  output->addLine(fs);
  output->addLine(fr);
  output->addLine(sr);
  output->addLine(thr);
}

int PP::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("PP Indicator"));
  
  QString pl = QObject::tr("Support");
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  QString t = QObject::tr("Support Color");
  dialog->addColorItem(t, pl, supColor);
  t = QObject::tr("Support Line Type");
  dialog->addComboItem(t, pl, lineTypes, supLineType);
  t = QObject::tr("Label First Support");
  dialog->addTextItem(t, pl, supLabel);
  t = QObject::tr("Label Second Support");
  dialog->addTextItem(t, pl, supLabel2);
  t = QObject::tr("Label Third Support");
  dialog->addTextItem(t, pl, supLabel3);
  
  if (customFlag)
  {
    t = QObject::tr("Label");
    dialog->addTextItem(t, pl, label);
  }
  
  pl = QObject::tr("Resistance");
  dialog->createPage (pl);
  t = QObject::tr("Resistance Color");
  dialog->addColorItem(t, pl, resColor);
  t = QObject::tr("Resistance Line Type");
  dialog->addComboItem(t, pl, lineTypes, resLineType);
  t = QObject::tr("Label First Resistance");
  dialog->addTextItem(t, pl, resLabel);
  t = QObject::tr("Label Second Resistance");
  dialog->addTextItem(t, pl, resLabel2);
  t = QObject::tr("Label Third Resistance");
  dialog->addTextItem(t, pl, resLabel3);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    if (customFlag)
    {
      t = QObject::tr("Label");
      label = dialog->getText(t);
    }
  
    t = QObject::tr("Support Color");
    supColor = dialog->getColor(t);
    t = QObject::tr("Resistance Color");
    resColor = dialog->getColor(t);
    t = QObject::tr("Support Line Type");
    supLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("Resistance Line Type");
    resLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("Label First Support");
    supLabel = dialog->getText(t);
    t = QObject::tr("Label Second Support");
    supLabel2 = dialog->getText(t);
    t = QObject::tr("Label Third Support");
    supLabel3 = dialog->getText(t);
    t = QObject::tr("Label First Resistance");
    resLabel = dialog->getText(t);
    t = QObject::tr("Label Second Resistance");
    resLabel2 = dialog->getText(t);
    t = QObject::tr("Label Third Resistance");
    resLabel3 = dialog->getText(t);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void PP::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;
}

void PP::getIndicatorSettings (Setting &dict)
{
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
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
}

int PP::getMinBars ()
{
  int t = minBars + 2;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  PP *o = new PP;
  return ((IndicatorPlugin *) o);
}


