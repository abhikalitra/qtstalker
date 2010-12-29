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


#include "ChartObjectTLine.h"
#include "DateScaleDraw.h"
#include "ChartObjectTLineDraw.h"
#include "Strip.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QSettings>

ChartObjectTLine::ChartObjectTLine ()
{
  _createFlag = 0;
  _draw = new ChartObjectTLineDraw;
}

void ChartObjectTLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("TLine"));

  QDateTime dt = _settings->dateTime("Date");
  info.setData(QObject::tr("SD"), dt.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ST"), dt.toString("HH:mm:ss"));

  dt = _settings->dateTime("Date2");
  info.setData(QObject::tr("ED"), dt.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ET"), dt.toString("HH:mm:ss"));
  
  info.setData(QObject::tr("SP"), _settings->data("Price"));
  
  info.setData(QObject::tr("EP"), _settings->data("Price2"));
}

int ChartObjectTLine::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings->dateTime("Date"));
  int x2 = dsd->x(_settings->dateTime("Date2"));

  if ((x >= start && x <= end) || ((x2 >= start && x2 <= end)))
  {
    h = -99999999.0;
    l = 99999999.0;

    double tp = _settings->getDouble("Price");
    if (tp > h)
      h = tp;

    if (tp < l)
      l = tp;

    tp = _settings->getDouble("Price2");
    if (tp > h)
      h = tp;

    if (tp < l)
      l = tp;
    
    return 1;
  }

  return 0;
}

/*
int ChartObjectTLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<EXCHANGE>,<SYMBOL>,<INDICATOR>,<DATE>,<PRICE>,<DATE2>,<PRICE2>,<COLOR>
  //  0    1       2         3          4        5       6       7       8        9

  if (l.count() != 10)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  // verify exchange
  Strip strip;
  QString s = l.at(2);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid exchange" << l.at(2);
    return 1;
  }
  _settings->exchange = s;

  // verify symbol
  s = l.at(3);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid symbol" << l.at(3);
    return 1;
  }
  _settings->symbol = s;

  // verify indicator
  s = l.at(4);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid indicator" << l.at(4);
    return 1;
  }
  _settings->indicator = s;

  // verify date
  _settings->date = QDateTime::fromString(l.at(5), Qt::ISODate);
  if (! _settings->date.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid start date" << l.at(5);
    return 1;
  }

  // verify price
  bool ok;
  _settings->price = l.at(6).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid start price" << l.at(6);
    return 1;
  }

  // verify date2
  _settings->date2 = QDateTime::fromString(l.at(7), Qt::ISODate);
  if (! _settings->date2.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid end date" << l.at(7);
    return 1;
  }

  // verify price2
  _settings->price2 = l.at(8).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid end price" << l.at(8);
    return 1;
  }

  // verify color
  _settings->color.setNamedColor(l.at(9));
  if (! _settings->color.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid color" << l.at(9);
    return 1;
  }

  return 0;
}
*/

void ChartObjectTLine::move (QPoint p)
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

      if (_createFlag)
      {
        _settings->setData("Date2", dt);
        _settings->setData("Price2", _settings->data("Price"));
      }
      
      _draw->plot()->replot();
      break;
    }
    case _Move2:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      _settings->setData("Date2", dt);

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings->setData("Price2", map.invTransform((double) p.y()));
      
      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectTLine::click (int button, QPoint p)
{
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
            _settings->setData("Modified", 1);
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
            return;
          }
          
          _status = _Selected;
          emit signalMoveEnd(_settings->data("ID"));
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

void ChartObjectTLine::dialog ()
{
/*  
  ChartObjectTLineDialog *dialog = new ChartObjectTLineDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
*/
}

void ChartObjectTLine::dialog2 (Setting)
{
  _settings->setData("Modified", 1);
//  setSettings(set);
  _draw->plot()->replot();
}

void ChartObjectTLine::create ()
{
  QSettings set(g_settingsFile);
  _settings->setData("Color", set.value("default_chart_object_tline_color", "red").toString());
  _settings->setData("Extend", set.value("default_chart_object_tline_extend", 0).toInt());
  _settings->setData("Modified", 1);
  _settings->setData("Type", ChartObject::_TLine);

  _createFlag = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->data("ID"));
  emit signalMoveStart(_settings->data("ID"));
}
