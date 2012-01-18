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

#include "ChartObject.h"
#include "EAVDataBase.h"
#include "Plot.h"
#include "ChartObjectKey.h"
#include "EntityType.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QUuid>

ChartObject::ChartObject ()
{
  _status = _NONE;
  _selected = 0;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);
  _modified = 0;
  
  ChartObjectKey keys;
  _settings.setType(EntityType::_CHART_OBJECT);
  _settings.set(keys.indexToString(ChartObjectKey::_RO), Data(FALSE));
  _settings.set(keys.indexToString(ChartObjectKey::_Z), Data(1));
  _settings.set(keys.indexToString(ChartObjectKey::_PEN), Data(1));
  
  QString id;
  newID(id);
  _settings.setName(id);
  _settings.set(keys.indexToString(ChartObjectKey::_ID), Data(id));
}

ChartObject::~ChartObject ()
{
  detach();
  save();
}

int ChartObject::rtti () const
{
  return Rtti_PlotUserItem;
}

int ChartObject::isSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _selectionArea.count(); loop++)
  {
    QRegion r = _selectionArea.at(loop);
    if (r.contains(p))
      return 1;
  }

  return 0;
}

int ChartObject::isGrabSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _grabHandles.count(); loop++)
  {
    QRegion r = _grabHandles.at(loop);
    if (r.contains(p))
      return loop + 1;
  }

  return 0;
}

void ChartObject::setSelected (int d)
{
  _selected = d;
}

void ChartObject::setSettings (Entity d)
{
  _settings = d;
}

Entity ChartObject::settings ()
{
  return _settings;
}

// virtual
int ChartObject::info (Entity &)
{
  return 0;
}

// virtual
int ChartObject::highLow (int start, int end, double &high, double &low)
{
  double h = 0;
  double l = 0;
  int flag = 0;

  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  ChartObjectKey keys;
  Data tddt;
  _settings.toData(keys.indexToString(ChartObjectKey::_DATE), tddt);
  int x = dsd->x(tddt.toDateTime());

  if (x < start || x > end)
    return 1;

  Data tdd;
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), tdd);
  double d = tdd.toDouble();
  if (! flag)
  {
    h = d;
    l = d;
    flag++;
  }
  else
  {
    if (d > h)
      h = d;
    if (d < l)
      l = d;
  }

  if (! flag)
    return 1;

  high = h;
  low = l;

  return 0;
}

// virtual
void ChartObject::move (QPoint)
{
}

// virtual
void ChartObject::click (int button, QPoint p)
{
  switch (_status)
  {
    case _SELECTED:
    {
      switch (button)
      {
        case Qt::LeftButton:
          if (isGrabSelected(p))
          {
            _status = _MOVE;
            return;
          }

          if (! isSelected(p))
          {
            _status = _NONE;
            setSelected(FALSE);

	    Plot *tplot = (Plot *) plot();
	    tplot->unselect();

            plot()->replot();
            return;
          }
          break;
        case Qt::RightButton:
	{
          Plot *p = (Plot *) plot();
	  p->showChartObjectMenu();
          break;
	}
        default:
          break;
      }

      break;
    }
    case _MOVE:
    {
      switch (button)
      {
        case Qt::LeftButton:
          _status = _SELECTED;
          return;
        default:
          break;
      }

      break;
    }
    default: // _None
    {
      switch (button)
      {
        case Qt::LeftButton:
	{
          if (isSelected(p))
          {
            _status = _SELECTED;
            _selected = 1;

	    Plot *tplot = (Plot *) plot();
	    
	    ChartObjectKey keys;
	    Data td;
	    _settings.toData(keys.indexToString(ChartObjectKey::_ID), td);
	    tplot->select(td.toString());

            plot()->replot();
            return;
          }
          break;
	}
        default:
          break;
      }

      break;
    }
  }
}

// virtual
int ChartObject::create ()
{
  return 0;
}

int ChartObject::save ()
{
  ChartObjectKey keys;
  Data td;
  _settings.toData(keys.indexToString(ChartObjectKey::_RO), td);
  if (td.toBool())
    return 1;

  if (! _modified)
    return 1;

  EAVDataBase db("chartObjects");
  db.transaction();
  if (db.set(&_settings))
    return 1;
  db.commit();

  _modified = 0;

  return 0;
}

void ChartObject::setScript (QString d)
{
  _script = d;
}

QString ChartObject::script ()
{
  return _script;
}

void ChartObject::setModified (int d)
{
  _modified = d;
}

int ChartObject::modified ()
{
  return _modified;
}

void ChartObject::newID (QString &d)
{
  d = QUuid::createUuid().toString();
  d.remove("-");
  d.remove("{");
  d.remove("}");
}
