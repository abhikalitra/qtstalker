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

#include "OVRLY.h"
#include "ChartDb.h"
#include <qdatetime.h>
#include <qdict.h>
#include "PrefDialog.h"
#include "Config.h"

OVRLY::OVRLY ()
{
  pluginName = "OVRLY";
  setDefaults();

  methodList.append(tr("Compare Price"));
  methodList.append(tr("Compare Performance"));
  methodList.sort();
}

OVRLY::~OVRLY ()
{
}

void OVRLY::setDefaults ()
{
  color.setNamedColor("yellow");
  baseColor.setNamedColor("red");
  lineType = PlotLine::Line;
  baseLineType = PlotLine::Line;
  label = pluginName;
  baseLabel = tr("Base");
  baseSymbol.truncate(0);
  method = tr("Compare Performance");
}

void OVRLY::calculate ()
{
  while (1)
  {
    if (! method.compare(tr("Compare Price")))
    {
      comparePrice();
      break;
    }

    if (! method.compare(tr("Compare Performance")))
    {
      comparePerformance();
      break;
    }

    break;
  }
}

void OVRLY::comparePrice ()
{
  PlotLine *line1 = data->getInput(BarData::Close);
  line1->setColor(color);
  line1->setType(lineType);
  line1->setLabel(label);
  line1->setScaleFlag(TRUE);
  output.append(line1);

  if (baseSymbol.length())
  {
    PlotLine *line2 = getSymbolLine(baseSymbol);
    line2->setColor(baseColor);
    line2->setType(baseLineType);
    line2->setLabel(baseLabel);
    line2->setScaleFlag(TRUE);
    output.append(line2);
  }
}

void OVRLY::comparePerformance ()
{
  if (data->count() < 1)
    return;

  if (baseSymbol.length() == 0)
    return;

  PlotLine *tline = getSymbolLine(baseSymbol);
  if (tline->getSize() < 1)
  {
    delete tline;
    return;
  }

  PlotLine *line1 = new PlotLine;
  line1->setColor(color);
  line1->setType(lineType);
  line1->setLabel(label);

  PlotLine *line2 = new PlotLine;
  line2->setColor(baseColor);
  line2->setType(baseLineType);
  line2->setLabel(baseLabel);

  int loop = 0;
  double base = data->getClose(loop);
  for (; loop < (int) data->count(); loop++)
    line1->append(((data->getClose(loop) - base) / base) * 100);
  
  loop = 0;
  base = tline->getData(loop);
  for (; loop < tline->getSize(); loop++)
    line2->append(((tline->getData(loop) - base) / base) * 100);

  delete tline;

  output.append(line1);
  output.append(line2);
}

PlotLine * OVRLY::getSymbolLine (QString d)
{
  PlotLine *line = new PlotLine();

  ChartDb *db = new ChartDb;
  if (db->openChart(d))
  {
    delete db;
    return line;
  }
  
  BarDate date = data->getDate(0);
  
  db->setBarCompression(ChartDb::Daily);
  db->setBarRange(data->count());
  BarData *recordList = db->getHistory();

  QDict<Setting> dict;
  dict.setAutoDelete(TRUE);
  
  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *r = new Setting;
    r->setData("Close", QString::number(recordList->getClose(loop)));
    dict.insert(recordList->getDate(loop).getDateTimeString(FALSE), r);
  }

  double val = 0;

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *r2 = dict[data->getDate(loop).getDateTimeString(FALSE)];
    if (r2)
    {
      val = r2->getFloat(tr("Close"));
      line->append(val);
    }
  }

  delete recordList;
  delete db;

  return line;
}

int OVRLY::indicatorPrefDialog (QWidget *w)
{
  Config *config = new Config;
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("OVRLY Indicator"));
  
  dialog->createPage (tr("Base Symbol"));
  dialog->addColorItem(tr("Base Color"), tr("Base Symbol"), baseColor);
  dialog->addComboItem(tr("Base Line Type"), tr("Base Symbol"), lineTypes, baseLineType);
  dialog->addTextItem(tr("Base Label"), tr("Base Symbol"), baseLabel);
  dialog->addComboItem(tr("Method"), tr("Base Symbol"), methodList, method);
  dialog->addSymbolItem(tr("Base Symbol"), tr("Base Symbol"), config->getData(Config::DataPath), baseSymbol);
  
  dialog->createPage (tr("Current Symbol"));
  dialog->addColorItem(tr("Color"), tr("Current Symbol"), color);
  dialog->addComboItem(tr("Line Type"), tr("Current Symbol"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Current Symbol"), label);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    baseColor = dialog->getColor(tr("Base Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    baseLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Base Line Type"));
    label = dialog->getText(tr("Label"));
    baseLabel = dialog->getText(tr("Base Label"));
    method = dialog->getCombo(tr("Method"));
    baseSymbol = dialog->getSymbol(tr("Base Symbol"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  delete config;
  return rc;
}

void OVRLY::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void OVRLY::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void OVRLY::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("baseColor");
  if (s.length())
    baseColor.setNamedColor(s);
  
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("baseLineType");
  if (s.length())
    baseLineType = (PlotLine::LineType) s.toInt();
  
  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("baseLabel");
  if (s.length())
    baseLabel = s;
  
  s = dict.getData("method");
  if (s.length())
    method = s;
    
  s = dict.getData("baseSymbol");
  if (s.length())
    baseSymbol = s;
}

Setting OVRLY::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("baseColor", baseColor.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("baseLineType", QString::number(baseLineType));
  dict.setData("label", label);
  dict.setData("baseLabel", baseLabel);
  dict.setData("method", method);
  dict.setData("baseSymbol", baseSymbol);
  dict.setData("plugin", pluginName);
  return dict;
}

Plugin * create ()
{
  OVRLY *o = new OVRLY;
  return ((Plugin *) o);
}



