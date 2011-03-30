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


#include "ChartObjectSell.h"
#include "DateScaleDraw.h"
#include "Strip.h"
#include "Globals.h"

#include <QDebug>
#include <QSettings>

ChartObjectSell::ChartObjectSell ()
{
  _draw = new ChartObjectSellDraw;
  _draw->setSettings(_settings);
  _dialog = 0;

  QSettings set(g_globalSettings);
  _settings->setData("COLOR", set.value("default_chart_object_sell_color", "red").toString());
  _settings->setData("TYPE", QString("Sell"));
}

ChartObjectSell::~ChartObjectSell ()
{
  delete _draw;
  if (_dialog)
    delete _dialog;
}

void ChartObjectSell::info (Setting &info)
{
  info.setData(tr("Type"), tr("Sell"));

  QDateTime dt = _settings->dateTime("DATE");
  info.setData(tr("D"), dt.toString("yyyy-MM-dd"));
  info.setData(tr("T"), dt.toString("HH:mm:ss"));

  info.setData(tr("Price"), _settings->data("PRICE"));
}

int ChartObjectSell::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings->dateTime("DATE"));

  if (x < start || x > end)
    return 0;

  h = _settings->getDouble("PRICE");
  l = _settings->getDouble("PRICE");

  return 1;
}

void ChartObjectSell::move (QPoint p)
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

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings->setData("PRICE", map.invTransform((double) p.y()));
      
      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectSell::click (int button, QPoint p)
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

void ChartObjectSell::create ()
{
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->data("ID"));
  emit signalMoveStart(_settings->data("ID"));
  g_middleMan->statusMessage(tr("Place Sell object..."));
}

void ChartObjectSell::dialog ()
{
  if (_dialog)
    return;

  _dialog = new ChartObjectSellDialog(_parent, this);
  connect(_dialog, SIGNAL(accepted()), this, SLOT(update()));
  connect(_dialog, SIGNAL(finished(int)), this, SLOT(dialogDone()));
  _dialog->show();
}

void ChartObjectSell::dialogDone ()
{
  _dialog = 0;
}

int ChartObjectSell::isSelected (QPoint p)
{
  return _draw->isSelected(p);
}

void ChartObjectSell::setZ (int d)
{
  _draw->setZ(d);
}

void ChartObjectSell::attach (QwtPlot *p)
{
  _draw->attach(p);
}
