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

#include "VOL.h"
#include "PrefDialog.h"
#include <qdict.h>

VOL::VOL ()
{
  pluginName = "VOL";
  plotFlag = FALSE;
  alertFlag = FALSE;
  setDefaults();
}

VOL::~VOL ()
{
}

void VOL::setDefaults ()
{
  upColor.setNamedColor("green");
  downColor.setNamedColor("red");
  maColor.setNamedColor("yellow");
  volLineType = PlotLine::HistogramBar;
  maLineType = PlotLine::Line;
  volLabel = "VOL";
  maLabel = "MAVol";
  period = 0;
  displace = 0;
  maType = QSMath::SMA;
}

void VOL::calculate ()
{
  PlotLine *pl = data->getInput(BarData::Volume);
  pl->setType(volLineType);
  pl->setLabel(volLabel);
  pl->setColorFlag(TRUE);

  // set the first bar color
  pl->appendColorBar(upColor);

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    if (data->getClose(loop) > data->getClose(loop - 1))
      pl->appendColorBar(upColor);
    else
      pl->appendColorBar(downColor);
  }

  output.append(pl);

  if (period < 1)
    return;

  QSMath *t = new QSMath();
  PlotLine *ma = t->getMA(pl, maType, period, displace);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output.append(ma);
  delete t;
}

int VOL::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("VOL Indicator"));
  
  dialog->createPage (tr("VOL"));
  dialog->addColorItem(tr("Up Color"), tr("VOL"), upColor);
  dialog->addColorItem(tr("Down Color"), tr("VOL"), downColor);
  dialog->addTextItem(tr("VOL Label"), tr("VOL"), volLabel);
  dialog->addComboItem(tr("VOL Line Type"), tr("VOL"), lineTypes, volLineType);
  
  dialog->createPage (tr("MA"));
  dialog->addColorItem(tr("MA Color"), tr("MA"), maColor);
  dialog->addIntItem(tr("MA Period"), tr("MA"), period, 0, 99999999);
  dialog->addTextItem(tr("MA Label"), tr("MA"), maLabel);
  dialog->addComboItem(tr("MA Line Type"), tr("MA"), lineTypes, maLineType);
  dialog->addComboItem(tr("MA Type"), tr("MA"), maTypeList, maType);
  dialog->addIntItem(tr("Displacement"), tr("MA"), displace, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    upColor = dialog->getColor(tr("Up Color"));
    downColor = dialog->getColor(tr("Down Color"));
    volLabel = dialog->getText(tr("VOL Label"));
    volLineType = (PlotLine::LineType) dialog->getComboIndex(tr("VOL Line Type"));
    
    maColor = dialog->getColor(tr("MA Color"));
    period = dialog->getInt(tr("MA Period"));
    maLabel = dialog->getText(tr("MA Label"));
    maLineType = (PlotLine::LineType) dialog->getComboIndex(tr("MA Line Type"));
    maType = (QSMath::MAType) dialog->getComboIndex(tr("MA Type"));
    displace = dialog->getInt(tr("Displacement"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VOL::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["upColor"];
  if (s)
    upColor.setNamedColor(s->left(s->length()));
    
  s = dict["downColor"];
  if (s)
    downColor.setNamedColor(s->left(s->length()));
    
  s = dict["volLabel"];
  if (s)
    volLabel = s->left(s->length());
        
  s = dict["volLineType"];
  if (s)
    volLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["maColor"];
  if (s)
    maColor.setNamedColor(s->left(s->length()));
        
  s = dict["maPeriod"];
  if (s)
    period = s->left(s->length()).toInt();
	
  s = dict["maLabel"];
  if (s)
    maLabel = s->left(s->length());
        
  s = dict["maLineType"];
  if (s)
    maLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["maType"];
  if (s)
    maType = (QSMath::MAType) s->left(s->length()).toInt();
        
  s = dict["maDisplace"];
  if (s)
    displace = s->left(s->length()).toInt();
}

void VOL::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("upColor", new QString(upColor.name()));
  dict.replace("downColor", new QString(downColor.name()));
  dict.replace("volLabel", new QString(volLabel));
  dict.replace("volLineType", new QString(QString::number(volLineType)));
  dict.replace("maColor", new QString(maColor.name()));
  dict.replace("maPeriod", new QString(QString::number(period)));
  dict.replace("maLabel", new QString(maLabel));
  dict.replace("maLineType", new QString(QString::number(maLineType)));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("maDisplace", new QString(QString::number(displace)));
  dict.replace("plugin", new QString(pluginName));
  
  saveFile(file, dict);
}

Plugin * create ()
{
  VOL *o = new VOL;
  return ((Plugin *) o);
}

