/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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
#include <qobject.h>
#include <math.h>

UO::UO ()
{
  pluginName = "UO";
  helpFile = "uo.html";

  colorLabel = "color";
  lineTypeLabel = "lineType";
  labelLabel = "label";
  shortPeriodLabel = "shortPeriod";
  medPeriodLabel = "medPeriod";
  longPeriodLabel = "longPeriod";
  pluginLabel = "plugin";

  formatList.append(FormatInteger);
  formatList.append(FormatInteger);
  formatList.append(FormatInteger);
  
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
  PlotLine *trg = getTR();

  PlotLine *atr = getMA(trg, 1, shortPeriod);
  int atrLoop = atr->getSize() - 1;

  PlotLine *atr2 = getMA(trg, 1, medPeriod);
  int atr2Loop = atr2->getSize() - 1;

  PlotLine *atr3 = getMA(trg, 1, longPeriod);
  int atr3Loop = atr3->getSize() - 1;

  PlotLine *f = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    f->append(data->getClose(loop) - data->getLow(loop));

  PlotLine *sma = getMA(f, 1, shortPeriod);
  int smaLoop = sma->getSize() - 1;

  PlotLine *sma2 = getMA(f, 1, medPeriod);
  int sma2Loop = sma2->getSize() - 1;

  PlotLine *sma3 = getMA(f, 1, longPeriod);
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
  output->addLine(uo);

  delete trg;
  delete atr;
  delete atr2;
  delete atr3;
  delete sma;
  delete sma2;
  delete sma3;
  delete f;
}

int UO::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString sper = QObject::tr("Short Period");
  QString mper = QObject::tr("Medium Period");
  QString lper = QObject::tr("Long Period");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("UO Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(sper, pl, shortPeriod, 1, 99999999);
  dialog->addIntItem(mper, pl, medPeriod, 1, 99999999);
  dialog->addIntItem(lper, pl, longPeriod, 1, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    dialog->getText(ll, label);
    shortPeriod = dialog->getInt(sper);
    medPeriod = dialog->getInt(mper);
    longPeriod = dialog->getInt(lper);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void UO::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s;
  dict.getData(colorLabel, s);
  if (s.length())
    color.setNamedColor(s);
    
  dict.getData(lineTypeLabel, s);
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(shortPeriodLabel, s);
  if (s.length())
    shortPeriod = s.toInt();

  dict.getData(medPeriodLabel, s);
  if (s.length())
    medPeriod = s.toInt();

  dict.getData(longPeriodLabel, s);
  if (s.length())
    longPeriod = s.toInt();
}

void UO::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(shortPeriod);
  dict.setData(shortPeriodLabel, ts);
  ts = QString::number(medPeriod);
  dict.setData(medPeriodLabel, ts);
  ts = QString::number(longPeriod);
  dict.setData(longPeriodLabel, ts);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * UO::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: SHORT_PERIOD, MED_PERIOD, LONG_PERIOD

  if (checkFormat(p, d, 3, 3))
    return 0;

  shortPeriod = formatStringList[0].toInt();
  medPeriod = formatStringList[1].toInt();
  longPeriod = formatStringList[2].toInt();

  clearOutput();
  calculate();
  return output->getLine(0);
}

PlotLine * UO::getTR ()
{
  PlotLine *tr = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double close;
    if (loop > 0)
      close = data->getClose(loop - 1);
    else
      close = high;

    double t = high - low;

    double t2 = fabs(high - close);
    if (t2 > t)
      t = t2;

    t2 = fabs(low - close);
    if (t2 > t)
      t = t2;

    tr->append(t);
  }
  
  return tr;
}

void UO::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString sper = QObject::tr("Short Period");
  QString mper = QObject::tr("Medium Period");
  QString lper = QObject::tr("Long Period");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("UO Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);
  dialog->addIntItem(sper, pl, shortPeriod, 1, 99999999);
  dialog->addIntItem(mper, pl, medPeriod, 1, 99999999);
  dialog->addIntItem(lper, pl, longPeriod, 1, 99999999);

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);

    int t = dialog->getInt(sper);
    rs = QString::number(t);

    t = dialog->getInt(mper);
    rs.append("," + QString::number(t));

    t = dialog->getInt(lper);
    rs.append("," + QString::number(t));
  }

  delete dialog;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  UO *o = new UO;
  return ((IndicatorPlugin *) o);
}

