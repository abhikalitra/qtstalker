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

#include "SYMBOL.h"
#include "ChartDb.h"
#include <qdatetime.h>
#include <qdict.h>
#include "PrefDialog.h"
#include "Config.h"
#include <qobject.h>

SYMBOL::SYMBOL ()
{
  pluginName = "SYMBOL";
  setDefaults();
  helpFile = "symbol.html";
}

SYMBOL::~SYMBOL ()
{
}

void SYMBOL::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
}

void SYMBOL::calculate ()
{
  ChartDb *db = new ChartDb;
  if (db->openChart(symbol))
  {
    delete db;
    return;
  }
  
  PlotLine *line = new PlotLine();
  
  BarDate date = data->getDate(0);
  
  db->setBarCompression(BarData::DailyBar);
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
      val = r2->getFloat(QObject::tr("Close"));
      line->append(val);
    }
  }

  delete recordList;
  delete db;

  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  line->setScaleFlag(TRUE);
  output->addLine(line);
}

int SYMBOL::indicatorPrefDialog (QWidget *w)
{
  Config *config = new Config;
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SYMBOL Indicator"));
  dialog->setHelpFile(helpFile);
  dialog->createPage (QObject::tr("Parms"));
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addSymbolItem(QObject::tr("Symbol"), QObject::tr("Parms"), config->getData(Config::DataPath), symbol);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    label = dialog->getText(QObject::tr("Label"));
    symbol = dialog->getSymbol(QObject::tr("Symbol"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  delete config;
  return rc;
}

void SYMBOL::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void SYMBOL::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void SYMBOL::setIndicatorSettings (Setting dict)
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
      
  s = dict.getData("symbol");
  if (s.length())
    symbol = s;
}

Setting SYMBOL::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("symbol", symbol);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * SYMBOL::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  SYMBOL *o = new SYMBOL;
  return ((IndicatorPlugin *) o);
}



