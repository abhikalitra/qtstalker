/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. SQObject::tratigakos
 *
 *  This program is free software; you can redisQObject::tribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is disQObject::tributed in the hope that it will be useful,
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
}

void AD::calculate ()
{
  if (! method.compare("AD"))
    calculateAD();
  else
    calculateWAD();
}

void AD::calculateAD ()
{
  PlotLine *line = new PlotLine();
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
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
  
  output->addLine(line);
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

int AD::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString ml = QObject::tr("Method");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("AD Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addComboItem(ml, pl, methodList, method);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    method = dialog->getCombo(ml);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

PlotLine * AD::calculateCustom (QDict<PlotLine> *)
{
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

  s = dict.getData("method");
  if (s.length())
    method = s;
}

IndicatorPlugin * createIndicatorPlugin ()
{
  AD *o = new AD;
  return ((IndicatorPlugin *) o);
}



