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

#include "OI.h"
#include "PrefDialog.h"
#include <qdict.h>

OI::OI ()
{
  pluginName = "OI";
  plotFlag = FALSE;
  alertFlag = FALSE;
  setDefaults();
}

OI::~OI ()
{
}

void OI::setDefaults ()
{
  oiColor.setNamedColor("yellow");
  maColor.setNamedColor("red");
  oiLineType = PlotLine::Line;
  maLineType = PlotLine::Line;
  oiLabel = "OI";
  maLabel = "MAOI";
  period = 0;
  displace = 0;
  maType = IndicatorPlugin::SMA;
}

void OI::calculate ()
{
  PlotLine *pl = getInput(IndicatorPlugin::OpenInterest);
  pl->setColor(oiColor);
  pl->setType(oiLineType);
  pl->setLabel(oiLabel);
  output.append(pl);

  if (period < 1)
    return;

  PlotLine *ma = getMA(pl, maType, period, displace);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output.append(ma);
}

int OI::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("OI Indicator"));
  
  dialog->createPage (tr("OI"));
  dialog->addColorItem(tr("OI Color"), 1, oiColor);
  dialog->addTextItem(tr("OI Label"), 1, oiLabel);
  dialog->addComboItem(tr("OI Line Type"), 1, lineTypes, oiLineType);
  
  dialog->createPage (tr("MA"));
  dialog->addColorItem(tr("MA Color"), 2, maColor);
  dialog->addIntItem(tr("MA Period"), 2, period, 0, 99999999);
  dialog->addTextItem(tr("MA Label"), 2, maLabel);
  dialog->addComboItem(tr("MA Line Type"), 2, lineTypes, maLineType);
  dialog->addComboItem(tr("MA Type"), 2, getMATypes(), maType);
  dialog->addIntItem(tr("Displacement"), 2, displace, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    oiColor = dialog->getColor(tr("OI Color"));
    oiLabel = dialog->getText(tr("OI Label"));
    oiLineType = (PlotLine::LineType) dialog->getComboIndex(tr("OI Line Type"));
    
    maColor = dialog->getColor(tr("MA Color"));
    period = dialog->getInt(tr("MA Period"));
    maLabel = dialog->getText(tr("MA Label"));
    maLineType = (PlotLine::LineType) dialog->getComboIndex(tr("MA Line Type"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    displace = dialog->getInt(tr("Displacement"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void OI::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["oiColor"];
  if (s)
    oiColor.setNamedColor(s->left(s->length()));
    
  s = dict["oiLabel"];
  if (s)
    oiLabel = s->left(s->length());
        
  s = dict["oiLineType"];
  if (s)
    oiLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
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
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
        
  s = dict["maDisplace"];
  if (s)
    displace = s->left(s->length()).toInt();
}

void OI::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("oiColor", new QString(oiColor.name()));
  dict.replace("oiLabel", new QString(oiLabel));
  dict.replace("oiLineType", new QString(QString::number(oiLineType)));
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
  OI *o = new OI;
  return ((Plugin *) o);
}

