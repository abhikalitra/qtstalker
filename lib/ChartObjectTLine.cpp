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
#include "Config.h"
#include "ChartObjectTLineDialog.h"
#include "DateScaleDraw.h"
#include "ChartObjectTLineDraw.h"

#include <QDebug>
#include <qwt_plot.h>

ChartObjectTLine::ChartObjectTLine ()
{
  _createFlag = 0;
  
  _draw = new ChartObjectTLineDraw;

  _settings.type = (int) ChartObject::_TLine;

  Config config;
  config.transaction();
  config.getData(Config::DefaultChartObjectTLineColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::green);
    config.setData(Config::DefaultChartObjectTLineColor, _settings.color);
  }

  QString s;
  config.getData(Config::DefaultChartObjectTLineExtend, s);
  if (s.isEmpty())
  {
    _settings.extend = 0;
    config.setData(Config::DefaultChartObjectTLineExtend, _settings.extend);
  }
  else
    _settings.extend = s.toInt();

  config.commit();
}

void ChartObjectTLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("TLine"));
  info.setData(QObject::tr("SD"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ST"), _settings.date.toString("HH:mm:ss"));
  info.setData(QObject::tr("ED"), _settings.date2.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ET"), _settings.date2.toString("HH:mm:ss"));
  info.setData(QObject::tr("SP"), _settings.price);
  info.setData(QObject::tr("EP"), _settings.price2);
}

int ChartObjectTLine::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings.date);
  int x2 = dsd->x(_settings.date2);

  if ((x >= start && x <= end) || ((x2 >= start && x2 <= end)))
  {
    h = -99999999.0;
    l = 99999999.0;

    if (_settings.price > h)
      h = _settings.price;

    if (_settings.price < l)
      l = _settings.price;

    if (_settings.price2 > h)
      h = _settings.price2;

    if (_settings.price2 < l)
      l = _settings.price2;
    
    return 1;
  }

  return 0;
}

int ChartObjectTLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<PRICE>,<DATE2>,<PRICE2>,<COLOR>
  //  0    1      2      3       4       5        6

  if (l.count() != 7)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  _settings.date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid start date" << l[2];
    return 1;
  }

  bool ok;
  _settings.price = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid start price" << l[3];
    return 1;
  }

  _settings.date2 = QDateTime::fromString(l[4], Qt::ISODate);
  if (! _settings.date2.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid end date" << l[4];
    return 1;
  }

  _settings.price2 = l[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid end price" << l[5];
    return 1;
  }

  _settings.color.setNamedColor(l[6]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid color" << l[6];
    return 1;
  }

  return 0;
}

void ChartObjectTLine::move (QPoint p)
{
  switch (_status)
  {
    case _Move:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      dsd->date(x, _settings.date);

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings.price = map.invTransform((double) p.y());
      
      _draw->setSettings(_settings);

      _draw->plot()->replot();
      break;
    }
    case _Move2:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      dsd->date(x, _settings.date2);

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings.price2 = map.invTransform((double) p.y());
      
      _draw->setSettings(_settings);

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
            emit signalMoveStart(_settings.id);
            _modified = 1;
            return;
          }

          if (! _draw->isSelected(p))
          {
            _status = _None;
            _draw->setSelected(FALSE);
            emit signalUnselected(_settings.id);
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
          emit signalMoveEnd(_settings.id);
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
          emit signalMoveEnd(_settings.id);
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
            emit signalSelected(_settings.id);
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
  ChartObjectTLineDialog *dialog = new ChartObjectTLineDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartObjectTLine::dialog2 (ChartObjectSettings set)
{
  _modified = 1;
  setSettings(set);
  _draw->plot()->replot();
}

void ChartObjectTLine::create ()
{
  _modified = 1;
  _createFlag = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings.id);
  emit signalMoveStart(_settings.id);
}

