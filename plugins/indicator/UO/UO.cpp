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

#include "UO.h"
#include "PrefDialog.h"
#include <qdict.h>

UO::UO ()
{
  pluginName = "UO";
  plotFlag = FALSE;
  alertFlag = FALSE;
  setDefaults();
}

UO::~UO ()
{
}

void UO::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  shortPeriod = 7;
  medPeriod = 14;
  longPeriod = 28;
}

void UO::calculate ()
{
  QSMath *t = new QSMath(data);
  PlotLine *trg = t->getTR();

  PlotLine *atr = t->getSMA(trg, shortPeriod);
  int atrLoop = atr->getSize() - 1;

  PlotLine *atr2 = t->getSMA(trg, medPeriod);
  int atr2Loop = atr2->getSize() - 1;

  PlotLine *atr3 = t->getSMA(trg, longPeriod);
  int atr3Loop = atr3->getSize() - 1;

  PlotLine *f = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    f->append(data->getClose(loop) - data->getLow(loop));

  PlotLine *sma = t->getSMA(f, shortPeriod);
  int smaLoop = sma->getSize() - 1;

  PlotLine *sma2 = t->getSMA(f, medPeriod);
  int sma2Loop = sma2->getSize() - 1;

  PlotLine *sma3 = t->getSMA(f, longPeriod);
  int sma3Loop = sma3->getSize() - 1;

  PlotLine *uo = new PlotLine();

  while (smaLoop > -1 && sma2Loop > -1 && sma3Loop > -1 && atrLoop > -1 && atr2Loop > -1 && atr3Loop > -1)
  {
    double t = (sma->getData(smaLoop) / atr->getData(atrLoop)) * 4;
    t = t + ((sma2->getData(sma2Loop) / atr2->getData(atr2Loop)) * 2);
    t = t + (sma3->getData(sma3Loop) / atr3->getData(atr3Loop));
    t = (t / 7) * 100;

    uo->prepend(t);

    smaLoop--;
    sma2Loop--;
    sma3Loop--;
    atrLoop--;
    atr2Loop--;
    atr3Loop--;
  }

  uo->setColor(color);
  uo->setType(lineType);
  uo->setLabel(label);
  output.append(uo);

  delete trg;
  delete atr;
  delete atr2;
  delete atr3;
  delete sma;
  delete sma2;
  delete sma3;
  delete f;
  delete t;
}

int UO::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("UO Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addIntItem(tr("Short Period"), 1, shortPeriod, 1, 99999999);
  dialog->addIntItem(tr("Medium Period"), 1, medPeriod, 1, 99999999);
  dialog->addIntItem(tr("Long Period"), 1, longPeriod, 1, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    shortPeriod = dialog->getInt(tr("Short Period"));
    medPeriod = dialog->getInt(tr("Medium Period"));
    longPeriod = dialog->getInt(tr("Long Period"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void UO::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["color"];
  if (s)
    color.setNamedColor(s->left(s->length()));
    
  s = dict["lineType"];
  if (s)
    lineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["label"];
  if (s)
    label = s->left(s->length());

  s = dict["shortPeriod"];
  if (s)
    shortPeriod = s->left(s->length()).toInt();

  s = dict["medPeriod"];
  if (s)
    medPeriod = s->left(s->length()).toInt();

  s = dict["longPeriod"];
  if (s)
    longPeriod = s->left(s->length()).toInt();
}

void UO::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("label", new QString(label));
  dict.replace("shortPeriod", new QString(QString::number(shortPeriod)));
  dict.replace("medPeriod", new QString(QString::number(medPeriod)));
  dict.replace("longPeriod", new QString(QString::number(longPeriod)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  UO *o = new UO;
  return ((Plugin *) o);
}

