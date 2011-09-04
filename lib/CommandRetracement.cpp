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

#include "CommandRetracement.h"
#include "ChartObjectData.h"
#include "Script.h"

#include <QtDebug>
#include <QDateTime>

CommandRetracement::CommandRetracement (QObject *p) : Command (p)
{
  _type = "CHART_OBJECT_RETRACEMENT";
}

int CommandRetracement::runScript (Data *sg, Script *script)
{
  // verify DATE
  QString s = sg->get("DATE").toString();
  QDateTime dt = QDateTime::fromString(s, Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _type << "::runScript: invalid DATE" << s;
    return _ERROR;
  }

  // verify DATE2
  s = sg->get("DATE_2").toString();
  QDateTime dt2 = QDateTime::fromString(s, Qt::ISODate);
  if (! dt2.isValid())
  {
    qDebug() << _type << "::runScript: invalid DATE_2" << s;
    return _ERROR;
  }

  // verify COLOR
  QString color = sg->get("COLOR").toString();
  QColor c(color);
  if (! c.isValid())
  {
    qDebug() << _type << "::runScript: invalid COLOR" << color;
    return _ERROR;
  }

  // verify HIGH
  QVariant v = sg->get("HIGH");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid HIGH" << v.toString();
    return _ERROR;
  }
  double high = v.toDouble();

  // verify LOW
  v = sg->get("LOW");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid LOW" << v.toString();
    return _ERROR;
  }
  double low = v.toDouble();

  // verify EXTEND
  v = sg->get("EXTEND");
  if (! v.canConvert(QVariant::Bool))
  {
    qDebug() << _type << "::runScript: invalid EXTEND" << v.toString();
    return _ERROR;
  }
  bool extend = v.toBool();

  // verify Z
  v = sg->get("Z");
  if (! v.canConvert(QVariant::Int))
  {
    qDebug() << _type << "::runScript: invalid Z" << v.toString();
    return _ERROR;
  }
  int z = v.toInt();

  // verify LINE_1
  v = sg->get("LINE_1");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid LINE_1" << v.toString();
    return _ERROR;
  }
  double line1 = v.toDouble();

  // verify LINE_2
  v = sg->get("LINE_2");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid LINE_2" << v.toString();
    return _ERROR;
  }
  double line2 = v.toDouble();

  // verify LINE_3
  v = sg->get("LINE_3");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid LINE_3" << v.toString();
    return _ERROR;
  }
  double line3 = v.toDouble();

  // verify LINE_4
  v = sg->get("LINE_4");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid LINE_4" << v.toString();
    return _ERROR;
  }
  double line4 = v.toDouble();

  // verify LINE_5
  v = sg->get("LINE_5");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid LINE_5" << v.toString();
    return _ERROR;
  }
  double line5 = v.toDouble();

  // verify LINE_6
  v = sg->get("LINE_6");
  if (! v.canConvert(QVariant::Double))
  {
    qDebug() << _type << "::runScript: invalid LINE_6" << v.toString();
    return _ERROR;
  }
  double line6 = v.toDouble();

  // CHART
  QString chart = sg->get("CHART").toString();

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_TYPE, QVariant(QString("Retracement")));
  co->set(ChartObjectData::_DATE, QVariant(dt));
  co->set(ChartObjectData::_DATE2, QVariant(dt2));
  co->set(ChartObjectData::_COLOR, QVariant(color));
  co->set(ChartObjectData::_HIGH, QVariant(high));
  co->set(ChartObjectData::_LOW, QVariant(low));
  co->set(ChartObjectData::_EXTEND, QVariant(extend));
  co->set(ChartObjectData::_LINE1, QVariant(line1));
  co->set(ChartObjectData::_LINE2, QVariant(line2));
  co->set(ChartObjectData::_LINE3, QVariant(line3));
  co->set(ChartObjectData::_LINE4, QVariant(line4));
  co->set(ChartObjectData::_LINE5, QVariant(line5));
  co->set(ChartObjectData::_LINE6, QVariant(line6));
  co->set(ChartObjectData::_CHART, QVariant(chart));
  co->set(ChartObjectData::_Z, QVariant(z));
  co->set(ChartObjectData::_ID, QVariant(id));
  co->set(ChartObjectData::_RO, QVariant(TRUE));

  script->setData(QString::number(id), co);

  return _OK;
}

Data * CommandRetracement::settings ()
{
  Data *sg = new Data;
  sg->set("DATE", QVariant(QDateTime::currentDateTime()));
  sg->set("DATE_2", QVariant(QDateTime::currentDateTime()));
  sg->set("CHART", QVariant(QString()));
  sg->set("HIGH", QVariant(0));
  sg->set("LOW", QVariant(0));
  sg->set("COLOR", QVariant("red"));
  sg->set("EXTEND", QVariant(FALSE));
  sg->set("LINE_1", QVariant(0));
  sg->set("LINE_2", QVariant(0));
  sg->set("LINE_3", QVariant(0));
  sg->set("LINE_4", QVariant(0));
  sg->set("LINE_5", QVariant(0));
  sg->set("LINE_6", QVariant(0));
  sg->set("Z", QVariant(1));
  return sg;
}
