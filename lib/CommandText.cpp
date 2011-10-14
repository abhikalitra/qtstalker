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

#include "CommandText.h"
#include "ChartObjectData.h"
#include "DataColor.h"
#include "DataDouble.h"
#include "DataString.h"
#include "DataBool.h"
#include "DataInteger.h"
#include "VerifyDataInput.h"
#include "DataDateTime.h"

#include <QtDebug>

CommandText::CommandText (QObject *p) : Command (p)
{
  _name = "CHART_OBJECT_TEXT";
}

int CommandText::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // COLOR
  QColor color;
  QString s = sg->value("COLOR");
  if (vdi.toColor(script, s, color))
  {
    qDebug() << "CommandText::runScript: invalid COLOR, using default" << s;
    color = QColor(Qt::red);
  }

  // START_DATE
  QDateTime date;
  s = sg->value("DATE");
  if (vdi.toDateTime(script, s, date))
  {
    qDebug() << "CommandText::runScript: invalid DATE, using default" << s;
    date = QDateTime::currentDateTime();
  }

  // PRICE
  double price = 0;
  s = sg->value("PRICE");
  if (vdi.toDouble(script, s, price))
  {
    qDebug() << "CommandText::runScript: invalid PRICE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // CHART
  QString chart;
  s = sg->value("CHART");
  if (vdi.toString(script, s, chart))
  {
    qDebug() << "CommandText::runScript: invalid CHART" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // Z
  int z = 0;
  s = sg->value("Z");
  if (vdi.toInteger(script, s, z))
  {
    qDebug() << "CommandText::runScript: invalid Z, using default" << s;
    z = 0;
  }

  // TEXT
  QString text;
  s = sg->value("TEXT");
  if (vdi.toString(script, s, text))
  {
    qDebug() << "CommandText::runScript: invalid TEXT, using default" << s;
    text = tr("Text");
  }

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_COLOR, new DataColor(color));
  co->set(ChartObjectData::_DATE, new DataDateTime(date));
  co->set(ChartObjectData::_PRICE, new DataDouble(price));
  co->set(ChartObjectData::_TEXT, new DataString(text));
  co->set(ChartObjectData::_CHART, new DataString(chart));
  co->set(ChartObjectData::_Z, new DataInteger(z));
  co->set(ChartObjectData::_ID, new DataInteger(id));
  co->set(ChartObjectData::_RO, new DataBool(TRUE));
  co->set(ChartObjectData::_TYPE, new DataString(QString("Text")));

  script->setData(QString::number(id), co);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
