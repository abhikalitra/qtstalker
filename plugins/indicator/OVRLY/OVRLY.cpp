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

OVRLY::OVRLY ()
{
  pluginName = "OVRLY";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  set(tr("Color"), "yellow", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);

  set(tr("Base Color"), "red", Setting::Color);
  set(tr("Base Line Type"), tr("Line"), Setting::LineType);
  set(tr("Base Label"), tr("Base"), Setting::Text);
  set(tr("Base Symbol"), " ", Setting::Symbol);

  QStringList l;
  l.append(tr("Compare Price"));
  l.append(tr("Compare Performance"));
  l.sort();
  set(tr("Method"), tr("Compare Performance"), Setting::List);
  setList(tr("Method"), l);

  about = "Price Overlay\n";
}

OVRLY::~OVRLY ()
{
}

void OVRLY::calculate ()
{
  QString s = getData(tr("Method"));
  while (1)
  {
    if (! s.compare(tr("Compare Price")))
    {
      comparePrice();
      break;
    }

    if (! s.compare(tr("Compare Performance")))
    {
      comparePerformance();
      break;
    }

    break;
  }
}

void OVRLY::comparePrice ()
{

  PlotLine *tline = getInput(tr("Close"));
  PlotLine *line1 = normalize(tline);
  line1->setColor(getData(tr("Color")));
  line1->setType(getData(tr("Line Type")));
  line1->setLabel(getData(tr("Label")));
  delete tline;
  output.append(line1);

  QString s = getData(tr("Base Symbol"));
  if (s.length())
  {
    tline = getSymbolLine(s);
    PlotLine *line2 = normalize(tline);
    delete tline;
    line2->setColor(getData(tr("Base Color")));
    line2->setType(getData(tr("Base Line Type")));
    line2->setLabel(getData(tr("Base Label")));
    output.append(line2);
  }
}

void OVRLY::comparePerformance ()
{
  if (data->count() < 1)
    return;

  QString s = getData(tr("Base Symbol"));
  if (s.length() == 0)
    return;

  PlotLine *tline = getSymbolLine(s);
  if (tline->getSize() < 1)
  {
    delete tline;
    return;
  }

  int line1Loop = data->count() - 1;
  PlotLine *line1 = new PlotLine;
  line1->setColor(getData(tr("Color")));
  line1->setType(getData(tr("Line Type")));
  line1->setLabel(getData(tr("Label")));

  int line2Loop = tline->getSize() - 1;
  PlotLine *line2 = new PlotLine;
  line2->setColor(getData(tr("Base Color")));
  line2->setType(getData(tr("Base Line Type")));
  line2->setLabel(getData(tr("Base Label")));

  while (line1Loop > -1 && line2Loop > -1)
  {
    line1Loop--;
    line2Loop--;
  }
  line1Loop++;
  line2Loop++;
  Setting *r = data->at(line1Loop);
  double line1base = r->getFloat("Close");
  double line2base = tline->getData(line2Loop);

  for (; line1Loop < (int) data->count() && line2Loop < tline->getSize(); line1Loop++, line2Loop++)
  {
    r = data->at(line1Loop);
    line1->append(((r->getFloat("Close") - line1base) / line1base) * 100);
    line2->append(((tline->getData(line2Loop) - line2base) / line2base) * 100);
  }

  delete tline;

  output.append(line1);
  output.append(line2);
}

PlotLine * OVRLY::getSymbolLine (QString d)
{
  ChartDb *db = new ChartDb;
  db->openChart(d);
  Setting *r = data->at(0);
  QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
  QList<Setting> *recordList = db->getHistory(ChartDb::Daily, date);

  QDict<Setting> dict;
  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    r = recordList->at(loop);
    dict.insert(r->getData("Date"), r);
  }

  PlotLine *line = new PlotLine();

  double val = 0;

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    r = data->at(loop);
    Setting *r2 = dict[r->getData("Date")];
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

PlotLine * OVRLY::normalize (PlotLine *input)
{
  PlotLine *line = new PlotLine();

  int loop;
  double high = input->getHigh();
  double low = input->getLow();
  double scaler = (high - low) / 100;

  for (loop = 0; loop < input->getSize(); loop++)
  {
    double t = (input->getData(loop) - low) / scaler;
    line->append(t);
  }

  return line;
}

Plugin * create ()
{
  OVRLY *o = new OVRLY;
  return ((Plugin *) o);
}



