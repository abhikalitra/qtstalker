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
#include "Strip.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QSettings>

ChartObjectVLine::ChartObjectVLine ()
{
  _draw = new ChartObjectVLineDraw;
  _draw->setSettings(_settings);
  _dialog = 0;

  QSettings set(g_globalSettings);
  _settings->setData("COLOR", set.value("default_vline_color", "red").toString());
  _settings->setData("TYPE", QString("VLine"));
  _settings->setData("PLUGIN", QString("VLINE"));
  _settings->setData("DATE", QDateTime::currentDateTime());
  _settings->setData("Z", 99);
}

ChartObjectVLine::~ChartObjectVLine ()
{
  delete _draw;
  if (_dialog)
    delete _dialog;
}

void ChartObjectVLine::info (Setting &info)
{
  info.setData(tr("Type"), tr("VLine"));
  
  QDateTime dt = _settings->dateTime("DATE");
  info.setData(tr("D"), dt.toString("yyyy-MM-dd"));
  info.setData(tr("T"), dt.toString("HH:mm:ss"));
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
      _settings->setData("DATE", dt);
      
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
	  if (! _dialog)
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
  g_middleMan->statusMessage(tr("Place VLine object..."));
}

void ChartObjectVLine::dialog ()
{
  if (_dialog)
    return;

  _dialog = new ChartObjectDialog(g_parent, this);
  connect(_dialog, SIGNAL(accepted()), this, SLOT(update()));
  connect(_dialog, SIGNAL(finished(int)), this, SLOT(dialogDone()));
  _dialog->show();
}

void ChartObjectVLine::dialogDone ()
{
  _dialog = 0;
}

int ChartObjectVLine::isSelected (QPoint p)
{
  return _draw->isSelected(p);
}

void ChartObjectVLine::setZ (int d)
{
  _draw->setZ(d);
}

void ChartObjectVLine::attach (QwtPlot *p)
{
  _draw->attach(p);
}
