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

#include "MA2.h"
#include "PrefDialog.h"
#include <qdict.h>

MA2::MA2 ()
{
  pluginName = "MA2";
  plotFlag = TRUE;
  setDefaults();
}

MA2::~MA2 ()
{
}

void MA2::setDefaults ()
{
  fastColor.setNamedColor("red");
  slowColor.setNamedColor("red");
  fastLineType = PlotLine::Line;
  slowLineType = PlotLine::Line;
  fastLabel = "MA2F";
  slowLabel = "MA2S";
  fastPeriod = 10;
  slowPeriod = 20;
  fastDisplace = 0;  
  slowDisplace = 0;  
  fastMaType = QSMath::SMA;  
  slowMaType = QSMath::SMA;  
  fastInput = BarData::Close;
  slowInput = BarData::Close;
}

void MA2::calculate ()
{
  QSMath *t = new QSMath();

  PlotLine *fin = data->getInput(fastInput);
  PlotLine *sin = data->getInput(slowInput);

  PlotLine *fma = t->getMA(fin, fastMaType, fastPeriod, fastDisplace);
  fma->setColor(fastColor);
  fma->setType(fastLineType);
  fma->setLabel(fastLabel);

  PlotLine *sma = t->getMA(sin, slowMaType, slowPeriod, slowDisplace);
  sma->setColor(slowColor);
  sma->setType(slowLineType);
  sma->setLabel(slowLabel);

  delete fin;
  delete sin;
  delete t;
  
  if (fma->getSize())
    output.append(fma);
  else
    delete fma;

  if (sma->getSize())
    output.append(sma);
  else
    delete sma;
}

int MA2::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MA2 Indicator"));
  dialog->createPage (tr("Fast MA"));
  dialog->addColorItem(tr("Fast Color"), tr("Fast MA"), fastColor);
  dialog->addComboItem(tr("Fast Line Type"), tr("Fast MA"), lineTypes, fastLineType);
  dialog->addTextItem(tr("Fast Label"), tr("Fast MA"), fastLabel);
  dialog->addIntItem(tr("Fast Period"), tr("Fast MA"), fastPeriod, 1, 99999999);
  dialog->addComboItem(tr("Fast MA Type"), tr("Fast MA"), maTypeList, fastMaType);
  dialog->addComboItem(tr("Fast Input"), tr("Fast MA"), inputTypeList, fastInput);
  dialog->addIntItem(tr("Fast Displace"), tr("Fast MA"), fastDisplace, 0, 99999999);
  
  dialog->createPage (tr("Slow MA"));
  dialog->addColorItem(tr("Slow Color"), tr("Slow MA"), slowColor);
  dialog->addComboItem(tr("Slow Line Type"), tr("Slow MA"), lineTypes, slowLineType);
  dialog->addTextItem(tr("Slow Label"), tr("Slow MA"), slowLabel);
  dialog->addIntItem(tr("Slow Period"), tr("Slow MA"), slowPeriod, 1, 99999999);
  dialog->addComboItem(tr("Slow MA Type"), tr("Slow MA"), maTypeList, slowMaType);
  dialog->addComboItem(tr("Slow Input"), tr("Slow MA"), inputTypeList, slowInput);
  dialog->addIntItem(tr("Slow Displace"), tr("Slow MA"), slowDisplace, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    fastColor = dialog->getColor(tr("Fast Color"));
    fastLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Fast Line Type"));
    fastPeriod = dialog->getInt(tr("Fast Period"));
    fastLabel = dialog->getText(tr("Fast Label"));
    fastMaType = (QSMath::MAType) dialog->getComboIndex(tr("Fast MA Type"));
    fastDisplace = dialog->getInt(tr("Fast Displace"));
    fastInput = (BarData::InputType) dialog->getComboIndex(tr("Fast Input"));
    slowColor = dialog->getColor(tr("Slow Color"));
    slowLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Slow Line Type"));
    slowPeriod = dialog->getInt(tr("Slow Period"));
    slowLabel = dialog->getText(tr("Slow Label"));
    slowMaType = (QSMath::MAType) dialog->getComboIndex(tr("Slow MA Type"));
    slowDisplace = dialog->getInt(tr("Slow Displace"));
    slowInput = (BarData::InputType) dialog->getComboIndex(tr("Slow Input"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MA2::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["fastColor"];
  if (s)
    fastColor.setNamedColor(s->left(s->length()));
    
  s = dict["fastLineType"];
  if (s)
    fastLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["fastPeriod"];
  if (s)
    fastPeriod = s->left(s->length()).toInt();

  s = dict["fastLabel"];
  if (s)
    fastLabel = s->left(s->length());
      
  s = dict["fastMaType"];
  if (s)
    fastMaType = (QSMath::MAType) s->left(s->length()).toInt();
    
  s = dict["fastInput"];
  if (s)
    fastInput = (BarData::InputType) s->left(s->length()).toInt();
    
  s = dict["fastDisplace"];
  if (s)
    fastDisplace = s->left(s->length()).toInt();

  s = dict["slowColor"];
  if (s)
    slowColor.setNamedColor(s->left(s->length()));
    
  s = dict["slowLineType"];
  if (s)
    slowLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["slowPeriod"];
  if (s)
    slowPeriod = s->left(s->length()).toInt();

  s = dict["slowLabel"];
  if (s)
    slowLabel = s->left(s->length());
      
  s = dict["slowMaType"];
  if (s)
    slowMaType = (QSMath::MAType) s->left(s->length()).toInt();
    
  s = dict["slowInput"];
  if (s)
    slowInput = (BarData::InputType) s->left(s->length()).toInt();
    
  s = dict["slowDisplace"];
  if (s)
    slowDisplace = s->left(s->length()).toInt();
}

void MA2::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("fastColor", new QString(fastColor.name()));
  dict.replace("fastLineType", new QString(QString::number(fastLineType)));
  dict.replace("fastPeriod", new QString(QString::number(fastPeriod)));
  dict.replace("fastLabel", new QString(fastLabel));
  dict.replace("fastMaType", new QString(QString::number(fastMaType)));
  dict.replace("fastInput", new QString(QString::number(fastInput)));
  dict.replace("fastDisplace", new QString(QString::number(fastDisplace)));
  dict.replace("slowColor", new QString(slowColor.name()));
  dict.replace("slowLineType", new QString(QString::number(slowLineType)));
  dict.replace("slowPeriod", new QString(QString::number(slowPeriod)));
  dict.replace("slowLabel", new QString(slowLabel));
  dict.replace("slowMaType", new QString(QString::number(slowMaType)));
  dict.replace("slowInput", new QString(QString::number(slowInput)));
  dict.replace("slowDisplace", new QString(QString::number(slowDisplace)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  MA2 *o = new MA2;
  return ((Plugin *) o);
}


