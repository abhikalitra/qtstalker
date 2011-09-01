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

#include "CommandHLine.h"
#include "Script.h"
#include "ChartObjectData.h"

#include <QtDebug>

CommandHLine::CommandHLine (QObject *p) : Command (p)
{
  _type = "CHART_OBJECT_HLINE";
}

int CommandHLine::runScript (Data *sg, Script *script)
{
  // verify COLOR
  QString s = sg->get("COLOR");
  QColor color(s);
  if (! color.isValid())
  {
    qDebug() << _type << "::runScript: invalid COLOR" << s;
    return _ERROR;
  }

  // verify PRICE
  s = sg->get("PRICE");
  bool ok;
  double price = s.toDouble(&ok);
  if (! ok)
  {
    qDebug() << _type << "::runScript: invalid PRICE" << s;
    return _ERROR;
  }

  // CHART
  QString chart = sg->get("CHART");

  // verify Z
  s = sg->get("Z");
  int z = s.toInt(&ok);
  if (! ok)
  {
    qDebug() << _type << "::runScript: invalid Z" << s;
    return _ERROR;
  }

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_COLOR, color);
  co->set(ChartObjectData::_PRICE, price);
  co->set(ChartObjectData::_CHART, chart);
  co->set(ChartObjectData::_Z, z);
  co->set(ChartObjectData::_ID, id);
  co->set(ChartObjectData::_RO, 1);
  co->set(ChartObjectData::_TYPE, QString("HLine"));

  script->setData(QString::number(id), co);

  return _OK;
}

Data * CommandHLine::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QString());
  sg->set("PRICE", 0);
  sg->set("COLOR", QColor(Qt::red));
  sg->set("Z", 1);
  return sg;
}
