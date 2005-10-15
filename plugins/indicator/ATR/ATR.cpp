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

#include "ATR.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>


ATR::ATR ()
{
  pluginName = "ATR";
  helpFile = "atr.html";
  
  setDefaults();
}

ATR::~ATR ()
{
}

void ATR::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  smoothing = 14;
  maType = 1;
}

void ATR::calculate ()
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
  
  if (smoothing > 1)
  {
    PlotLine *ma = getMA(tr, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete tr;
  }
  else
  {
    tr->setColor(color);
    tr->setType(lineType);
    tr->setLabel(label);
    output->addLine(tr);
  }
}

int ATR::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("ATR Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addIntItem(sl, pl, smoothing, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    smoothing = dialog->getInt(sl);
    maType = dialog->getComboIndex(stl);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ATR::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
}

void ATR::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
}

PlotLine * ATR::calculateCustom (QString &p, QPtrList<PlotLine> &)
{
  // format1: MA_TYPE, PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2)
    ;
  else
  {
    qDebug("ATR::calculateCustom: invalid parm count");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[0]) == -1)
  {
    qDebug("ATR::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    maType = mal.findIndex(l[0]);

  if (l.count() == 2)
  {
    bool ok;
    int t = l[1].toInt(&ok);
    if (ok)
      smoothing = t;
    else
    {
      qDebug("ATR::calculateCustom: invalid PERIOD parm");
      return 0;
    }
  }

  clearOutput();
  calculate();
  return output->getLine(0);
}

int ATR::getMinBars ()
{
  int t = minBars + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ATR *o = new ATR;
  return ((IndicatorPlugin *) o);
}




