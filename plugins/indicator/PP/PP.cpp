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
  plotFlag = TRUE;
  alertFlag = FALSE;
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
  QSMath *t = new QSMath();

  double high = data->getHigh(data->count() - 1);
  double low = data->getLow(data->count() - 1);
  double close = data->getClose(data->count() - 1);

  PlotLine *fr = new PlotLine();
  fr->setColor(resColor);
  fr->setType(resLineType);
  fr->setLabel(resLabel);
  fr->append(t->getPPFR(high, low, close));

  PlotLine *sr = new PlotLine();
  sr->setColor(resColor);
  sr->setType(resLineType);
  sr->setLabel(resLabel2);
  sr->append(t->getPPSR(high, low, close));

  PlotLine *thr = new PlotLine();
  thr->setColor(resColor);
  thr->setType(resLineType);
  thr->setLabel(resLabel3);
  thr->append(t->getPPTR(high, low, close));

  PlotLine *fs = new PlotLine();
  fs->setColor(supColor);
  fs->setType(supLineType);
  fs->setLabel(supLabel);
  fs->append(t->getPPFS(high, low, close));

  PlotLine *ss = new PlotLine();
  ss->setColor(supColor);
  ss->setType(supLineType);
  ss->setLabel(supLabel2);
  ss->append(t->getPPSS(high, low, close));

  PlotLine *ts = new PlotLine();
  ts->setColor(supColor);
  ts->setType(supLineType);
  ts->setLabel(supLabel3);
  ts->append(t->getPPTS(high, low, close));

  output.append(ts);
  output.append(ss);
  output.append(fs);
  output.append(fr);
  output.append(sr);
  output.append(thr);
  delete t;
}

int PP::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
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
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["resColor"];
  if (s)
    resColor.setNamedColor(s->left(s->length()));
    
  s = dict["supColor"];
  if (s)
    supColor.setNamedColor(s->left(s->length()));
  
  s = dict["resLineType"];
  if (s)
    resLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["supLineType"];
  if (s)
    supLineType = (PlotLine::LineType) s->left(s->length()).toInt();
  
  s = dict["resLabel"];
  if (s)
    resLabel = s->left(s->length());
      
  s = dict["resLabel2"];
  if (s)
    resLabel2 = s->left(s->length());
  
  s = dict["resLabel3"];
  if (s)
    resLabel3 = s->left(s->length());

  s = dict["supLabel"];
  if (s)
    supLabel = s->left(s->length());

  s = dict["supLabel2"];
  if (s)
    supLabel2 = s->left(s->length());

  s = dict["supLabel3"];
  if (s)
    supLabel3 = s->left(s->length());
}

void PP::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("resColor", new QString(resColor.name()));
  dict.replace("supColor", new QString(supColor.name()));
  dict.replace("resLineType", new QString(QString::number(resLineType)));
  dict.replace("supLineType", new QString(QString::number(supLineType)));
  dict.replace("resLabel", new QString(resLabel));
  dict.replace("resLabel2", new QString(resLabel2));
  dict.replace("resLabel3", new QString(resLabel3));
  dict.replace("supLabel", new QString(supLabel));
  dict.replace("supLabel2", new QString(supLabel2));
  dict.replace("supLabel3", new QString(supLabel3));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  PP *o = new PP;
  return ((Plugin *) o);
}


