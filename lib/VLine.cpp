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


#include "VLine.h"
#include "PrefDialog.h"
#include "Config.h"
#include "CODataBase.h"

#include "../pics/vertical.xpm"

#include <QObject>
#include <QPainter>

VLine::VLine ()
{
  _plugin = "VLine";
  _color.setNamedColor("red");
}

void VLine::draw (PlotData &pd, DateBar &dateBars, Scaler &)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = dateBars.getX(_date);
  if (x2 == -1)
    return;

  int x = (x2 * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
  if (x == -1)
    return;

  painter.setPen(_color);

  painter.drawLine (x, 0, x, pd.buffer.height());

  clearSelectionArea();
  QPolygon array;
  array.putPoints(0,
  	          4,
		  x - (_handleWidth / 2), 0,
		  x + (_handleWidth / 2), 0,
		  x + (_handleWidth / 2), pd.buffer.height(),
		  x - (_handleWidth / 2), pd.buffer.height());
  setSelectionArea(new QRegion(array));

  if (_selected)
  {
    clearGrabHandles();
    int t = (int) pd.buffer.height() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      setGrabHandle(new QRegion(x - (_handleWidth / 2),
		    t * loop,
		    _handleWidth,
		    _handleWidth,
		    QRegion::Rectangle));

      painter.fillRect(x - (_handleWidth / 2),
		       t * loop,
		       _handleWidth,
		       _handleWidth,
		       _color);
    }
  }

  painter.end();
}

void VLine::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = "VLine";
  info->setData(k, d);

  k = QObject::tr("Date");
  d = _date.toString(Qt::ISODate);
  info->setData(k, d);
}

void VLine::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit VLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, _color);

  int def = FALSE;
  s = QObject::tr("Default");
  dialog->addCheckItem(pid++, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pid = 0;
  dialog->getColor(pid++, _color);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultVLineColor, _color);
  }

  _saveFlag = TRUE;
  
  delete dialog;
}

void VLine::load (QSqlQuery &q)
{
  _id = q.value(0).toInt();
  _exchange = q.value(1).toString();
  _symbol = q.value(2).toString();
  _indicator = q.value(3).toString();
  _color.setNamedColor(q.value(5).toString()); // t1 field
  _date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate); // t2 field
}

void VLine::save ()
{
  if (! _saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,t2) VALUES (";
  s.append(QString::number(_id));
  s.append(",'" + _exchange + "'");
  s.append(",'" + _symbol + "'");
  s.append(",'" + _indicator + "'");
  s.append(",'" + _plugin + "'");
  s.append(",'" + _color.name() + "'");
  s.append(",'" + _date.toString(Qt::ISODate) + "'");
  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  _saveFlag = FALSE;
}

void VLine::create ()
{
  emit signalMessage(QString(QObject::tr("Select point to place VLine...")));
}

int VLine::create2 (QDateTime &x, double)
{
  _saveFlag = TRUE;
  _date = x;
  emit signalMessage(QString());
  return 0;
}

void VLine::moving (QDateTime &x, double, int)
{
  _saveFlag = TRUE;
  _date = x;
  
  emit signalMessage(QString(x.toString(Qt::ISODate)));
}

void VLine::getIcon (QIcon &d)
{
  d = QIcon(vertical_xpm);
}

int VLine::getHighLow (double &, double &)
{
  return 1;
}

int VLine::inDateRange (QDateTime &startDate, QDateTime &endDate, DateBar &)
{
  int rc = FALSE;
  if (_date >= startDate && _date <= endDate)
    rc = TRUE;
  
  return rc;
}

int VLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<COLOR>
  //  0    1      2      3

  if (l.count() != 4)
  {
    qDebug() << _plugin << "::CUS: invalid parm count" << l.count();
    return 1;
  }

  _date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _date.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid date" << l[2];
    return 1;
  }

  _color.setNamedColor(l[3]);
  if (! _color.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid color" << l[3];
    return 1;
  }

  return 0;
}

