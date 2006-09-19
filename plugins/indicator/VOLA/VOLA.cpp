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
  methodList.append("VOLR");

  colorLabel = "color";
  lineTypeLabel = "lineType";
  cvPeriodLabel = "cvPeriod";
  volrPeriodLabel = "volrPeriod";
  labelLabel = "label";
  methodLabel = "method";
  pluginLabel = "plugin";

  helpFile = "volr.html";

  formatList.append(FormatString);
  formatList.append(FormatInteger);
  
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
}

void VOLA::calculate ()
{
  if (! method.compare("CV"))
    calculateCV();
  else
    calculateVOLR();
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

  PlotLine *ema = getMA(hl, 0, cvPeriod);

  for (loop = cvPeriod; loop < (int) ema->getSize(); loop++)
  {
    double t = (ema->getData(loop) - ema->getData(loop - cvPeriod)) / ema->getData(loop - cvPeriod) * 100;
    cv->append(t);
  }

  output->addLine(cv);

  delete hl;
  delete ema;
}

void VOLA::calculateVOLR ()
{
  PlotLine *volr = new PlotLine();
  volr->setColor(color);
  volr->setType(lineType);
  volr->setLabel(label);

  PlotLine *trg = getTR();
  int trgLoop = trg->getSize() - 1;

  PlotLine *ma = getMA(trg, 0, volrPeriod);
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
    dialog->getColor(cl, color);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    dialog->getText(ll, label);

    while (1)
    {
      if (! method.compare("CV"))
      {
        cvPeriod = dialog->getInt(perl);
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
  
  QString s;
  dict.getData(colorLabel, s);
  if (s.length())
    color.setNamedColor(s);
    
  dict.getData(lineTypeLabel, s);
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  dict.getData(cvPeriodLabel, s);
  if (s.length())
    cvPeriod = s.toInt();

  dict.getData(volrPeriodLabel, s);
  if (s.length())
    volrPeriod = s.toInt();

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(methodLabel, s);
  if (s.length())
    method = s;
}

void VOLA::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  ts = QString::number(cvPeriod);
  dict.setData(cvPeriodLabel, ts);
  ts = QString::number(volrPeriod);
  dict.setData(volrPeriodLabel, ts);
  dict.setData(labelLabel, label);
  dict.setData(methodLabel, method);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * VOLA::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, PERIOD

  if (checkFormat(p, d, 2, 2))
    return 0;

  method = formatStringList[0];
  if (methodList.findIndex(method) == -1)
  {
    qDebug("VOLA::calculateCustom: invalid METHOD parm");
    return 0;
  }

  if (! method.compare("CV"))
    cvPeriod = formatStringList[1].toInt();
  else
    volrPeriod = formatStringList[1].toInt();

  clearOutput();
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

void VOLA::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString ml = QObject::tr("Method");
  QString perl = QObject::tr("Period");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("VOL Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  // format1: METHOD, PERIOD

  QString s;
  dialog->addTextItem(vnl, pl, s);
  dialog->addComboItem(ml, pl, methodList, method);
  dialog->addIntItem(perl, pl, cvPeriod, 1, 99999999);

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);

    dialog->getCombo(ml, rs);

    int t = dialog->getInt(perl);
    rs.append("," + QString::number(t));
  }

  delete dialog;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VOLA *o = new VOLA;
  return ((IndicatorPlugin *) o);
}

