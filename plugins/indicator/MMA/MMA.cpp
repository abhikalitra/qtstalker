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

#include "MMA.h"
#include "PrefDialog.h"
#include <qdict.h>

MMA::MMA ()
{
  pluginName = "MMA";
  plotFlag = FALSE;
  alertFlag = FALSE;
  setDefaults();
}

MMA::~MMA ()
{
}

void MMA::setDefaults ()
{
  fastColor.setNamedColor("red");
  fastLineType = PlotLine::Line;
  fastLabel = "MMAF1";
  fastLabel2 = "MMAF2";
  fastLabel3 = "MMAF3";
  fastLabel4 = "MMAF4";
  fastLabel5 = "MMAF5";
  fastLabel6 = "MMAF6";
  fastPeriod = 3;
  fastPeriod2 = 6;
  fastPeriod3 = 8;
  fastPeriod4 = 10;
  fastPeriod5 = 12;
  fastPeriod6 = 15;
  fastDisplace = 0;  
  fastDisplace2 = 0;  
  fastDisplace3 = 0;  
  fastDisplace4 = 0;  
  fastDisplace5 = 0;  
  fastDisplace6 = 0;  
  fastMaType = QSMath::EMA;  
  fastInput = BarData::Close;

  slowColor.setNamedColor("yellow");
  slowLineType = PlotLine::Line;
  slowLabel = "MMAS1";
  slowLabel2 = "MMAS2";
  slowLabel3 = "MMAS3";
  slowLabel4 = "MMAS4";
  slowLabel5 = "MMAS5";
  slowLabel6 = "MMAS6";
  slowPeriod = 30;
  slowPeriod2 = 35;
  slowPeriod3 = 40;
  slowPeriod4 = 45;
  slowPeriod5 = 50;
  slowPeriod6 = 60;
  slowDisplace = 0;  
  slowDisplace2 = 0;  
  slowDisplace3 = 0;  
  slowDisplace4 = 0;  
  slowDisplace5 = 0;  
  slowDisplace6 = 0;  
  slowMaType = QSMath::EMA;  
  slowInput = BarData::Close;
  
  longColor.setNamedColor("blue");
  longLineType = PlotLine::Line;
  longLabel = "MMAL";
  longPeriod = 150;
  longDisplace = 0;  
  longMaType = QSMath::SMA;  
  longInput = BarData::Close;
}

void MMA::calculate ()
{
  QSMath *t = new QSMath();

  PlotLine *fin = data->getInput(fastInput);
  PlotLine *sin = data->getInput(slowInput);
  PlotLine *lin = data->getInput(longInput);

  PlotLine *fma1 = t->getMA(fin, fastMaType, fastPeriod, fastDisplace);
  fma1->setColor(fastColor);
  fma1->setType(fastLineType);
  fma1->setLabel(fastLabel);
  
  PlotLine *fma2 = t->getMA(fin, fastMaType, fastPeriod2, fastDisplace2);
  fma2->setColor(fastColor);
  fma2->setType(fastLineType);
  fma2->setLabel(fastLabel2);
  
  PlotLine *fma3 = t->getMA(fin, fastMaType, fastPeriod3, fastDisplace3);
  fma3->setColor(fastColor);
  fma3->setType(fastLineType);
  fma3->setLabel(fastLabel3);
  
  PlotLine *fma4 = t->getMA(fin, fastMaType, fastPeriod4, fastDisplace4);
  fma4->setColor(fastColor);
  fma4->setType(fastLineType);
  fma4->setLabel(fastLabel4);
  
  PlotLine *fma5 = t->getMA(fin, fastMaType, fastPeriod5, fastDisplace5);
  fma5->setColor(fastColor);
  fma5->setType(fastLineType);
  fma5->setLabel(fastLabel5);
  
  PlotLine *fma6 = t->getMA(fin, fastMaType, fastPeriod6, fastDisplace6);
  fma6->setColor(fastColor);
  fma6->setType(fastLineType);
  fma6->setLabel(fastLabel6);

  PlotLine *sma1 = t->getMA(sin, slowMaType, slowPeriod, slowDisplace);
  sma1->setColor(slowColor);
  sma1->setType(slowLineType);
  sma1->setLabel(slowLabel);
  
  PlotLine *sma2 = t->getMA(sin, slowMaType, slowPeriod2, slowDisplace2);
  sma2->setColor(slowColor);
  sma2->setType(slowLineType);
  sma2->setLabel(slowLabel2);
  
  PlotLine *sma3 = t->getMA(sin, slowMaType, slowPeriod3, slowDisplace3);
  sma3->setColor(slowColor);
  sma3->setType(slowLineType);
  sma3->setLabel(slowLabel3);
  
  PlotLine *sma4 = t->getMA(sin, slowMaType, slowPeriod4, slowDisplace4);
  sma4->setColor(slowColor);
  sma4->setType(slowLineType);
  sma4->setLabel(slowLabel4);
  
  PlotLine *sma5 = t->getMA(sin, slowMaType, slowPeriod5, slowDisplace5);
  sma5->setColor(slowColor);
  sma5->setType(slowLineType);
  sma5->setLabel(slowLabel5);
  
  PlotLine *sma6 = t->getMA(sin, slowMaType, slowPeriod6, slowDisplace6);
  sma6->setColor(slowColor);
  sma6->setType(slowLineType);
  sma6->setLabel(slowLabel6);

  PlotLine *lma = t->getMA(lin, longMaType, longPeriod, longDisplace);
  lma->setColor(longColor);
  lma->setType(longLineType);
  lma->setLabel(longLabel);

  delete fin;
  delete sin;
  delete lin;
  delete t;

  if (fma1->getSize())
    output.append(fma1);
  else
    delete fma1;

  if (fma2->getSize())
    output.append(fma2);
  else
    delete fma2;

  if (fma3->getSize())
    output.append(fma3);
  else
    delete fma3;

  if (fma4->getSize())
    output.append(fma4);
  else
    delete fma4;

  if (fma5->getSize())
    output.append(fma5);
  else
    delete fma5;

  if (fma6->getSize())
    output.append(fma6);
  else
    delete fma6;

  if (sma1->getSize())
    output.append(sma1);
  else
    delete sma1;

  if (sma2->getSize())
    output.append(sma2);
  else
    delete sma2;

  if (sma3->getSize())
    output.append(sma3);
  else
    delete sma3;

  if (sma4->getSize())
    output.append(sma4);
  else
    delete sma4;

  if (sma5->getSize())
    output.append(sma5);
  else
    delete sma5;

  if (sma6->getSize())
    output.append(sma6);
  else
    delete sma6;

  if (lma->getSize())
    output.append(lma);
  else
    delete lma;
}

int MMA::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MMA Indicator"));
  dialog->createPage (tr("Fast MA"));
  dialog->addColorItem(tr("Fast Color"), 1, fastColor);
  dialog->addComboItem(tr("Fast Line Type"), 1, lineTypes, fastLineType);
  dialog->addTextItem(tr("Fast Label 1"), 1, fastLabel);
  dialog->addTextItem(tr("Fast Label 2"), 1, fastLabel2);
  dialog->addTextItem(tr("Fast Label 3"), 1, fastLabel3);
  dialog->addTextItem(tr("Fast Label 4"), 1, fastLabel4);
  dialog->addTextItem(tr("Fast Label 5"), 1, fastLabel5);
  dialog->addTextItem(tr("Fast Label 6"), 1, fastLabel6);
  dialog->addIntItem(tr("Fast Period 1"), 1, fastPeriod, 1, 99999999);
  dialog->addIntItem(tr("Fast Period 2"), 1, fastPeriod2, 1, 99999999);
  dialog->addIntItem(tr("Fast Period 3"), 1, fastPeriod3, 1, 99999999);
  dialog->addIntItem(tr("Fast Period 4"), 1, fastPeriod4, 1, 99999999);
  dialog->addIntItem(tr("Fast Period 5"), 1, fastPeriod5, 1, 99999999);
  dialog->addIntItem(tr("Fast Period 6"), 1, fastPeriod6, 1, 99999999);
  dialog->addIntItem(tr("Fast Displace 1"), 1, fastDisplace, 0, 99999999);
  dialog->addIntItem(tr("Fast Displace 2"), 1, fastDisplace2, 0, 99999999);
  dialog->addIntItem(tr("Fast Displace 3"), 1, fastDisplace3, 0, 99999999);
  dialog->addIntItem(tr("Fast Displace 4"), 1, fastDisplace4, 0, 99999999);
  dialog->addIntItem(tr("Fast Displace 5"), 1, fastDisplace5, 0, 99999999);
  dialog->addIntItem(tr("Fast Displace 6"), 1, fastDisplace6, 0, 99999999);
  dialog->addComboItem(tr("Fast MA Type"), 1, maTypeList, fastMaType);
  dialog->addComboItem(tr("Fast Input"), 1, inputTypeList, fastInput);
  
  dialog->createPage (tr("Slow MA"));
  dialog->addColorItem(tr("Slow Color"), 2, slowColor);
  dialog->addComboItem(tr("Slow Line Type"), 2, lineTypes, slowLineType);
  dialog->addTextItem(tr("Slow Label 1"), 2, slowLabel);
  dialog->addTextItem(tr("Slow Label 2"), 2, slowLabel2);
  dialog->addTextItem(tr("Slow Label 3"), 2, slowLabel3);
  dialog->addTextItem(tr("Slow Label 4"), 2, slowLabel4);
  dialog->addTextItem(tr("Slow Label 5"), 2, slowLabel5);
  dialog->addTextItem(tr("Slow Label 6"), 2, slowLabel6);
  dialog->addIntItem(tr("Slow Period 1"), 2, slowPeriod, 1, 99999999);
  dialog->addIntItem(tr("Slow Period 2"), 2, slowPeriod2, 1, 99999999);
  dialog->addIntItem(tr("Slow Period 3"), 2, slowPeriod3, 1, 99999999);
  dialog->addIntItem(tr("Slow Period 4"), 2, slowPeriod4, 1, 99999999);
  dialog->addIntItem(tr("Slow Period 5"), 2, slowPeriod5, 1, 99999999);
  dialog->addIntItem(tr("Slow Period 6"), 2, slowPeriod6, 1, 99999999);
  dialog->addIntItem(tr("Slow Displace 1"), 2, slowDisplace, 0, 99999999);
  dialog->addIntItem(tr("Slow Displace 2"), 2, slowDisplace2, 0, 99999999);
  dialog->addIntItem(tr("Slow Displace 3"), 2, slowDisplace3, 0, 99999999);
  dialog->addIntItem(tr("Slow Displace 4"), 2, slowDisplace4, 0, 99999999);
  dialog->addIntItem(tr("Slow Displace 5"), 2, slowDisplace5, 0, 99999999);
  dialog->addIntItem(tr("Slow Displace 6"), 2, slowDisplace6, 0, 99999999);
  dialog->addComboItem(tr("Slow MA Type"), 2, maTypeList, slowMaType);
  dialog->addComboItem(tr("Slow Input"), 2, inputTypeList, slowInput);
  
  dialog->createPage (tr("Long MA"));
  dialog->addColorItem(tr("Long Color"), 3, longColor);
  dialog->addComboItem(tr("Long Line Type"), 3, lineTypes, longLineType);
  dialog->addTextItem(tr("Long Label"), 3, longLabel);
  dialog->addIntItem(tr("Long Period"), 3, longPeriod, 1, 99999999);
  dialog->addIntItem(tr("Long Displace"), 3, longDisplace, 0, 99999999);
  dialog->addComboItem(tr("Long MA Type"), 3, maTypeList, longMaType);
  dialog->addComboItem(tr("Long Input"), 3, inputTypeList, longInput);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    fastColor = dialog->getColor(tr("Fast Color"));
    fastLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Fast Line Type"));
    fastPeriod = dialog->getInt(tr("Fast Period 1"));
    fastPeriod2 = dialog->getInt(tr("Fast Period 2"));
    fastPeriod3 = dialog->getInt(tr("Fast Period 3"));
    fastPeriod4 = dialog->getInt(tr("Fast Period 4"));
    fastPeriod5 = dialog->getInt(tr("Fast Period 5"));
    fastPeriod6 = dialog->getInt(tr("Fast Period 6"));
    fastLabel = dialog->getText(tr("Fast Label 1"));
    fastLabel2 = dialog->getText(tr("Fast Label 2"));
    fastLabel3 = dialog->getText(tr("Fast Label 3"));
    fastLabel4 = dialog->getText(tr("Fast Label 4"));
    fastLabel5 = dialog->getText(tr("Fast Label 5"));
    fastLabel6 = dialog->getText(tr("Fast Label 6"));
    fastDisplace = dialog->getInt(tr("Fast Displace 1"));
    fastDisplace2 = dialog->getInt(tr("Fast Displace 2"));
    fastDisplace3 = dialog->getInt(tr("Fast Displace 3"));
    fastDisplace4 = dialog->getInt(tr("Fast Displace 4"));
    fastDisplace5 = dialog->getInt(tr("Fast Displace 5"));
    fastDisplace6 = dialog->getInt(tr("Fast Displace 6"));
    fastMaType = (QSMath::MAType) dialog->getComboIndex(tr("Fast MA Type"));
    fastInput = (BarData::InputType) dialog->getComboIndex(tr("Fast Input"));
    
    slowColor = dialog->getColor(tr("Slow Color"));
    slowLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Slow Line Type"));
    slowPeriod = dialog->getInt(tr("Slow Period 1"));
    slowPeriod2 = dialog->getInt(tr("Slow Period 2"));
    slowPeriod3 = dialog->getInt(tr("Slow Period 3"));
    slowPeriod4 = dialog->getInt(tr("Slow Period 4"));
    slowPeriod5 = dialog->getInt(tr("Slow Period 5"));
    slowPeriod6 = dialog->getInt(tr("Slow Period 6"));
    slowLabel = dialog->getText(tr("Slow Label 1"));
    slowLabel2 = dialog->getText(tr("Slow Label 2"));
    slowLabel3 = dialog->getText(tr("Slow Label 3"));
    slowLabel4 = dialog->getText(tr("Slow Label 4"));
    slowLabel5 = dialog->getText(tr("Slow Label 5"));
    slowLabel6 = dialog->getText(tr("Slow Label 6"));
    slowDisplace = dialog->getInt(tr("Slow Displace 1"));
    slowDisplace2 = dialog->getInt(tr("Slow Displace 2"));
    slowDisplace3 = dialog->getInt(tr("Slow Displace 3"));
    slowDisplace4 = dialog->getInt(tr("Slow Displace 4"));
    slowDisplace5 = dialog->getInt(tr("Slow Displace 5"));
    slowDisplace6 = dialog->getInt(tr("Slow Displace 6"));
    slowMaType = (QSMath::MAType) dialog->getComboIndex(tr("Slow MA Type"));
    slowInput = (BarData::InputType) dialog->getComboIndex(tr("Slow Input"));

    longColor = dialog->getColor(tr("Long Color"));
    longLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Long Line Type"));
    longPeriod = dialog->getInt(tr("Long Period"));
    longLabel = dialog->getText(tr("Long Label"));
    longDisplace = dialog->getInt(tr("Long Displace"));
    longMaType = (QSMath::MAType) dialog->getComboIndex(tr("Long MA Type"));
    longInput = (BarData::InputType) dialog->getComboIndex(tr("Long Input"));
        
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MMA::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  // fast
  
  QString *s = dict["fastColor"];
  if (s)
    fastColor.setNamedColor(s->left(s->length()));
    
  s = dict["fastLineType"];
  if (s)
    fastLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["fastPeriod"];
  if (s)
    fastPeriod = s->left(s->length()).toInt();

  s = dict["fastPeriod2"];
  if (s)
    fastPeriod2 = s->left(s->length()).toInt();
  
  s = dict["fastPeriod3"];
  if (s)
    fastPeriod3 = s->left(s->length()).toInt();

  s = dict["fastPeriod4"];
  if (s)
    fastPeriod4 = s->left(s->length()).toInt();

  s = dict["fastPeriod5"];
  if (s)
    fastPeriod5 = s->left(s->length()).toInt();

  s = dict["fastPeriod6"];
  if (s)
    fastPeriod6 = s->left(s->length()).toInt();

  s = dict["fastLabel"];
  if (s)
    fastLabel = s->left(s->length());
      
  s = dict["fastLabel2"];
  if (s)
    fastLabel2 = s->left(s->length());
  
  s = dict["fastLabel3"];
  if (s)
    fastLabel3 = s->left(s->length());
  
  s = dict["fastLabel4"];
  if (s)
    fastLabel4 = s->left(s->length());
  
  s = dict["fastLabel5"];
  if (s)
    fastLabel5 = s->left(s->length());
  
  s = dict["fastLabel6"];
  if (s)
    fastLabel6 = s->left(s->length());
  
  s = dict["fastDisplace"];
  if (s)
    fastDisplace = s->left(s->length()).toInt();
  
  s = dict["fastDisplace2"];
  if (s)
    fastDisplace2 = s->left(s->length()).toInt();
  
  s = dict["fastDisplace3"];
  if (s)
    fastDisplace3 = s->left(s->length()).toInt();
  
  s = dict["fastDisplace4"];
  if (s)
    fastDisplace4 = s->left(s->length()).toInt();
  
  s = dict["fastDisplace5"];
  if (s)
    fastDisplace5 = s->left(s->length()).toInt();
  
  s = dict["fastDisplace6"];
  if (s)
    fastDisplace6 = s->left(s->length()).toInt();
  
  s = dict["fastMaType"];
  if (s)
    fastMaType = (QSMath::MAType) s->left(s->length()).toInt();
    
  s = dict["fastInput"];
  if (s)
    fastInput = (BarData::InputType) s->left(s->length()).toInt();

  // slow
  
  s = dict["slowColor"];
  if (s)
    slowColor.setNamedColor(s->left(s->length()));
    
  s = dict["slowLineType"];
  if (s)
    slowLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["slowPeriod"];
  if (s)
    slowPeriod = s->left(s->length()).toInt();

  s = dict["slowPeriod2"];
  if (s)
    slowPeriod2 = s->left(s->length()).toInt();
  
  s = dict["slowPeriod3"];
  if (s)
    slowPeriod3 = s->left(s->length()).toInt();

  s = dict["slowPeriod4"];
  if (s)
    slowPeriod4 = s->left(s->length()).toInt();

  s = dict["slowPeriod5"];
  if (s)
    slowPeriod5 = s->left(s->length()).toInt();

  s = dict["slowPeriod6"];
  if (s)
    slowPeriod6 = s->left(s->length()).toInt();

  s = dict["slowLabel"];
  if (s)
    slowLabel = s->left(s->length());
      
  s = dict["slowLabel2"];
  if (s)
    slowLabel2 = s->left(s->length());
  
  s = dict["slowLabel3"];
  if (s)
    slowLabel3 = s->left(s->length());
  
  s = dict["slowLabel4"];
  if (s)
    slowLabel4 = s->left(s->length());
  
  s = dict["slowLabel5"];
  if (s)
    slowLabel5 = s->left(s->length());
  
  s = dict["slowLabel6"];
  if (s)
    slowLabel6 = s->left(s->length());
  
  s = dict["slowDisplace"];
  if (s)
    slowDisplace = s->left(s->length()).toInt();
  
  s = dict["slowDisplace2"];
  if (s)
    slowDisplace2 = s->left(s->length()).toInt();
  
  s = dict["slowDisplace3"];
  if (s)
    slowDisplace3 = s->left(s->length()).toInt();
  
  s = dict["slowDisplace4"];
  if (s)
    slowDisplace4 = s->left(s->length()).toInt();
  
  s = dict["slowDisplace5"];
  if (s)
    slowDisplace5 = s->left(s->length()).toInt();
  
  s = dict["slowDisplace6"];
  if (s)
    slowDisplace6 = s->left(s->length()).toInt();
  
  s = dict["slowMaType"];
  if (s)
    slowMaType = (QSMath::MAType) s->left(s->length()).toInt();
    
  s = dict["slowInput"];
  if (s)
    slowInput = (BarData::InputType) s->left(s->length()).toInt();
    
  // long
  
  s = dict["longColor"];
  if (s)
    longColor.setNamedColor(s->left(s->length()));
    
  s = dict["longLineType"];
  if (s)
    longLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["longPeriod"];
  if (s)
    longPeriod = s->left(s->length()).toInt();

  s = dict["longLabel"];
  if (s)
    longLabel = s->left(s->length());
      
  s = dict["longDisplace"];
  if (s)
    longDisplace = s->left(s->length()).toInt();
  
  s = dict["longMaType"];
  if (s)
    longMaType = (QSMath::MAType) s->left(s->length()).toInt();
    
  s = dict["longInput"];
  if (s)
    longInput = (BarData::InputType) s->left(s->length()).toInt();
}

void MMA::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("fastColor", new QString(fastColor.name()));
  dict.replace("fastLineType", new QString(QString::number(fastLineType)));
  dict.replace("fastPeriod", new QString(QString::number(fastPeriod)));
  dict.replace("fastPeriod2", new QString(QString::number(fastPeriod2)));
  dict.replace("fastPeriod3", new QString(QString::number(fastPeriod3)));
  dict.replace("fastPeriod4", new QString(QString::number(fastPeriod4)));
  dict.replace("fastPeriod5", new QString(QString::number(fastPeriod5)));
  dict.replace("fastPeriod6", new QString(QString::number(fastPeriod6)));
  dict.replace("fastLabel", new QString(fastLabel));
  dict.replace("fastLabel2", new QString(fastLabel2));
  dict.replace("fastLabel3", new QString(fastLabel3));
  dict.replace("fastLabel4", new QString(fastLabel4));
  dict.replace("fastLabel5", new QString(fastLabel5));
  dict.replace("fastLabel6", new QString(fastLabel6));
  dict.replace("fastDisplace", new QString(QString::number(fastDisplace)));
  dict.replace("fastDisplace2", new QString(QString::number(fastDisplace2)));
  dict.replace("fastDisplace3", new QString(QString::number(fastDisplace3)));
  dict.replace("fastDisplace4", new QString(QString::number(fastDisplace4)));
  dict.replace("fastDisplace5", new QString(QString::number(fastDisplace5)));
  dict.replace("fastDisplace6", new QString(QString::number(fastDisplace6)));
  dict.replace("fastMaType", new QString(QString::number(fastMaType)));
  dict.replace("fastInput", new QString(QString::number(fastInput)));

  dict.replace("slowColor", new QString(slowColor.name()));
  dict.replace("slowLineType", new QString(QString::number(slowLineType)));
  dict.replace("slowPeriod", new QString(QString::number(slowPeriod)));
  dict.replace("slowPeriod2", new QString(QString::number(slowPeriod2)));
  dict.replace("slowPeriod3", new QString(QString::number(slowPeriod3)));
  dict.replace("slowPeriod4", new QString(QString::number(slowPeriod4)));
  dict.replace("slowPeriod5", new QString(QString::number(slowPeriod5)));
  dict.replace("slowPeriod6", new QString(QString::number(slowPeriod6)));
  dict.replace("slowLabel", new QString(slowLabel));
  dict.replace("slowLabel2", new QString(slowLabel2));
  dict.replace("slowLabel3", new QString(slowLabel3));
  dict.replace("slowLabel4", new QString(slowLabel4));
  dict.replace("slowLabel5", new QString(slowLabel5));
  dict.replace("slowLabel6", new QString(slowLabel6));
  dict.replace("slowDisplace", new QString(QString::number(slowDisplace)));
  dict.replace("slowDisplace2", new QString(QString::number(slowDisplace2)));
  dict.replace("slowDisplace3", new QString(QString::number(slowDisplace3)));
  dict.replace("slowDisplace4", new QString(QString::number(slowDisplace4)));
  dict.replace("slowDisplace5", new QString(QString::number(slowDisplace5)));
  dict.replace("slowDisplace6", new QString(QString::number(slowDisplace6)));
  dict.replace("slowMaType", new QString(QString::number(slowMaType)));
  dict.replace("slowInput", new QString(QString::number(slowInput)));

  dict.replace("longColor", new QString(longColor.name()));
  dict.replace("longLineType", new QString(QString::number(longLineType)));
  dict.replace("longPeriod", new QString(QString::number(longPeriod)));
  dict.replace("longLabel", new QString(longLabel));
  dict.replace("longDisplace", new QString(QString::number(longDisplace)));
  dict.replace("longMaType", new QString(QString::number(longMaType)));
  dict.replace("longInput", new QString(QString::number(longInput)));
    
  dict.replace("plugin", new QString(pluginName));
  
  saveFile(file, dict);
}

Plugin * create ()
{
  MMA *o = new MMA;
  return ((Plugin *) o);
}


