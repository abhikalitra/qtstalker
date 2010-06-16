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


#include "Retracement.h"
#include "PrefDialog.h"
#include "Config.h"
#include "CODataBase.h"

#include "../pics/fib.xpm"

#include <QObject>
#include <QDebug>
#include <QPainter>

Retracement::Retracement ()
{
  _plugin = "Retracement";
  _color.setNamedColor("red");
  _high = 0;
  _low = 0;
  _extend = 0;

  Config config;
  config.getData(Config::DefaultRetracementColor, _color);
  if (! _color.isValid())
  {
    _color.setNamedColor("red");
    config.setData(Config::DefaultRetracementColor, _color);
  }

  _line1 = config.getDouble(Config::DefaultRetracementLine1);
  if (! _line1)
  {
    _line1 = 0.382;
    config.setData(Config::DefaultRetracementLine1, _line1);
  }

  _line2 = config.getDouble(Config::DefaultRetracementLine2);
  if (! _line2)
  {
    _line2 = 0.5;
    config.setData(Config::DefaultRetracementLine2, _line2);
  }

  _line3 = config.getDouble(Config::DefaultRetracementLine3);
  if (! _line3)
  {
    _line3 = 0.618;
    config.setData(Config::DefaultRetracementLine3, _line3);
  }

  _line4 = config.getDouble(Config::DefaultRetracementLine4);
  _line5 = config.getDouble(Config::DefaultRetracementLine5);
  _line6 = config.getDouble(Config::DefaultRetracementLine6);
}

void Retracement::draw (PlotData &pd, DateBar &dateBars, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  painter.setFont(pd.plotFont);

  int x = dateBars.getX(_date);
  if (x == -1)
    return;

  x = (x * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
  if (x == -1)
    return;

  QDateTime dt = _date2;

  if (_extend)
    dateBars.getDate(dateBars.count() - 1, dt);

  int x2 = dateBars.getX(dt);
  if (x2 == -1)
    return;

  x2 = (x2 * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
  if (x2 == -1)
    return;

  painter.setPen(_color);

  QPolygon array;
  clearSelectionArea();

  QList<double> lineList;
  lineList.append(_line1);
  lineList.append(_line2);
  lineList.append(_line3);
  lineList.append(_line4);
  lineList.append(_line5);
  lineList.append(_line6);

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = _high - _low;
      double r = 0;
      if (td < 0)
        r = _low + (range * td);
      else
      {
        if (td > 0)
          r = _low + (range * td);
        else
        {
          if (td < 0)
            r = _high;
          else
            r = _low;
        }
      }
      int y = scaler.convertToY(r);
      painter.drawLine (x, y, x2, y);
      painter.drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      setSelectionArea(new QRegion(array));
    }
  }
  
  // draw the low line
  int y = scaler.convertToY(_low);
  painter.drawLine (x, y, x2, y);
  painter.drawText(x, y - 1, "0% - " + QString::number(_low));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  setSelectionArea(new QRegion(array));

  // draw the high line
  int y2 = scaler.convertToY(_high);
  painter.drawLine (x, y2, x2, y2);
  painter.drawText(x, y2 - 1, "100% - " + QString::number(_high));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));
  
  if (_selected)
  {
    clearGrabHandles();

    //top left corner
    y = scaler.convertToY(_high);
    setGrabHandle(new QRegion(x, y - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x,
		     y - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);

    //bottom right corner
    x2 = (dateBars.getX(_date2) * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
    y2 = scaler.convertToY(_low);
    setGrabHandle(new QRegion(x2, y2 - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x2,
		     y2 - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);
  }

  painter.end();
}

void Retracement::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Retracement");
  info->setData(k, d);

  k = QObject::tr("High Point");
  d = QString::number(_high);
  info->setData(k, d);

  k = QObject::tr("Low Point");
  d = QString::number(_low);
  info->setData(k, d);

  k = QObject::tr("Line 1");
  d = QString::number(_line1);
  info->setData(k, d);

  k = QObject::tr("Line 2");
  d = QString::number(_line2);
  info->setData(k, d);

  k = QObject::tr("Line 3");
  d = QString::number(_line3);
  info->setData(k, d);

  k = QObject::tr("Line 4");
  d = QString::number(_line4);
  info->setData(k, d);

  k = QObject::tr("Line 5");
  d = QString::number(_line5);
  info->setData(k, d);

  k = QObject::tr("Line 6");
  d = QString::number(_line6);
  info->setData(k, d);
}

void Retracement::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit Retracement");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, _color);

  s = QObject::tr("High Point");
  dialog->addDoubleItem(pid++, page, s, _high);

  s = QObject::tr("Low Point");
  dialog->addDoubleItem(pid++, page, s, _low);

  s = QObject::tr("Extend");
  dialog->addCheckItem(pid++, page, s, _extend);

  int def = FALSE;
  s = QObject::tr("Default");
  dialog->addCheckItem(pid++, page, s, def);

  page++;
  s = QObject::tr("Lines");
  dialog->addPage(page, s);

  s = QObject::tr("Line 1");
  dialog->addDoubleItem(pid++, page, s, _line1);

  s = QObject::tr("Line 2");
  dialog->addDoubleItem(pid++, page, s, _line2);

  s = QObject::tr("Line 3");
  dialog->addDoubleItem(pid++, page, s, _line3);

  s = QObject::tr("Line 4");
  dialog->addDoubleItem(pid++, page, s, _line4);

  s = QObject::tr("Line 5");
  dialog->addDoubleItem(pid++, page, s, _line5);

  s = QObject::tr("Line 6");
  dialog->addDoubleItem(pid++, page, s, _line6);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pid = 0;
  dialog->getColor(pid++, _color);
  _high = dialog->getDouble(pid++);
  _low = dialog->getDouble(pid++);
  _extend = dialog->getCheck(pid++);
  def = dialog->getCheck(pid++);
  _line1 = dialog->getDouble(pid++);
  _line2 = dialog->getDouble(pid++);
  _line3 = dialog->getDouble(pid++);
  _line4 = dialog->getDouble(pid++);
  _line5 = dialog->getDouble(pid++);
  _line6 = dialog->getDouble(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultRetracementColor, _color);
    config.setData((int) Config::DefaultRetracementLine1, _line1);
    config.setData((int) Config::DefaultRetracementLine2, _line2);
    config.setData((int) Config::DefaultRetracementLine3, _line3);
    config.setData((int) Config::DefaultRetracementLine4, _line4);
    config.setData((int) Config::DefaultRetracementLine5, _line5);
    config.setData((int) Config::DefaultRetracementLine6, _line6);
  }

  _saveFlag = TRUE;
  
  delete dialog;
}

void Retracement::load (QSqlQuery &q)
{
  _id = q.value(0).toInt();
  _exchange = q.value(1).toString();
  _symbol = q.value(2).toString();
  _indicator = q.value(3).toString();
  _color.setNamedColor(q.value(5).toString()); // t1 field
  _date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate); // t2 field
  _date2 = QDateTime::fromString(q.value(7).toString(), Qt::ISODate); // t3 field
  _high = q.value(25).toDouble(); // d1 field
  _low = q.value(26).toDouble(); // d2 field
  _line1 = q.value(27).toDouble(); // d3 field
  _line2 = q.value(28).toDouble(); // d4 field
  _line3 = q.value(29).toDouble(); // d5 field
  _line4 = q.value(30).toDouble(); // d6 field
  _line5 = q.value(31).toDouble(); // d7 field
  _line6 = q.value(32).toDouble(); // d8 field
  _extend = q.value(15).toInt(); // i1 field
}

void Retracement::save ()
{
  if (! _saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,d1,d2,d3,d4,d5,d6,d7,d8,i1,t2,t3) VALUES (";
  s.append(QString::number(_id));
  s.append(",'" + _exchange + "'");
  s.append(",'" + _symbol + "'");
  s.append(",'" + _indicator + "'");
  s.append(",'" + _plugin + "'");
  s.append(",'" + _color.name() + "'");
  s.append("," + QString::number(_high));
  s.append("," + QString::number(_low));
  s.append("," + QString::number(_line1));
  s.append("," + QString::number(_line2));
  s.append("," + QString::number(_line3));
  s.append("," + QString::number(_line4));
  s.append("," + QString::number(_line5));
  s.append("," + QString::number(_line6));
  s.append("," + QString::number(_extend));
  s.append(",'" + _date.toString(Qt::ISODate) + "'");
  s.append(",'" + _date2.toString(Qt::ISODate) + "'");
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  _saveFlag = FALSE;
}

void Retracement::create ()
{
  _saveFlag = TRUE;
  emit signalMessage(QString(QObject::tr("Select Retracement high point...")));
}

int Retracement::create2 (QDateTime &x, double y)
{
  _date = x;
  _high = y;
  emit signalMessage(QString(QObject::tr("Select Retracement low point...")));
  
  return 1;
}

int Retracement::create3 (QDateTime &x, double y)
{
  if (x < _date)
    return 1;

  if (y > _high)
    return 1;

  _date2 = x;
  _low = y;

  emit signalMessage(QString());

  return 0;
}

void Retracement::moving (QDateTime &x, double y, int moveFlag)
{
  switch (moveFlag)
  {
    case 1: // top left corner
    {
      if (x > _date2)
        return;
      
      if (y < _low)
        return;
      
      _date = x;
      _high = y;
      _saveFlag = TRUE;
      
      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
    case 2:
    default: // bottom right corner
    {
      if (x < _date)
        return;
      
      if (y > _high)
        return;
      
      _date2 = x;
      _low = y;
      _saveFlag = TRUE;

      emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
      break;
    }
  }
}

void Retracement::getIcon (QIcon &d)
{
  d = QIcon(fib_xpm);
}

int Retracement::inDateRange (QDateTime &startDate, QDateTime &endDate, DateBar &dateBars)
{
  int rc = FALSE;
  
  QDateTime dt = _date2;
  if (_extend)
    dateBars.getDate(dateBars.count() - 1, dt);
  
  // is start past our end?
  if (startDate > dt)
    return rc;
  
  // is end before our start?
  if (endDate < _date)
    return rc;
  
  if (startDate >= _date && startDate <= dt)
    return TRUE;
  
  if (endDate >= _date && endDate <= dt)
    return TRUE;
  
  return rc;
}

int Retracement::getHighLow (double &h, double &l)
{
  h = _high;
  l = _low;
  return 0;
}

int Retracement::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<DATE2>,<HIGH>,<LOW>,<PERCENTAGE>,<COLOR>
  //  0    1      2      3      4      5      6       7

  if (l.count() != 8)
  {
    qDebug() << _plugin << "::CUS: invalid parm count" << l.count();
    return 1;
  }

  _date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _date.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid start date" << l[2];
    return 1;
  }

  _date2 = QDateTime::fromString(l[3], Qt::ISODate);
  if (! _date2.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid end date" << l[3];
    return 1;
  }

  bool ok;
  _high = l[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::CUS: invalid high" << l[4];
    return 1;
  }

  _low = l[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::CUS: invalid low" << l[5];
    return 1;
  }

  _line1 = l[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::CUS: invalid retracement" << l[6];
    return 1;
  }

  _color.setNamedColor(l[7]);
  if (! _color.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid color" << l[7];
    return 1;
  }

  _line2 = 0;
  _line3 = 0;
  _line4 = 0;
  _line5 = 0;
  _line6 = 0;

  return 0;
}

