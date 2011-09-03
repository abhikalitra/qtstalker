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
  QString color = sg->get("COLOR").toString();
  QColor c(color);
  if (! c.isValid())
  {
    qDebug() << _type << "::runScript: invalid COLOR" << color;
    return _ERROR;
  }

  // verify PRICE
  QVariant v = sg->get("PRICE");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid PRICE" << v.toString();
    return _ERROR;
  }
  double price = v.toDouble();

  // CHART
  QString chart = sg->get("CHART").toString();

  // verify Z
  v = sg->get("Z");
  if (! v.canConvert(QVariant::Int))
  {
    qDebug() << _type << "::runScript: invalid Z" << v.toString();
    return _ERROR;
  }
  int z = v.toInt();

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_COLOR, QVariant(color));
  co->set(ChartObjectData::_PRICE, QVariant(price));
  co->set(ChartObjectData::_CHART, QVariant(chart));
  co->set(ChartObjectData::_Z, QVariant(z));
  co->set(ChartObjectData::_ID, QVariant(id));
  co->set(ChartObjectData::_RO, QVariant(TRUE));
  co->set(ChartObjectData::_TYPE, QVariant(QString("HLine")));

  script->setData(QString::number(id), co);

  return _OK;
}

Data * CommandHLine::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QVariant(QString()));
  sg->set("PRICE", QVariant(0));
  sg->set("COLOR", QVariant("red"));
  sg->set("Z", QVariant(1));
  return sg;
}
