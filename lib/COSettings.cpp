/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "COSettings.h"
#include "Config.h"



COSettings::COSettings (QString i, QString s, QString indi, QString t)
{
  id = i;
  symbol = s;
  indicator = indi;
  type = t;
  dateFormat = "yyyy-MM-dd HH:mm:ss";
  selected = FALSE;
  saveFlag = FALSE;
}

COSettings::COSettings ()
{
  dateFormat = "yyyy-MM-dd HH:mm:ss";
  selected = FALSE;
  saveFlag = FALSE;
}

COSettings::~COSettings ()
{
  qDeleteAll(grabHandles);
  qDeleteAll(selectionArea);
}

void COSettings::getSettings (QString &s)
{
  s.clear();
  QStringList l;
  QHashIterator<COParm, QString> it(settings);
  while (it.hasNext())
  {
    it.next();
    l.append(QString::number(it.key()) + "=" + it.value());
  }
  s = l.join("|");
}

void COSettings::parse (QString &d)
{
  settings.clear();

  QStringList l = d.split("|");

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QStringList l2 = l[loop].split("=");
    if (l2.count() > 1)
      settings.insert((COParm) l2[0].toInt(), l2[1]);
  }
}

void COSettings::getData (COParm k, int &d)
{
  if (settings.contains(k))
    d = settings.value(k).toInt();
}

void COSettings::setData (COParm k, int d)
{
  settings.insert(k, QString::number(d));
}

void COSettings::getData (COParm k, double &d)
{
  if (settings.contains(k))
    d = settings.value(k).toDouble();
}

void COSettings::setData (COParm k, double d)
{
  settings.insert(k, QString::number(d));
}

void COSettings::getData (COParm k, QColor &d)
{
  if (settings.contains(k))
    d.setNamedColor(settings.value(k));
}

void COSettings::setData (COParm k, QColor &d)
{
  settings.insert(k, d.name());
}

void COSettings::getData (COParm k, QDateTime &d)
{
  if (settings.contains(k))
    d = QDateTime::fromString(settings.value(k), dateFormat);
}

void COSettings::setData (COParm k, QDateTime &d)
{
  settings.insert(k, d.toString(dateFormat));
}

void COSettings::getData (COParm, QFont &d)
{
  d = font;
}

void COSettings::setData (COParm, QFont &d)
{
  font = d;
}

void COSettings::getData (COParm k, QString &d)
{
  d.clear();

  switch (k)
  {
    case COID:
      d = id;
      break;
    case COSymbol:
      d = symbol;
      break;
    case COIndicator:
      d = indicator;
      break;
    case COCOType:
      d = type;
      break;
    default:
      if (settings.contains(k))
        d = settings.value(k);
      break;
  };
}

void COSettings::setData (COParm k, QString &d)
{
  switch (k)
  {
    case COID:
      id = d;
      break;
    case COSymbol:
      symbol = d;
      break;
    case COIndicator:
      indicator = d;
      break;
    case COCOType:
      type = d;
      break;
    case COLabel:
    case COBarField:
      settings.insert(k, d);
      break;
    default:
      break;
  };
}

void COSettings::newObject (int o)
{
  Config config;
  QColor color;

  QString d = QString::number(o);
  setData(COCOType, d);

  switch ((COType) o)
  {
    case COSettings::COBuyArrow:
      config.getBaseData((int) Config::DefaultBuyArrowColor, color);
      setData(COColor, color);
      break;
    case COSettings::COFiboLine:
      config.getBaseData((int) Config::DefaultFiboLineColor, color);
      setData(COColor, color);

      setData(COLine1, config.getDouble(Config::DefaultFiboLine1));
      setData(COLine2, config.getDouble(Config::DefaultFiboLine2));
      setData(COLine3, config.getDouble(Config::DefaultFiboLine3));
      setData(COLine4, config.getDouble(Config::DefaultFiboLine4));
      setData(COLine5, config.getDouble(Config::DefaultFiboLine5));
      setData(COLine6, config.getDouble(Config::DefaultFiboLine6));

      setData(COExtend, 0);
      break;
    case COSettings::COHorizontalLine:
      config.getBaseData((int) Config::DefaultHorizontalLineColor, color);
      setData(COColor, color);
      break;
    case COSettings::COSellArrow:
      config.getBaseData((int) Config::DefaultSellArrowColor, color);
      setData(COColor, color);
      break;
    case COSettings::COText:
      config.getBaseData((int) Config::DefaultTextColor, color);
      setData(COColor, color);

      config.getBaseData((int) Config::DefaultTextFont, font);
      setData(COFont, font);

      config.getData(Config::DefaultTextLabel, d);
      setData(COLabel, d);
      break;
    case COSettings::COTrendLine:
      config.getBaseData((int) Config::DefaultTrendLineColor, color);
      setData(COColor, color);

      config.getData(Config::DefaultTrendLineBar, d);
      setData(COBarField, d);

      setData(COUseBar, config.getInt(Config::DefaultTrendLineUseBar));

      setData(COExtend, config.getInt(Config::DefaultTrendLineExtend));
      break;
    case COSettings::COVerticalLine:
      config.getBaseData((int) Config::DefaultVerticalLineColor, color);
      setData(COColor, color);
      break;
    default:
      break;
  }
}

void COSettings::setSaveFlag (int d)
{
  saveFlag = d;
}

int COSettings::getSaveFlag ()
{
  return saveFlag;
}

void COSettings::setSelected (int d)
{
  selected = d;
}

int COSettings::getSelected ()
{
  return selected;
}

void COSettings::clearGrabHandles ()
{
  qDeleteAll(grabHandles);
  grabHandles.clear();
}

void COSettings::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void COSettings::clearSelectionArea ()
{
  qDeleteAll(selectionArea);
  selectionArea.clear();
}

void COSettings::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

int COSettings::isSelected (QPoint &point)
{
  int loop;
  for (loop = 0; loop < (int) selectionArea.count(); loop++)
  {
    QRegion *r = selectionArea.at(loop);
    if (r->contains(point))
      return 1;
  }

  return 0;
}

int COSettings::isGrabSelected (QPoint &point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return loop + 1;
  }

  return 0;
}

void COSettings::getHighLow (double &high, double &low)
{
  double td = 0;
  QString s;
  getData(COCOType, s);
  switch ((COType) s.toInt())
  {
    case COSettings::COBuyArrow:
    case COSettings::COSellArrow:
    case COSettings::COHorizontalLine:
    case COSettings::COText:
      getData(COValue, high);
      getData(COValue, low);
      break;
    case COSettings::COFiboLine:
      getData(COHigh, high);
      getData(COLow, low);
      break;
    case COSettings::COTrendLine:
      high = -99999999.0;
      low = 99999999.0;

      getData(COValue, td);
      if (td > high)
        high = td;
      if (td < low)
        low = td;

      getData(COValue2, td);
      if (td > high)
        high = td;
      if (td < low)
        low = td;
      break;
    default:
      break;
  }
}

void COSettings::getInfo (Setting &info)
{
  QString k, d;
  getData(COCOType, d);
  switch ((COType) d.toInt())
  {
    case COSettings::COBuyArrow:
      k = QObject::tr("Type");
      d = QObject::tr("Buy Arrow");
      info.setData(k, d);

      k = QObject::tr("Date");
      getData(CODate, d);
      info.setData(k, d);

      k = QObject::tr("Price");
      getData(COValue, d);
      info.setData(k, d);
      break;
    case COSettings::COFiboLine:
      k = QObject::tr("Type");
      d = QObject::tr("Fibo Line");
      info.setData(k, d);

      k = QObject::tr("High Point");
      getData(COHigh, d);
      info.setData(k, d);

      k = QObject::tr("Low Point");
      getData(COLow, d);
      info.setData(k, d);

      k = QObject::tr("Line 1");
      getData(COLine1, d);
      info.setData(k, d);

      k = QObject::tr("Line 2");
      getData(COLine2, d);
      info.setData(k, d);

      k = QObject::tr("Line 3");
      getData(COLine3, d);
      info.setData(k, d);

      k = QObject::tr("Line 4");
      getData(COLine4, d);
      info.setData(k, d);

      k = QObject::tr("Line 5");
      getData(COLine5, d);
      info.setData(k, d);

      k = QObject::tr("Line 6");
      getData(COLine6, d);
      info.setData(k, d);
      break;
    case COSettings::COHorizontalLine:
      k = QObject::tr("Type");
      d = QObject::tr("Horizontal Line");
      info.setData(k, d);

      k = QObject::tr("Date");
      getData(CODate, d);
      info.setData(k, d);

      k = QObject::tr("Price");
      getData(COValue, d);
      info.setData(k, d);
      break;
    case COSettings::COSellArrow:
      k = QObject::tr("Type");
      d = QObject::tr("Sell Arrow");
      info.setData(k, d);

      k = QObject::tr("Date");
      getData(CODate, d);
      info.setData(k, d);

      k = QObject::tr("Price");
      getData(COValue, d);
      info.setData(k, d);
      break;
    case COSettings::COText:
      k = QObject::tr("Type");
      d = QObject::tr("Text");
      info.setData(k, d);

      k = QObject::tr("Label");
      getData(COLabel, d);
      info.setData(k, d);
      break;
    case COSettings::COTrendLine:
      k = QObject::tr("Type");
      d = QObject::tr("Trend Line");
      info.setData(k, d);

      k = QObject::tr("Start Date");
      getData(CODate, d);
      info.setData(k, d);

      k = QObject::tr("Start Price");
      getData(COValue, d);
      info.setData(k, d);

      k = QObject::tr("End Date");
      getData(CODate2, d);
      info.setData(k, d);

      k = QObject::tr("End Price");
      getData(COValue2, d);
      info.setData(k, d);
      break;
    case COSettings::COVerticalLine:
      k = QObject::tr("Type");
      d = QObject::tr("Vertical Line");
      info.setData(k, d);

      k = QObject::tr("Date");
      getData(CODate, d);
      info.setData(k, d);
      break;
    default:
      break;
  }

}

