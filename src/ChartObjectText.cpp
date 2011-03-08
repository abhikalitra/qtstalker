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


#include "ChartObjectText.h"
#include "DateScaleDraw.h"
#include "ChartObjectTextDraw.h"
#include "Strip.h"
#include "Globals.h"
#include "ChartObjectTextDialog.h"

#include <QDebug>
#include <QSettings>

ChartObjectText::ChartObjectText ()
{
  _draw = new ChartObjectTextDraw;
  _draw->setSettings(_settings);

  QSettings set(g_globalSettings);
  _settings->setData("Color", set.value("default_chart_object_text_color", "red").toString());
  _settings->setData("Font", set.value("default_chart_object_text_font", "Helvetica,9,50,0").toString());
  _settings->setData("Text", set.value("default_chart_object_text_text", "Text").toString());
  _settings->setData("Type", QString("Text"));
}

void ChartObjectText::info (Setting &info)
{
  info.setData(tr("Type"), tr("Text"));

  QDateTime dt = _settings->dateTime("Date");
  info.setData(tr("D"), dt.toString("yyyy-MM-dd"));
  info.setData(tr("T"), dt.toString("HH:mm:ss"));

  info.setData(tr("Price"), _settings->data("Price"));
  
  info.setData(tr("Text"), _settings->data("Text"));
}

int ChartObjectText::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings->dateTime("Date"));

  if (x < start || x > end)
    return 0;

  h = _settings->getDouble("Price");
  l = _settings->getDouble("Price");

  return 1;
}

void ChartObjectText::move (QPoint p)
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

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings->setData("Price", map.invTransform((double) p.y()));
      
      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectText::click (int button, QPoint p)
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
//          _editAction->setText(tr("Edit") + " " + _settings->data("ID"));
//          _deleteAction->setText(tr("Delete") + " " + _settings->data("ID"));
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

void ChartObjectText::create ()
{
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->data("ID"));
  emit signalMoveStart(_settings->data("ID"));
  g_middleMan->statusMessage(tr("Place Text object..."));
}

void ChartObjectText::dialog ()
{
  ChartObjectTextDialog *dialog = new ChartObjectTextDialog(0, _settings);
  connect(dialog, SIGNAL(accepted()), this, SLOT(update()));
  dialog->show();
}
