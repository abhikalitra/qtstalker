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

#include "STOCH.h"
#include "PrefDialog.h"
#include <qdict.h>

STOCH::STOCH ()
{
  pluginName = "STOCH";
  plotFlag = FALSE;
  setDefaults();
}

STOCH::~STOCH ()
{
}

void STOCH::setDefaults ()
{
  dcolor.setNamedColor("yellow");
  dlineType = PlotLine::Dash;
  dlabel = "%D";
  dperiod = 3;
  kcolor.setNamedColor("red");
  klineType = PlotLine::Line;
  klabel = "%K";
  kperiod = 3;
  period = 14;
  buyLine = 20;
  sellLine = 80;
  maType = QSMath::SMA;  
}

void STOCH::calculate ()
{
  QSMath *t = new QSMath(data);
  PlotLine *k = t->getSTOCH(maType, period, kperiod);
  k->setColor(kcolor);
  k->setType(klineType);
  k->setLabel(klabel);
  output.append(k);

  if (dperiod > 1)
  {
    PlotLine *d = t->getMA(k, maType, dperiod);
    d->setColor(dcolor);
    d->setType(dlineType);
    d->setLabel(dlabel);
    output.append(d);
  }
  
  delete t;
}

int STOCH::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("STOCH Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("Parms"), maTypeList, maType);
  dialog->addFloatItem(tr("Buy Line"), tr("Parms"), buyLine, 0, 100);
  dialog->addFloatItem(tr("Sell Line"), tr("Parms"), sellLine, 0, 100);
  
  dialog->createPage (tr("%K Parms"));
  dialog->addColorItem(tr("%K Color"), tr("%K Parms"), kcolor);
  dialog->addComboItem(tr("%K Line Type"), tr("%K Parms"), lineTypes, klineType);
  dialog->addTextItem(tr("%K Label"), tr("%K Parms"), klabel);
  dialog->addIntItem(tr("%K Smoothing"), tr("%K Parms"), kperiod, 0, 99999999);
  
  dialog->createPage (tr("%D Parms"));
  dialog->addColorItem(tr("%D Color"), tr("%D Parms"), dcolor);
  dialog->addComboItem(tr("%D Line Type"), tr("%D Parms"), lineTypes, dlineType);
  dialog->addTextItem(tr("%D Label"), tr("%D Parms"), dlabel);
  dialog->addIntItem(tr("%D Smoothing"), tr("%D Parms"), dperiod, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dcolor = dialog->getColor(tr("%D Color"));
    dlineType = (PlotLine::LineType) dialog->getComboIndex(tr("%D Line Type"));
    dperiod = dialog->getInt(tr("%D Smoothing"));
    dlabel = dialog->getText(tr("%D Label"));
    kcolor = dialog->getColor(tr("%K Color"));
    klineType = (PlotLine::LineType) dialog->getComboIndex(tr("%K Line Type"));
    kperiod = dialog->getInt(tr("%K Smoothing"));
    klabel = dialog->getText(tr("%K Label"));
    period = dialog->getInt(tr("Period"));
    maType = (QSMath::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    buyLine = dialog->getFloat(tr("Buy Line"));
    sellLine = dialog->getFloat(tr("Sell Line"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void STOCH::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["dcolor"];
  if (s)
    dcolor.setNamedColor(s->left(s->length()));
    
  s = dict["kcolor"];
  if (s)
    kcolor.setNamedColor(s->left(s->length()));
  
  s = dict["dlineType"];
  if (s)
    dlineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["klineType"];
  if (s)
    klineType = (PlotLine::LineType) s->left(s->length()).toInt();
  
  s = dict["period"];
  if (s)
    period = s->left(s->length()).toInt();

  s = dict["dperiod"];
  if (s)
    dperiod = s->left(s->length()).toInt();
  
  s = dict["kperiod"];
  if (s)
    kperiod = s->left(s->length()).toInt();
  
  s = dict["dlabel"];
  if (s)
    dlabel = s->left(s->length());
      
  s = dict["klabel"];
  if (s)
    klabel = s->left(s->length());
  
  s = dict["maType"];
  if (s)
    maType = (QSMath::MAType) s->left(s->length()).toInt();

  s = dict["buyLine"];
  if (s)
    buyLine = s->left(s->length()).toFloat();

  s = dict["sellLine"];
  if (s)
    sellLine = s->left(s->length()).toFloat();
}

void STOCH::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("dcolor", new QString(dcolor.name()));
  dict.replace("dlineType", new QString(QString::number(dlineType)));
  dict.replace("dperiod", new QString(QString::number(dperiod)));
  dict.replace("dlabel", new QString(dlabel));
  dict.replace("kcolor", new QString(kcolor.name()));
  dict.replace("klineType", new QString(QString::number(klineType)));
  dict.replace("kperiod", new QString(QString::number(kperiod)));
  dict.replace("klabel", new QString(klabel));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("buyLine", new QString(QString::number(buyLine)));
  dict.replace("sellLine", new QString(QString::number(sellLine)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  STOCH *o = new STOCH;
  return ((Plugin *) o);
}




