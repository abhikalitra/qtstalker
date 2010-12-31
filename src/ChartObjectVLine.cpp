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


#include "ChartObjectVLine.h"
#include "DateScaleDraw.h"
#include "ChartObjectVLineDraw.h"
#include "Strip.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QSettings>

ChartObjectVLine::ChartObjectVLine ()
{
  _draw = new ChartObjectVLineDraw;
  _draw->setSettings(_settings);

  QSettings set(g_settingsFile);
  _settings->setData("Color", set.value("default_chart_object_vline_color", "red").toString());
  _settings->setData("Type", QString("VLine"));
}

void ChartObjectVLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("VLine"));
  
  QDateTime dt = _settings->dateTime("Date");
  info.setData(QObject::tr("D"), dt.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), dt.toString("HH:mm:ss"));
}

int ChartObjectVLine::highLow (int, int, double &, double &)
{
  return 0;
}

void ChartObjectVLine::move (QPoint p)
{
  switch (_status)
  {
    case _Move:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      _settings->setData("Date", dt);
      
      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectVLine::click (int button, QPoint p)
{
  if (_settings->getInt("RO"))
    return;

  switch (_status)
  {
    case _Selected:
    {
      switch (button)
      {
        case Qt::LeftButton:
          if (_draw->isGrabSelected(p))
          {
            _status = _Move;
            emit signalMoveStart(_settings->data("ID"));
            return;
          }

          if (! _draw->isSelected(p))
          {
            _status = _None;
            _draw->setSelected(FALSE);
            emit signalUnselected(_settings->data("ID"));
            _draw->plot()->replot();
            return;
          }
          break;
        case Qt::RightButton:
          _editAction->setText(tr("Edit") + " " + _settings->data("ID"));
          _deleteAction->setText(tr("Delete") + " " + _settings->data("ID"));
          _menu->exec(QCursor::pos());
          break;
        default:
          break;
      }

      break;
    }
    case _Move:
    {
      switch (button)
      {
        case Qt::LeftButton:
          _status = _Selected;
          emit signalMoveEnd(_settings->data("ID"));
          save();
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
          if (_draw->isSelected(p))
          {
            _status = _Selected;
            _draw->setSelected(TRUE);
            emit signalSelected(_settings->data("ID"));
            _draw->plot()->replot();
          }
          break;
        default:
          break;
      }

      break;
    }
  }
}

void ChartObjectVLine::create ()
{
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->data("ID"));
  emit signalMoveStart(_settings->data("ID"));
}
