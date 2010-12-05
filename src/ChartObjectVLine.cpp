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

#include <QDebug>
#include <qwt_plot.h>
#include <QSettings>

ChartObjectVLine::ChartObjectVLine ()
{
  _draw = new ChartObjectVLineDraw;

  _settings.setData("Type", ChartObject::_VLine);

  QSettings set;
  set.beginGroup("main");

  _settings.setData("Color", set.value("default_chart_object_vline_color", "red").toString());
}

void ChartObjectVLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("VLine"));
  
  QDateTime dt = _settings.dateTime("Date");
  info.setData(QObject::tr("D"), dt.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), dt.toString("HH:mm:ss"));
}

int ChartObjectVLine::highLow (int, int, double &, double &)
{
  return 0;
}

/*
int ChartObjectVLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<EXCHANGE>,<SYMBOL>,<INDICATOR>,<DATE>,<COLOR>
  //  0   1        2         3          4        5       6

  if (l.count() != 7)
  {
    qDebug() << "ChartObjectVLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  // verify exchange
  Strip strip;
  QString s = l.at(2);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectVLine::CUS: invalid exchange" << l.at(2);
    return 1;
  }
  _settings.exchange = s;

  // verify symbol
  s = l.at(3);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectVLine::CUS: invalid symbol" << l.at(3);
    return 1;
  }
  _settings.symbol = s;

  // verify indicator
  s = l.at(4);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectVLine::CUS: invalid indicator" << l.at(4);
    return 1;
  }
  _settings.indicator = s;

  // verify date
  _settings.date = QDateTime::fromString(l.at(5), Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectVLine::CUS: invalid date" << l.at(5);
    return 1;
  }

  // verify color
  _settings.color.setNamedColor(l.at(6));
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectVLine::CUS: invalid color" << l.at(6);
    return 1;
  }

  return 0;
}
*/

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
      _settings.setData("Date", dt);
      
      _draw->setSettings(_settings);

      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectVLine::click (int button, QPoint p)
{
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
            emit signalMoveStart(_settings.getInt("ID"));
            _modified = 1;
            return;
          }

          if (! _draw->isSelected(p))
          {
            _status = _None;
            _draw->setSelected(FALSE);
            emit signalUnselected(_settings.getInt("ID"));
            _draw->plot()->replot();
            return;
          }
          break;
        case Qt::RightButton:
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
          emit signalMoveEnd(_settings.getInt("ID"));
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
            emit signalSelected(_settings.getInt("ID"));
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

void ChartObjectVLine::dialog ()
{
/*  
  ChartObjectVLineDialog *dialog = new ChartObjectVLineDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
*/
}

void ChartObjectVLine::dialog2 (Setting set)
{
  _modified = 1;
  setSettings(set);
  _draw->plot()->replot();
}

void ChartObjectVLine::create ()
{
  _modified = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings.getInt("ID"));
  emit signalMoveStart(_settings.getInt("ID"));
}