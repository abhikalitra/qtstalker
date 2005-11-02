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

#include "CCI.h"
#include "PrefDialog.h"
#include <math.h>
#include <qdict.h>
#include <qobject.h>

CCI::CCI ()
{
  pluginName = "CCI";
  helpFile = "cci.html";
  
  setDefaults();
}

CCI::~CCI ()
{
}

void CCI::setDefaults ()
{
  color.setNamedColor("red");
  lineColor.setNamedColor("gray");
  lineType = PlotLine::Line;
  label = pluginName;
  smoothing = 3;
  period = 20;
  maType = 1;
  upperLine = 100;
  lowerLine = -100;
}

void CCI::calculate ()
{
  PlotLine *cci = new PlotLine;

  PlotLine *tp = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    tp->append((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 3);
  int tpLoop = tp->getSize() - 1;

  PlotLine *sma = getMA(tp, maType, period);
  int smaLoop = sma->getSize() - 1;

  while (tpLoop >= period && smaLoop >= period)
  {
    double md = 0;
    int loop;
    for (loop = 0; loop < period; loop++)
      md = md + fabs(tp->getData(tpLoop - loop) - sma->getData(smaLoop - loop));
    md = md / period;

    double t = (tp->getData(tpLoop) - sma->getData(smaLoop)) / (0.015 * md);
    cci->prepend(t);

    tpLoop--;
    smaLoop--;
  }

  delete tp;
  delete sma;

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(cci, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete cci;
  }
  else
  {
    cci->setColor(color);
    cci->setType(lineType);
    cci->setLabel(label);
    output->addLine(cci);
  }

  PlotLine *bline = new PlotLine();
  bline->setColor(lineColor);
  bline->setType(PlotLine::Horizontal);
  bline->append(upperLine);
  output->addLine(bline);

  PlotLine *sline = new PlotLine();
  sline->setColor(lineColor);
  sline->setType(PlotLine::Horizontal);
  sline->append(lowerLine);
  output->addLine(sline);
}

int CCI::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  QString zc = QObject::tr("Zone Color");
  QString bzl = QObject::tr("Buy Zone");
  QString szl = QObject::tr("Sell Zone");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("CCI Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);

  pl = QObject::tr("Zones");
  dialog->addColorItem(zc, pl, lineColor);
  dialog->addDoubleItem(bzl, pl, upperLine);
  dialog->addDoubleItem(szl, pl, lowerLine);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    smoothing = dialog->getInt(sl);
    maType = dialog->getComboIndex(stl);

    lineColor = dialog->getColor(zc);
    upperLine = dialog->getDouble(bzl);
    lowerLine = dialog->getDouble(szl);

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void CCI::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("lineColor");
  if (s.length())
    lineColor.setNamedColor(s);

  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();

  s = dict.getData("upperLine");
  if (s.length())
    upperLine = s.toDouble();

  s = dict.getData("lowerLine");
  if (s.length())
    lowerLine = s.toDouble();
}

void CCI::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineColor", lineColor.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("upperLine", QString::number(upperLine));
  dict.setData("lowerLine", QString::number(lowerLine));
  dict.setData("plugin", pluginName);
}

PlotLine * CCI::calculateCustom (QString &p, QPtrList<PlotLine> &)
{
  // format: MA_TYPE, PERIOD, SMOOTHING

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("CCI::calculateCustom: invalid parm count");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[0]) == -1)
  {
    qDebug("CCI::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    maType = mal.findIndex(l[0]);

  bool ok;
  int t = l[1].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("CCI::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  t = l[2].toInt(&ok);
  if (ok)
    smoothing = t;
  else
  {
    qDebug("CCI::calculateCustom: invalid SMOOTHING parm");
    return 0;
  }

  clearOutput();
  calculate();
  return output->getLine(0);
}

int CCI::getMinBars ()
{
  int t = minBars + period + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CCI *o = new CCI;
  return ((IndicatorPlugin *) o);
}





