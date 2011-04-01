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


#include "ChartObjectRetracement.h"
#include "DateScaleDraw.h"
#include "Strip.h"
#include "Globals.h"

#include <QDebug>
#include <QSettings>

ChartObjectRetracement::ChartObjectRetracement ()
{
  _createFlag = 0;
  _draw = new ChartObjectRetracementDraw;
  _draw->setSettings(_settings);
  _dialog = 0;

  QSettings set(g_globalSettings);
  _settings->setData("COLOR", set.value("default_chart_object_retracement_color", "red").toString());
  _settings->setData("LINE1", set.value("default_chart_object_retracement_line1", 0.382).toDouble());
  _settings->setData("LINE2", set.value("default_chart_object_retracement_line2", 0.5).toDouble());
  _settings->setData("LINE3", set.value("default_chart_object_retracement_line3", 0.618).toDouble());
  _settings->setData("LINE4", set.value("default_chart_object_retracement_line4", 0).toDouble());
  _settings->setData("LINE5", set.value("default_chart_object_retracement_line5", 0).toDouble());
  _settings->setData("LINE6", set.value("default_chart_object_retracement_line6", 0).toDouble());
  _settings->setData("TYPE", QString("Retracement"));
  _settings->setData("EXTEND", 0);
}

ChartObjectRetracement::~ChartObjectRetracement ()
{
  delete _draw;
  if (_dialog)
    delete _dialog;
}

void ChartObjectRetracement::info (Setting &info)
{
  info.setData(tr("Type"), tr("Retracement"));
  
  QDateTime dt = _settings->dateTime("DATE");
  info.setData(tr("SD"), dt.toString("yyyy-MM-dd"));
  info.setData(tr("ST"), dt.toString("HH:mm:ss"));
  
  dt = _settings->dateTime("DATE2");
  info.setData(tr("ED"), dt.toString("yyyy-MM-dd"));
  info.setData(tr("ET"), dt.toString("HH:mm:ss"));
  
  info.setData(tr("High"), _settings->data("HIGH"));
  info.setData(tr("Low"), _settings->data("LOW"));
  info.setData(tr("Level 1"), _settings->data("LINE1"));
  info.setData(tr("Level 2"), _settings->data("LINE2"));
  info.setData(tr("Level 3"), _settings->data("LINE3"));
  info.setData(tr("Level 4"), _settings->data("LINE4"));
  info.setData(tr("Level 5"), _settings->data("LINE5"));
  info.setData(tr("Level 6"), _settings->data("LINE6"));
}

int ChartObjectRetracement::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings->dateTime("DATE"));
  int x2 = dsd->x(_settings->dateTime("DATE2"));

  if ((x >= start && x <= end) || ((x2 >= start && x2 <= end)))
  {
    h = _settings->getDouble("HIGH");
    l = _settings->getDouble("LOW");
    return 1;
  }

  return 0;
}

void ChartObjectRetracement::move (QPoint p)
{
  switch (_status)
  {
    case _Move:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());
      if (x < 0)
        break;

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      if (! dt.isValid())
        break;
      _settings->setData("DATE", dt);

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings->setData("HIGH", map.invTransform((double) p.y()));
      
      if (_createFlag)
      {
        _settings->setData("DATE2", dt);
        _settings->setData("LOW", _settings->data("HIGH"));
      }

      _draw->plot()->replot();
      break;
    }
    case _Move2:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());
      if (x < 0)
        break;

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      if (! dt.isValid())
        break;
      _settings->setData("DATE2", dt);

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings->setData("LOW", map.invTransform((double) p.y()));
      
      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectRetracement::click (int button, QPoint p)
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
        {
          int grab = _draw->isGrabSelected(p);
          if (grab)
          {
            _status = _Move;
            if (grab == 2)
              _status = _Move2;
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
        }
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
          if (_createFlag)
          {
            _status = _Move2;
            g_middleMan->statusMessage(tr("Select lowest ending point..."));
            return;
          }

          _status = _Selected;
          emit signalMoveEnd(_settings->data("ID"));
          save();
          return;
        default:
          break;
      }

      break;
    }
    case _Move2:
    {
      switch (button)
      {
        case Qt::LeftButton:
          _status = _Selected;
          _createFlag = 0;
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

void ChartObjectRetracement::create ()
{
  _createFlag = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->data("ID"));
  emit signalMoveStart(_settings->data("ID"));
  g_middleMan->statusMessage(tr("Select highest starting point..."));
}

void ChartObjectRetracement::dialog ()
{
  if (_dialog)
    return;

  _dialog = new ChartObjectRetracementDialog(g_parent, this);
  connect(_dialog, SIGNAL(accepted()), this, SLOT(update()));
  connect(_dialog, SIGNAL(finished(int)), this, SLOT(dialogDone()));
  _dialog->show();
}

void ChartObjectRetracement::dialogDone ()
{
  _dialog = 0;
}

int ChartObjectRetracement::isSelected (QPoint p)
{
  return _draw->isSelected(p);
}

void ChartObjectRetracement::setZ (int d)
{
  _draw->setZ(d);
}

void ChartObjectRetracement::attach (QwtPlot *p)
{
  _draw->attach(p);
}
