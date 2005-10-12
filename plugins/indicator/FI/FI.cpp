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

#include "FI.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

FI::FI ()
{
  pluginName = "FI";
  helpFile = "fi.html";
  
  setDefaults();
}

FI::~FI ()
{
}

void FI::setDefaults ()
{
  color.setNamedColor("orange");
  lineType = PlotLine::HistogramBar;
  label = pluginName;
  smoothing = 2;
  maType = 0;
}

void FI::calculate ()
{
  PlotLine *fi = new PlotLine();
  int loop;
  double force = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double cdiff = data->getClose(loop) - data->getClose(loop - 1);
    force = data->getVolume(loop) * cdiff;
    fi->append(force);
  }

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(fi, maType, smoothing, 0, 0);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete fi;
  }
  else
  {
    fi->setColor(color);
    fi->setType(lineType);
    fi->setLabel(label);
    output->addLine(fi);
  }
}

int FI::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("FI Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    smoothing = dialog->getInt(sl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void FI::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
}

void FI::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
}

PlotLine * FI::calculateCustom (QString &p, QPtrList<PlotLine> &)
{
  // format1: MA_TYPE, SMOOTHING

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2)
    ;
  else
  {
    qDebug("FI::calculateCustom: invalid parm count");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[0]) == -1)
  {
    qDebug("FI::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    maType = mal.findIndex(l[0]);

  bool ok;
  int t = l[1].toInt(&ok);
  if (ok)
    smoothing = t;
  else
  {
    qDebug("FI::calculateCustom: invalid SMOOTHING parm");
    return 0;
  }

  clearOutput();
  calculate();
  return output->getLine(0);
}

int FI::getMinBars ()
{
  int t = minBars + 1 + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  FI *o = new FI;
  return ((IndicatorPlugin *) o);
}



