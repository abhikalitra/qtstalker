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

#include "MA3.h"
#include "PrefDialog.h"
#include <qdict.h>

MA3::MA3 ()
{
  pluginName = "MA3";
  plotFlag = TRUE;
  alertFlag = TRUE;
  setDefaults();
}

MA3::~MA3 ()
{
}

void MA3::setDefaults ()
{
  fastColor.setNamedColor("red");
  midColor.setNamedColor("red");
  slowColor.setNamedColor("red");
  fastLineType = PlotLine::Line;
  midLineType = PlotLine::Line;
  slowLineType = PlotLine::Line;
  fastLabel = "MA3F";
  midLabel = "MA3M";
  slowLabel = "MA3S";
  slowPeriod = 40;
  midPeriod = 20;
  fastPeriod = 10;
  fastMAType = QSMath::SMA;  
  midMAType = QSMath::SMA;  
  slowMAType = QSMath::SMA;
  fastDisplace = 0;  
  midDisplace = 0;  
  slowDisplace = 0;  
  fastInput = BarData::Close;
  midInput = BarData::Close;
  slowInput = BarData::Close;
}

void MA3::calculate ()
{
  QSMath *t = new QSMath();

  PlotLine *fin = data->getInput(fastInput);
  PlotLine *min = data->getInput(midInput);
  PlotLine *sin = data->getInput(slowInput);

  PlotLine *fma = t->getMA(fin, fastMAType, fastPeriod, fastDisplace);
  fma->setColor(fastColor);
  fma->setType(fastLineType);
  fma->setLabel(fastLabel);

  PlotLine *mma = t->getMA(min, midMAType, midPeriod, midDisplace);
  mma->setColor(midColor);
  mma->setType(midLineType);
  mma->setLabel(midLabel);

  PlotLine *sma = t->getMA(sin, slowMAType, slowPeriod, slowDisplace);
  sma->setColor(slowColor);
  sma->setType(slowLineType);
  sma->setLabel(slowLabel);

  delete fin;
  delete min;
  delete sin;
  delete t;

  if (fma->getSize())
    output.append(fma);
  else
    delete fma;

  if (mma->getSize())
    output.append(mma);
  else
    delete mma;

  if (sma->getSize())
    output.append(sma);
  else
    delete sma;
}

QMemArray<int> MA3::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 3)
    return alerts;

  PlotLine *fma = output.at(0);
  PlotLine *mma = output.at(1);
  PlotLine *sma = output.at(2);

  int listLoop = data->count();
  int fmaLoop = fma->getSize();
  int mmaLoop = mma->getSize();
  int smaLoop = sma->getSize();
  while (listLoop != 0 && fmaLoop != 0 && mmaLoop != 0 &&  smaLoop != 0)
  {
    listLoop--;
    fmaLoop--;
    mmaLoop--;
    smaLoop--;
  }

  int status = 0;
  for (; listLoop < (int) data->count(); fmaLoop++, mmaLoop++, smaLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if (fma->getData(fmaLoop) > mma->getData(mmaLoop))
	  status = 0;
	break;
      case 1:
        if (fma->getData(fmaLoop) < mma->getData(mmaLoop))
	  status = 0;
	break;
      default:
        if ((fma->getData(fmaLoop) > mma->getData(mmaLoop)) && (mma->getData(mmaLoop) > sma->getData(smaLoop)))
	  status = 1;
	else
	{
          if ((fma->getData(fmaLoop) < mma->getData(mmaLoop)) && (mma->getData(mmaLoop) < sma->getData(smaLoop)))
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  return alerts;
}

int MA3::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MA3 Indicator"));
  
  dialog->createPage (tr("Fast MA"));
  dialog->addColorItem(tr("Fast Color"), tr("Fast MA"), fastColor);
  dialog->addIntItem(tr("Fast Period"), tr("Fast MA"), fastPeriod, 1, 99999999);
  dialog->addTextItem(tr("Fast Label"), tr("Fast MA"), fastLabel);
  dialog->addComboItem(tr("Fast Line Type"), tr("Fast MA"), lineTypes, fastLineType);
  dialog->addComboItem(tr("Fast MA Type"), tr("Fast MA"), maTypeList, fastMAType);
  dialog->addComboItem(tr("Fast Input"), tr("Fast MA"), inputTypeList, fastInput);
  
  dialog->createPage (tr("Mid MA"));
  dialog->addColorItem(tr("Mid Color"), tr("Mid MA"), midColor);
  dialog->addIntItem(tr("Mid Period"), tr("Mid MA"), midPeriod, 1, 99999999);
  dialog->addTextItem(tr("Mid Label"), tr("Mid MA"), midLabel);
  dialog->addComboItem(tr("Mid Line Type"), tr("Mid MA"), lineTypes, midLineType);
  dialog->addComboItem(tr("Mid MA Type"), tr("Mid MA"), maTypeList, midMAType);
  dialog->addComboItem(tr("Mid Input"), tr("Mid MA"), inputTypeList, midInput);
  
  dialog->createPage (tr("Slow MA"));
  dialog->addColorItem(tr("Slow Color"), tr("Slow MA"), slowColor);
  dialog->addIntItem(tr("Slow Period"), tr("Slow MA"), slowPeriod, 1, 99999999);
  dialog->addTextItem(tr("Slow Label"), tr("Slow MA"), slowLabel);
  dialog->addComboItem(tr("Slow Line Type"), tr("Slow MA"), lineTypes, slowLineType);
  dialog->addComboItem(tr("Slow MA Type"), tr("Slow MA"), maTypeList, slowMAType);
  dialog->addComboItem(tr("Slow Input"), tr("Slow MA"), inputTypeList, slowInput);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    fastColor = dialog->getColor(tr("Fast Color"));
    fastPeriod = dialog->getInt(tr("Fast Period"));
    fastLabel = dialog->getText(tr("Fast Label"));
    fastLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Fast Line Type"));
    fastMAType = (QSMath::MAType) dialog->getComboIndex(tr("Fast MA Type"));
    fastInput = (BarData::InputType) dialog->getComboIndex(tr("Fast Input"));
    
    midColor = dialog->getColor(tr("Mid Color"));
    midPeriod = dialog->getInt(tr("Mid Period"));
    midLabel = dialog->getText(tr("Mid Label"));
    midLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Mid Line Type"));
    midMAType = (QSMath::MAType) dialog->getComboIndex(tr("Mid MA Type"));
    midInput = (BarData::InputType) dialog->getComboIndex(tr("Mid Input"));
    
    slowColor = dialog->getColor(tr("Slow Color"));
    slowPeriod = dialog->getInt(tr("Slow Period"));
    slowLabel = dialog->getText(tr("Slow Label"));
    slowLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Slow Line Type"));
    slowMAType = (QSMath::MAType) dialog->getComboIndex(tr("Slow MA Type"));
    slowInput = (BarData::InputType) dialog->getComboIndex(tr("Slow Input"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MA3::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["fastColor"];
  if (s)
    fastColor.setNamedColor(s->left(s->length()));
    
  s = dict["fastPeriod"];
  if (s)
    fastPeriod = s->left(s->length()).toInt();
	
  s = dict["fastLabel"];
  if (s)
    fastLabel = s->left(s->length());
        
  s = dict["fastLineType"];
  if (s)
    fastLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["fastMAType"];
  if (s)
    fastMAType = (QSMath::MAType) s->left(s->length()).toInt();
        
  s = dict["fastInput"];
  if (s)
    fastInput = (BarData::InputType) s->left(s->length()).toInt();

  s = dict["midColor"];
  if (s)
    midColor.setNamedColor(s->left(s->length()));
    
  s = dict["midPeriod"];
  if (s)
    midPeriod = s->left(s->length()).toInt();
	
  s = dict["midLabel"];
  if (s)
    midLabel = s->left(s->length());
        
  s = dict["midLineType"];
  if (s)
    midLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["midMAType"];
  if (s)
    midMAType = (QSMath::MAType) s->left(s->length()).toInt();
        
  s = dict["midInput"];
  if (s)
    midInput = (BarData::InputType) s->left(s->length()).toInt();
    
  s = dict["slowColor"];
  if (s)
    slowColor.setNamedColor(s->left(s->length()));
    
  s = dict["slowPeriod"];
  if (s)
    slowPeriod = s->left(s->length()).toInt();
	
  s = dict["slowLabel"];
  if (s)
    slowLabel = s->left(s->length());
        
  s = dict["slowLineType"];
  if (s)
    slowLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["slowMAType"];
  if (s)
    slowMAType = (QSMath::MAType) s->left(s->length()).toInt();
        
  s = dict["slowInput"];
  if (s)
    slowInput = (BarData::InputType) s->left(s->length()).toInt();
}

void MA3::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("fastColor", new QString(fastColor.name()));
  dict.replace("fastPeriod", new QString(QString::number(fastPeriod)));
  dict.replace("fastLabel", new QString(fastLabel));
  dict.replace("fastLineType", new QString(QString::number(fastLineType)));
  dict.replace("fastMAType", new QString(QString::number(fastMAType)));
  dict.replace("fastInput", new QString(QString::number(fastInput)));

  dict.replace("midColor", new QString(midColor.name()));
  dict.replace("midPeriod", new QString(QString::number(midPeriod)));
  dict.replace("midLabel", new QString(midLabel));
  dict.replace("midLineType", new QString(QString::number(midLineType)));
  dict.replace("midMAType", new QString(QString::number(midMAType)));
  dict.replace("midInput", new QString(QString::number(midInput)));
  
  dict.replace("slowColor", new QString(slowColor.name()));
  dict.replace("slowPeriod", new QString(QString::number(slowPeriod)));
  dict.replace("slowLabel", new QString(slowLabel));
  dict.replace("slowLineType", new QString(QString::number(slowLineType)));
  dict.replace("slowMAType", new QString(QString::number(slowMAType)));
  dict.replace("slowInput", new QString(QString::number(slowInput)));

  dict.replace("plugin", new QString(pluginName));
  
  saveFile(file, dict);
}

Plugin * create ()
{
  MA3 *o = new MA3;
  return ((Plugin *) o);
}


