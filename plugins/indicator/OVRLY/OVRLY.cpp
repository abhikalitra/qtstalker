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

OVRLY::OVRLY ()
{
  pluginName = "OVRLY";
  plotFlag = FALSE;
  alertFlag = FALSE;
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

  int line1Loop = data->count() - 1;
  PlotLine *line1 = new PlotLine;
  line1->setColor(color);
  line1->setType(lineType);
  line1->setLabel(label);

  int line2Loop = tline->getSize() - 1;
  PlotLine *line2 = new PlotLine;
  line2->setColor(baseColor);
  line2->setType(baseLineType);
  line2->setLabel(baseLabel);

  while (line1Loop > -1 && line2Loop > -1)
  {
    line1Loop--;
    line2Loop--;
  }
  line1Loop++;
  line2Loop++;
  double line1base = data->getClose(line1Loop);
  double line2base = tline->getData(line2Loop);

  for (; line1Loop < (int) data->count() && line2Loop < tline->getSize(); line1Loop++, line2Loop++)
  {
    line1->append(((data->getClose(line1Loop) - line1base) / line1base) * 100);
    line2->append(((tline->getData(line2Loop) - line2base) / line2base) * 100);
  }

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
  db->setBarRange(99999999);
  BarData *recordList = db->getHistory();

  QDict<Setting> dict;
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
    Setting *r2 = dict[recordList->getDate(loop).getDateTimeString(FALSE)];
    if (! r2)
      line->append(val);
    else
    {
      val = r2->getFloat(tr("Close"));
      line->append(val);
    }
  }

  delete recordList;
  delete db;

  return line;
}

int OVRLY::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("OVRLY Indicator"));
  
  dialog->createPage (tr("Base Symbol"));
  dialog->addColorItem(tr("Base Color"), tr("Base Symbol"), baseColor);
  dialog->addComboItem(tr("Base Line Type"), tr("Base Symbol"), lineTypes, baseLineType);
  dialog->addTextItem(tr("Base Label"), tr("Base Symbol"), baseLabel);
  dialog->addComboItem(tr("Method"), tr("Base Symbol"), methodList, method);
  dialog->addSymbolItem(tr("Base Symbol"), tr("Base Symbol"), dataPath, baseSymbol);
  
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
  return rc;
}

void OVRLY::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["color"];
  if (s)
    color.setNamedColor(s->left(s->length()));
    
  s = dict["baseColor"];
  if (s)
    baseColor.setNamedColor(s->left(s->length()));
  
  s = dict["lineType"];
  if (s)
    lineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["baseLineType"];
  if (s)
    baseLineType = (PlotLine::LineType) s->left(s->length()).toInt();
  
  s = dict["label"];
  if (s)
    label = s->left(s->length());
      
  s = dict["baseLabel"];
  if (s)
    baseLabel = s->left(s->length());
  
  s = dict["method"];
  if (s)
    method = s->left(s->length());
    
  s = dict["baseSymbol"];
  if (s)
    baseSymbol = s->left(s->length());
}

void OVRLY::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("baseColor", new QString(baseColor.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("baseLineType", new QString(QString::number(baseLineType)));
  dict.replace("label", new QString(label));
  dict.replace("baseLabel", new QString(baseLabel));
  dict.replace("method", new QString(method));
  dict.replace("baseSymbol", new QString(baseSymbol));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  OVRLY *o = new OVRLY;
  return ((Plugin *) o);
}



