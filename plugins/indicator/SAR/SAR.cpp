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

#include "SAR.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

SAR::SAR ()
{
  pluginName = "SAR";
  helpFile = "sar.html";
  
  setDefaults();
}

SAR::~SAR ()
{
}

void SAR::setDefaults ()
{
  color.setNamedColor("white");
  lineType = PlotLine::Dot;
  label = pluginName;
  initial = 0.02;
  add = 0.02;
  limit = 0.2;
}

void SAR::calculate ()
{
  PlotLine *d = new PlotLine();
  d->setColor(color);
  d->setType(lineType);
  d->setLabel(label);

  double high = data->getHigh(1);
  double low = data->getLow(1);
  double yhigh = data->getHigh(0);
  double ylow = data->getLow(0);

  int flag = 0;
  double ep = 0;
  double sar = 0;
  double psar = 0;
  double a = initial;
  double t = yhigh;
  double t2 = high;
  if (t2 > t)
  {
    // we are long
    flag = 0;
    t = ylow;
    t2 = low;
    if (t < t2)
      ep = t;
    else
      ep = t2;

    sar = ep;
    psar = ep;
  }
  else
  {
    // we are short
    flag = 1;
    t = yhigh;
    t2 = high;
    if (t > t2)
      ep = t;
    else
      ep = t2;

    sar = ep;
    psar = ep;
  }

  d->append(sar);

  int loop;
  for (loop = 2; loop < (int) data->count(); loop++)
  {
    high = data->getHigh(loop);
    low = data->getLow(loop);
    yhigh = data->getHigh(loop - 1);
    ylow = data->getLow(loop - 1);

    // are we short?
    if (flag)
    {
      // check for a switch
      t = high;
      if (t >= sar)
      {
        sar = ep;
        psar = sar;
        ep = t;
        flag = 0;
        a = initial;
      }
      else
      {
        t = low;
        if (t  < ep)
        {
          ep = t;
          a = a + add;
          if (a > limit)
            a = limit;
        }

        t = psar + (a * (ep - psar));
        t2 = high;
        if (t < t2)
        {
          double t3 = yhigh;
          if (t3 > t2)
            t = t3;
          else
            t = t2;
        }
        psar = sar;
        sar = t;
      }
    }
    else
    {
      // we are long
      // check for a switch
      t = low;
      if (t <= sar)
      {
        sar = ep;
        psar = sar;
        ep = t;
        flag = 1;
        a = initial;
      }
      else
      {
        t = high;
        if (t  > ep)
        {
          ep = t;
          a = a + add;
          if (a > limit)
            a = limit;
        }

        t = psar + (a * (ep - psar));
        t2 = low;
        if (t > t2)
        {
          double t3 = ylow;
          if (t3 < t2)
            t = t3;
          else
            t = t2;
        }
        psar = sar;
        sar = t;
      }
    }

    d->append(sar);
  }

  output->addLine(d);
}

int SAR::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SAR Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addFloatItem(QObject::tr("Initial"), QObject::tr("Parms"), initial, 0, 99999999);
  dialog->addFloatItem(QObject::tr("Add"), QObject::tr("Parms"), add, 0, 99999999);
  dialog->addFloatItem(QObject::tr("Limit"), QObject::tr("Parms"), limit, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    label = dialog->getText(QObject::tr("Label"));
    initial = dialog->getFloat(QObject::tr("Initial"));
    add = dialog->getFloat(QObject::tr("Add"));
    limit = dialog->getFloat(QObject::tr("Limit"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void SAR::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void SAR::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void SAR::setIndicatorSettings (Setting dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("initial");
  if (s.length())
    initial = s.toFloat();

  s = dict.getData("add");
  if (s.length())
    add = s.toFloat();

  s = dict.getData("limit");
  if (s.length())
    limit = s.toFloat();
}

Setting SAR::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("initial", QString::number(initial));
  dict.setData("add", QString::number(add));
  dict.setData("limit", QString::number(limit));
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * SAR::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  SAR *o = new SAR;
  return ((IndicatorPlugin *) o);
}





