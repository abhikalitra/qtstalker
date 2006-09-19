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

#include "SYMBOL.h"
#include "DbPlugin.h"
#include <qdatetime.h>
#include <qdict.h>
#include "PrefDialog.h"
#include "Config.h"
#include <qobject.h>

SYMBOL::SYMBOL ()
{
  pluginName = "SYMBOL";

  colorLabel = "color";
  lineTypeLabel = "lineType";
  labelLabel = "label";
  symbolLabel = "symbol";
  pluginLabel = "plugin";

  formatList.append(FormatString);

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
  Config config;
  DbPlugin db;
  if (db.openChart(symbol))
  {
    db.close();
    return;
  }
  
  PlotLine *line = new PlotLine();
  
  QDateTime date;
  data->getDate(0, date);

  QString ts;
  config.getData(Config::BarLength, ts);
  db.setBarLength((BarData::BarLength) ts.toInt());
  config.getData(Config::Bars, ts);
  db.setBarRange(ts.toInt());
  BarData *recordList = new BarData(symbol);
  QDateTime dt = QDateTime::currentDateTime();
  db.getHistory(recordList, dt);

  QDict<Setting> dict;
  dict.setAutoDelete(TRUE);
  
  int loop;
  ts = "Close";
  QString ts2;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *r = new Setting;
    ts2 = QString::number(recordList->getClose(loop));
    r->setData(ts, ts2);
    recordList->getDate(loop, dt);
    QString s = dt.toString("yyyyMMddhhmmss");
    dict.insert(s, r);
  }

  double val = 0;

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    data->getDate(loop, dt);
    QString s = dt.toString("yyyyMMddhhmmss");
    Setting *r2 = dict[s];
    if (r2)
    {
      val = r2->getDouble(ts);
      line->append(val);
    }
  }

  delete recordList;
  db.close();

  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  line->setScaleFlag(TRUE);
  output->addLine(line);
}

int SYMBOL::indicatorPrefDialog (QWidget *w)
{
  Config config;
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString sl = QObject::tr("Symbol");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SYMBOL Indicator"));
  dialog->setHelpFile(helpFile);
  dialog->createPage (pl);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  QString t;
  config.getData(Config::DataPath, t);
  dialog->addSymbolItem(sl, pl, t, symbol);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    dialog->getText(ll, label);
    dialog->getSymbol(sl, symbol);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void SYMBOL::setIndicatorSettings (Setting &dict)
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

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;
      
  dict.getData(symbolLabel, s);
  if (s.length())
    symbol = s;
}

void SYMBOL::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  dict.setData(labelLabel, label);
  dict.setData(symbolLabel, symbol);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * SYMBOL::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: SYMBOL

  if (checkFormat(p, d, 1, 1))
    return 0;

  symbol = formatStringList[0];

  clearOutput();
  calculate();
  return output->getLine(0);
}

void SYMBOL::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString sl = QObject::tr("Symbol");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("SYMBOL Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  Config config;
  dialog->addTextItem(vnl, pl, s);
  config.getData(Config::DataPath, s);
  dialog->addSymbolItem(sl, pl, s, symbol);

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    dialog->getSymbol(sl, rs);
  }

  delete dialog;
}

//************************************************************
//************************************************************
//************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  SYMBOL *o = new SYMBOL;
  return ((IndicatorPlugin *) o);
}



