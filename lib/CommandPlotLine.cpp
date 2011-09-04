/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "CommandPlotLine.h"
#include "Script.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "CurveType.h"

#include <QtDebug>

CommandPlotLine::CommandPlotLine (QObject *p) : Command (p)
{
  _type = "PLOT_LINE";
}

int CommandPlotLine::runScript (Data *sg, Script *script)
{
  // input
  QString s = sg->get("INPUT").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT not found" << s;
    return _ERROR;
  }

  // output
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  // style
  CurveType ct;
  s = sg->get("STYLE").toString();
  if (ct.stringToType(s) == -1)
  {
    qDebug() << _type << "::runScript: invalid STYLE" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  // color
  s = sg->get("COLOR").toString();
  QColor c(s);
  if (! c.isValid())
  {
    qDebug() << _type << "::runScript: invalid COLOR" << s;
    return _ERROR;
  }

  line = new CurveData;
  line->set(CurveData::_TYPE, QVariant(QString("Line")));
  line->set(CurveData::_Z, sg->get("Z"));
  line->set(CurveData::_PEN, sg->get("PEN"));
  line->set(CurveData::_LABEL, sg->get("OUTPUT"));
  line->set(CurveData::_CHART, sg->get("CHART"));
  line->set(CurveData::_STYLE, sg->get("STYLE"));

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *ibar = in->getData(keys.at(loop));
    if (! ibar)
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_VALUE, ibar->get(CurveBar::_VALUE));
    bar->set(CurveBar::_COLOR, sg->get("COLOR"));
    line->set(keys.at(loop), bar);
  }

  script->setData(name, line);

  return _OK;
}

Data * CommandPlotLine::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QVariant(QString()));
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("INPUT", QVariant(QString("close")));
  sg->set("STYLE", QVariant(QString("Line")));
  sg->set("COLOR", QVariant(QString("red")));
  sg->set("Z", QVariant(-1));
  sg->set("PEN", QVariant(1));
  return sg;
}
