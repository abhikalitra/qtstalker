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


#include "ChartObjectHLine.h"
#include "DateScaleDraw.h"
#include "Strip.h"
#include "Globals.h"

#include <QDebug>
#include <QSettings>

ChartObjectHLine::ChartObjectHLine ()
{
  _draw = new ChartObjectHLineDraw;
  _draw->setSettings(_settings);
  _dialog = 0;
  
  QSettings set(g_globalSettings);
  QString s = set.value("default_chart_object_hline_color", "red").toString();
  _settings->setData("COLOR", s);
  _settings->setData("TYPE", QString("HLine"));
}

ChartObjectHLine::~ChartObjectHLine ()
{
  delete _draw;
  if (_dialog)
    delete _dialog;
}

void ChartObjectHLine::info (Setting &info)
{
  info.setData(tr("Type"), tr("HLine"));
  info.setData(tr("Price"), _settings->data("PRICE"));
}

int ChartObjectHLine::highLow (int, int, double &h, double &l)
{
  h = _settings->getDouble("PRICE");
  l = _settings->getDouble("PRICE");

  return 1;
}

void ChartObjectHLine::move (QPoint p)
{
  switch (_status)
  {
    case _Move:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings->setData("PRICE", map.invTransform((double) p.y()));
      
      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectHLine::click (int button, QPoint p)
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

void ChartObjectHLine::create ()
{
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->data("ID"));
  emit signalMoveStart(_settings->data("ID"));
  g_middleMan->statusMessage(tr("Place HLine object..."));
}

void ChartObjectHLine::dialog ()
{
  if (_dialog)
    return;

  _dialog = new ChartObjectHLineDialog(_parent, this);
  connect(_dialog, SIGNAL(accepted()), this, SLOT(update()));
  connect(_dialog, SIGNAL(finished(int)), this, SLOT(dialogDone()));
  _dialog->show();
}

void ChartObjectHLine::dialogDone ()
{
  _dialog = 0;
}

int ChartObjectHLine::isSelected (QPoint p)
{
  return _draw->isSelected(p);
}

void ChartObjectHLine::setZ (int d)
{
  _draw->setZ(d);
}

void ChartObjectHLine::attach (QwtPlot *p)
{
  _draw->attach(p);
}
