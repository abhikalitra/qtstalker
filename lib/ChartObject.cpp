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
#include "ConfirmDialog.h"
#include "ChartObjectDataBase.h"
#include "DateScaleDraw.h"
#include "Plot.h"

#include <QDebug>
#include <QPolygon>
#include <QSettings>
#include <QObject>
#include <qwt_plot.h>

ChartObject::ChartObject ()
{
  _status = _NONE;
  _selected = 0;
  _handleWidth = 6;
  _readOnly = FALSE;
  setYAxis(QwtPlot::yRight);
  _settings = new SettingGroup;
  _modified = 0;
}

ChartObject::~ChartObject ()
{
  detach();
  save();
  delete _settings;
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

SettingGroup * ChartObject::settings ()
{
  return _settings;
}

// virtual
int ChartObject::info (Message &)
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

  Setting *set = _settings->get("DATE");
  if (! set)
    return 1;

  int x = dsd->x(set->getDateTime());

  if (x < start || x > end)
    return 1;

  set = _settings->get("PRICE");
  if (! set)
    return 1;

  double d = set->getDouble();
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
	    tplot->select(_id);

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
  if (_readOnly)
    return 1;

  if (! _modified)
    return 1;

  ChartObjectDataBase db;
  db.transaction();
  if (db.save(this))
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

void ChartObject::setSymbol (QString d)
{
  _symbol = d;
}

QString ChartObject::symbol ()
{
  return _symbol;
}

void ChartObject::setID (QString d)
{
  _id = d;
}

QString ChartObject::ID ()
{
  return _id;
}

QString ChartObject::type ()
{
  return _type;
}

void ChartObject::setReadOnly (bool d)
{
  _readOnly = d;
}

bool ChartObject::readOnly ()
{
  return _readOnly;
}

void ChartObject::setPlotName (QString d)
{
  _plotName = d;
}

QString ChartObject::plotName ()
{
  return _plotName;
}

void ChartObject::setModified (int d)
{
  _modified = d;
}

int ChartObject::modified ()
{
  return _modified;
}
