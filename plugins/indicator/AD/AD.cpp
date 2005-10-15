/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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


#include "AD.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>


AD::AD ()
{
  pluginName = "AD";
  
  methodList.append("AD");
  methodList.append("WAD");
  methodList.append("CO");
  methodList.append("CMF");

  helpFile = "ad.html";
    
  setDefaults();
}

AD::~AD ()
{
}

void AD::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  method = "AD";
  cmfPeriod = 21;
}

void AD::calculate ()
{
  if (! method.compare("AD"))
    calculateAD();
  else
  {
    if (! method.compare("WAD"))
      calculateWAD();
    else
    {
      if (! method.compare("CO"))
        calculateCO();
      else
        calculateCMF();
    }
  }
}

void AD::calculateAD ()
{
  PlotLine *line = getAD();
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  output->addLine(line);
}

PlotLine * AD::getAD ()
{
  PlotLine *line = new PlotLine();
  
  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double volume = data->getVolume(loop);
    if (volume > 0)
    {
      double high = data->getHigh(loop);
      double low = data->getLow(loop);

      double t = high - low;

      if (t != 0)
      {
        double close = data->getClose(loop);
        double t2 = (close - low) - (high - close);
        accum = accum + ((t2 / t) * volume);
      }
    }

    line->append(accum);
  }
  
  return line;
}

void AD::calculateWAD ()
{
  PlotLine *wad = new PlotLine();
  wad->setColor(color);
  wad->setType(lineType);
  wad->setLabel(label);

  int loop;
  double accum = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double close = data->getClose(loop);
    double yclose = data->getClose(loop - 1);

    double h = high;
    if (yclose > h)
      h = yclose;

    double l = low;
    if (yclose < l)
      l = yclose;

    if (close > yclose)
      accum = accum + (close - l);
    else
    {
      if (yclose == close)
        ;
      else
        accum = accum - (h - close);
    }

    wad->append(accum);
  }

  output->addLine(wad);
}

void AD::calculateCO ()
{
  PlotLine *co = new PlotLine();
  co->setColor(color);
  co->setType(lineType);
  co->setLabel(label);

  PlotLine *ad = getAD();

  PlotLine *fma = getMA(ad, 0, 3);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(ad, 0, 10);
  int smaLoop = sma->getSize() - 1;

  while (fmaLoop > -1 && smaLoop > -1)
  {
    co->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }

  output->addLine(co);

  delete ad;
  delete fma;
  delete sma;
}

void AD::calculateCMF ()
{
  PlotLine *cmf = new PlotLine();
  cmf->setColor(color);
  cmf->setType(lineType);
  cmf->setLabel(label);

  PlotLine *ad = getAD();

  int loop;
  for (loop = cmfPeriod - 1; loop < (int) data->count(); loop++)
  {
    double adsum = 0;  
    double vsum = 0;
    int loop2;
    for (loop2 = cmfPeriod - 1; loop2 > -1; loop2--)
    {
      adsum = adsum + ad->getData(loop - loop2);
      vsum = vsum + data->getVolume(loop - loop2);
    }

    cmf->append(adsum / vsum);
  }

  output->addLine(cmf);

  delete ad;
}

int AD::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString ml = QObject::tr("Method");
  QString cmfpl = QObject::tr("Period");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("AD Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addComboItem(ml, pl, methodList, method);
  dialog->addIntItem(cmfpl, pl, cmfPeriod, 1, 999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    method = dialog->getCombo(ml);
    cmfPeriod = dialog->getInt(cmfpl);

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

PlotLine * AD::calculateCustom (QString &p, QPtrList<PlotLine> &)
{
  // format1: AD_TYPE
  // format2: AD_TYPE, PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 1 || l.count() == 2)
    ;
  else
  {
    qDebug("AD::calculateCustom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("AD::calculateCustom: invalid AD_TYPE parm");
    return 0;
  }
  else
    method = l[0];

  if (l.count() == 2)
  {
    bool ok;
    int t = l[1].toInt(&ok);
    if (ok)
      cmfPeriod = t;
    else
    {
      qDebug("AD::calculateCustom: invalid PERIOD parm");
      return 0;
    }
  }

  clearOutput();
  calculate();
  return output->getLine(0);
}

void AD::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("method", method);
  dict.setData("cmfPeriod", QString::number(cmfPeriod));
  dict.setData("plugin", pluginName);
}

void AD::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("label");
  if (s.length())
    label = s;
        
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("cmfPeriod");
  if (s.length())
    cmfPeriod = s.toInt();

  s = dict.getData("method");
  if (s.length())
    method = s;
}

int AD::getMinBars ()
{
  int t = minBars;
  if (! method.compare("CMF"))
    t = t + cmfPeriod;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  AD *o = new AD;
  return ((IndicatorPlugin *) o);
}



