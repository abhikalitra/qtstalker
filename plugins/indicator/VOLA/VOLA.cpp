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

#include "VOLA.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>
#include "PrefDialog.h"
#include <qinputdialog.h>

VOLA::VOLA ()
{
  pluginName = "VOLA";

  methodList.append("CV");
  methodList.append("SD");
  methodList.append("VOLR");

  helpFile = "volr.html";
  
  setDefaults();
}

VOLA::~VOLA ()
{
}

void VOLA::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = "";
  volrPeriod = 14;
  cvPeriod = 10;
  method = "";
  sdPeriod = 21;
  sdInput = BarData::Close;
}

void VOLA::calculate ()
{
  if (! method.compare("CV"))
    calculateCV();
  else
  {
    if (! method.compare("SD"))
    {
      PlotLine *in = data->getInput(sdInput);
      if (! in)
      {
        qDebug("VOLA::calculate: no input");
        return;
      }
      calculateSD(in);
      delete in;
    }
    else
      calculateVOLR();
  }
}

void VOLA::calculateCV ()
{
  PlotLine *cv = new PlotLine();
  cv->setColor(color);
  cv->setType(lineType);
  cv->setLabel(label);

  PlotLine *hl = new PlotLine();

  int loop;
  for (loop = 0; loop < data->count(); loop++)
    hl->append(data->getHigh(loop) - data->getLow(loop));

  PlotLine *ema = getMA(hl, 0, cvPeriod, 0.0, 0.0);

  for (loop = cvPeriod; loop < (int) ema->getSize(); loop++)
  {
    double t = (ema->getData(loop) - ema->getData(loop - cvPeriod)) / ema->getData(loop - cvPeriod) * 100;
    cv->append(t);
  }

  output->addLine(cv);

  delete hl;
  delete ema;
}

void VOLA::calculateSD (PlotLine *in)
{
  PlotLine *sd = new PlotLine();
  sd->setColor(color);
  sd->setType(lineType);
  sd->setLabel(label);

  int loop;
  for (loop = sdPeriod; loop < (int) in->getSize(); loop++)
  {
    double mean = 0;
    int loop2;
    for (loop2 = 0; loop2 < sdPeriod; loop2++)
      mean = mean + in->getData(loop - loop2);
    mean = mean / sdPeriod;

    double ds = 0;
    for (loop2 = 0; loop2 < sdPeriod; loop2++)
    {
      double t = in->getData(loop - loop2) - mean;
      ds = ds + (t * t);
    }
    ds = sqrt(ds / sdPeriod);

    sd->append(ds);
  }

  output->addLine(sd);
}

void VOLA::calculateVOLR ()
{
  PlotLine *volr = new PlotLine();
  volr->setColor(color);
  volr->setType(lineType);
  volr->setLabel(label);

  PlotLine *trg = getTR();
  int trgLoop = trg->getSize() - 1;

  PlotLine *ma = getMA(trg, 0, volrPeriod, 0, 0);
  int maLoop = ma->getSize() - 1;

  while (maLoop > -1 && trgLoop > -1)
  {
    volr->prepend(trg->getData(trgLoop) / ma->getData(maLoop));
    maLoop--;
    trgLoop--;
  }

  delete trg;
  delete ma;

  output->addLine(volr);
}

int VOLA::indicatorPrefDialog (QWidget *w)
{
  bool newFlag = FALSE;
  if (! method.length())
    newFlag = TRUE;

  if (newFlag)
  {
    bool ok;
    QString s = QInputDialog::getItem(QObject::tr("VOLA Indicator Selection"),
                                      QObject::tr("Select an indicator:"),
                                      methodList,
                                      0,
                                      TRUE,
                                      &ok,
                                      w);
    if (ok)
    {
      method = s;
      label = method;
    }
    else
      return FALSE;
  }

  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VOLA Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);

  while (1)
  {
    if (! method.compare("CV"))
    {
      dialog->addIntItem(perl, pl, cvPeriod, 1, 99999999);
      break;
    }

    if (! method.compare("SD"))
    {
      dialog->addIntItem(perl, pl, sdPeriod, 1, 99999999);
      dialog->addComboItem(il, pl, inputTypeList, sdInput);
      break;
    }

    if (! method.compare("VOLR"))
    {
      dialog->addIntItem(perl, pl, volrPeriod, 1, 99999999);
      break;
    }

    break;
  }

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);

    while (1)
    {
      if (! method.compare("CV"))
      {
        cvPeriod = dialog->getInt(perl);
        break;
      }

      if (! method.compare("SD"))
      {
        sdPeriod = dialog->getInt(perl);
        sdInput = (BarData::InputType) dialog->getComboIndex(il);
        break;
      }

      if (! method.compare("VOLR"))
      {
        volrPeriod = dialog->getInt(perl);
        break;
      }

      break;
    }

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VOLA::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("cvPeriod");
  if (s.length())
    cvPeriod = s.toInt();

  s = dict.getData("sdPeriod");
  if (s.length())
    sdPeriod = s.toInt();

  s = dict.getData("volrPeriod");
  if (s.length())
    volrPeriod = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;

  s = dict.getData("sdInput");
  if (s.length())
    sdInput = (BarData::InputType) s.toInt();

  s = dict.getData("method");
  if (s.length())
    method = s;
}

void VOLA::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("cvPeriod", QString::number(cvPeriod));
  dict.setData("sdPeriod", QString::number(sdPeriod));
  dict.setData("volrPeriod", QString::number(volrPeriod));
  dict.setData("label", label);
  dict.setData("sdInput", QString::number(sdInput));
  dict.setData("method", method);
  dict.setData("plugin", pluginName);
}

PlotLine * VOLA::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, PERIOD
  // format1: METHOD, PERIOD, ARRAY_INPUT

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2 || l.count() == 3)
    ;
  else
  {
    qDebug("VOLA::calculateCustom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("VOLA::calculateCustom: invalid METHOD parm");
    return 0;
  }
  else
    method = methodList.findIndex(l[0]);

  bool ok;
  int t = l[1].toInt(&ok);
  if (ok)
  {
    if (! method.compare("CV"))
      cvPeriod = t;
    else
    {
      if (! method.compare("SD"))
        sdPeriod = t;
      else
        volrPeriod = t;
    }
  }
  else
  {
    qDebug("VOLA::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  clearOutput();

  if (! method.compare("SD") && l.count() == 3)
  {
    if (d.count())
      calculateSD(d.at(0));
    else
    {
      qDebug("VOLA::calculateCustom: no input");
      return 0;
    }
  }
  else
    calculate();

  return output->getLine(0);
}

PlotLine * VOLA::getTR ()
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

int VOLA::getMinBars ()
{
  int t = minBars;
  if (! method.compare("CV"))
    t = t + cvPeriod;
  else
  {
    if (! method.compare("SD"))
      t = t + sdPeriod;
    else
      t = t + volrPeriod;
  }
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VOLA *o = new VOLA;
  return ((IndicatorPlugin *) o);
}

