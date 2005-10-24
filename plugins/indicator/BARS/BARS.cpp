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

#include "BARS.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <qinputdialog.h>


BARS::BARS ()
{
  pluginName = "BARS";
  helpFile = "bars.html";

  methodList.append("Bar");
  methodList.append("Candle");
  
  setDefaults();
}

BARS::~BARS ()
{
}

void BARS::setDefaults ()
{
  barUpColor.setNamedColor("green");
  barDownColor.setNamedColor("red");
  barNeutralColor.setNamedColor("blue");
  candleColor.setNamedColor("green");
  label = pluginName;
}

void BARS::calculate ()
{
  if (! method.compare("Bar"))
  {
    calculateBar();
    return;
  }

  if (! method.compare("Candle"))
  {
    calculateCandle();
    return;
  }

}

void BARS::calculateBar ()
{
  PlotLine *line = new PlotLine;
  int loop;
  QColor color;

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    if (loop > 0)
    {  
      if (data->getClose(loop) > data->getClose(loop - 1))
        color = barUpColor;
      else
      {
        if (data->getClose(loop) < data->getClose(loop - 1))
          color = barDownColor;
        else
          color = barNeutralColor;
      }
    }
    else
      color = barNeutralColor;

    line->appendBar(color, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop));
  }

  line->setType(PlotLine::Bar);
  line->setLabel(label);
  output->addLine(line);
}

void BARS::calculateCandle ()
{
  PlotLine *line = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double c = data->getClose(loop);
    double o = data->getOpen(loop);
    bool fillFlag = FALSE;

    if (o != 0)
    {
      if (c < o)
        fillFlag = TRUE;
    }

    line->appendBar(candleColor, o, data->getHigh(loop), data->getLow(loop), c, fillFlag);
  }

  line->setType(PlotLine::Candle);
  line->setLabel(label);
  output->addLine(line);
}

int BARS::indicatorPrefDialog (QWidget *w)
{
  bool newFlag = FALSE;
  if (! method.length())
    newFlag = TRUE;

  if (newFlag)
  {
    bool ok;
    QString s = QInputDialog::getItem(QObject::tr("BARS type Selection"),
                                      QObject::tr("Select a bar type:"),
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
  QString ucl = QObject::tr("Up Color");
  QString dcl = QObject::tr("Down Color");
  QString ncl = QObject::tr("Neutral Color");
  QString ccl = QObject::tr("Candle Color");
  QString ll = QObject::tr("Label");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("BARS Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  while (1)
  {
    if (! method.compare("Bar"))
    {
      dialog->addColorItem(ucl, pl, barUpColor);
      dialog->addColorItem(dcl, pl, barDownColor);
      dialog->addColorItem(ncl, pl, barNeutralColor);
      break;
    }

    if (! method.compare("Candle"))
    {
      dialog->addColorItem(ccl, pl, candleColor);
      break;
    }

    break;
  }

  dialog->addTextItem(ll, pl, label);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    while (1)
    {
      if (! method.compare("Bar"))
      {
        barUpColor = dialog->getColor(ucl);
        barDownColor = dialog->getColor(dcl);
        barNeutralColor = dialog->getColor(ncl);
        lineType = PlotLine::Bar;
        break;
      }

      if (! method.compare("Candle"))
      {
        candleColor = dialog->getColor(ccl);
        lineType = PlotLine::Candle;
        break;
      }

      break;
    }

    label = dialog->getText(ll);

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void BARS::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("label");
  if (s.length())
    label = s;
        
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("method");
  if (s.length())
    method = s;

  s = dict.getData("barUpColor");
  if (s.length())
    barUpColor.setNamedColor(s);
    
  s = dict.getData("barDownColor");
  if (s.length())
    barDownColor.setNamedColor(s);
    
  s = dict.getData("barNeutralColor");
  if (s.length())
    barNeutralColor.setNamedColor(s);

  s = dict.getData("candleColor");
  if (s.length())
    candleColor.setNamedColor(s);
}

void BARS::getIndicatorSettings (Setting &dict)
{
  dict.setData("barUpColor", barUpColor.name());
  dict.setData("barDownColor", barDownColor.name());
  dict.setData("barNeutralColor", barNeutralColor.name());
  dict.setData("candleColor", candleColor.name());
  dict.setData("label", label);
  dict.setData("method", method);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
}

PlotLine * BARS::calculateCustom (QString &p, QPtrList<PlotLine> &)
{
  // format1 (BARS): TYPE

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 1)
    ;
  else
  {
    qDebug("BARS::calculateCustom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("BARS::calculateCustom: invalid TYPE parm");
    return 0;
  }
  else
    method = l[0];

  clearOutput();
  calculate();
  return output->getLine(0);
}

int BARS::getMinBars ()
{
  return minBars;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BARS *o = new BARS;
  return ((IndicatorPlugin *) o);
}




