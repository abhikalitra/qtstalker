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
#include "Config.h"
#include "ChartObjectRetracementDialog.h"
#include "DateScaleDraw.h"
#include "ChartObjectRetracementDraw.h"

#include <QDebug>

ChartObjectRetracement::ChartObjectRetracement ()
{
  _createFlag = 0;
  
  _draw = new ChartObjectRetracementDraw;

  _settings.type = (int) ChartObject::_Retracement;

  Config config;
  config.getData(Config::DefaultChartObjectRetracementColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::red);
    config.setData(Config::DefaultChartObjectRetracementColor, _settings.color);
  }

  _settings.line1 = config.getDouble(Config::DefaultChartObjectRetracementLine1);
  if (! _settings.line1)
  {
    _settings.line1 = 0.382;
    config.setData(Config::DefaultChartObjectRetracementLine1, _settings.line1);
  }

  _settings.line2 = config.getDouble(Config::DefaultChartObjectRetracementLine2);
  if (! _settings.line2)
  {
    _settings.line2 = 0.5;
    config.setData(Config::DefaultChartObjectRetracementLine2, _settings.line2);
  }

  _settings.line3 = config.getDouble(Config::DefaultChartObjectRetracementLine3);
  if (! _settings.line3)
  {
    _settings.line3 = 0.618;
    config.setData(Config::DefaultChartObjectRetracementLine3, _settings.line3);
  }

  _settings.line4 = config.getDouble(Config::DefaultChartObjectRetracementLine4);
  _settings.line5 = config.getDouble(Config::DefaultChartObjectRetracementLine5);
  _settings.line6 = config.getDouble(Config::DefaultChartObjectRetracementLine6);
}

void ChartObjectRetracement::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("Retracement"));
  info.setData(QObject::tr("SD"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ST"), _settings.date.toString("HH:mm:ss"));
  info.setData(QObject::tr("ED"), _settings.date2.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ET"), _settings.date2.toString("HH:mm:ss"));
  info.setData(QObject::tr("High"), _settings.high);
  info.setData(QObject::tr("Low"), _settings.low);
  info.setData(QObject::tr("Level 1"), _settings.line1);
  info.setData(QObject::tr("Level 2"), _settings.line2);
  info.setData(QObject::tr("Level 3"), _settings.line3);
  info.setData(QObject::tr("Level 4"), _settings.line4);
  info.setData(QObject::tr("Level 5"), _settings.line5);
  info.setData(QObject::tr("Level 6"), _settings.line6);
}

int ChartObjectRetracement::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings.date);
  int x2 = dsd->x(_settings.date2);

  if ((x >= start && x <= end) || ((x2 >= start && x2 <= end)))
  {
    h = _settings.high;
    l = _settings.low;
    return 1;
  }

  return 0;
}

int ChartObjectRetracement::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<DATE2>,<HIGH>,<LOW>,<PERCENTAGE>,<COLOR>
  //  0    1      2      3      4      5        6          7

  if (l.count() != 8)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid parm count" << l.count();
    return 1;
  }

  _settings.date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid start date" << l[2];
    return 1;
  }

  _settings.date2 = QDateTime::fromString(l[3], Qt::ISODate);
  if (! _settings.date2.isValid())
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid end date" << l[3];
    return 1;
  }

  bool ok;
  _settings.high = l[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid high" << l[4];
    return 1;
  }

  _settings.low = l[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid low" << l[5];
    return 1;
  }

  _settings.line1 = l[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid retracement" << l[6];
    return 1;
  }

  _settings.color.setNamedColor(l[7]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid color" << l[7];
    return 1;
  }

  _settings.line2 = 0;
  _settings.line3 = 0;
  _settings.line4 = 0;
  _settings.line5 = 0;
  _settings.line6 = 0;

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

      DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
      dsd->date(x, _settings.date);

      map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings.high = map.invTransform((double) p.y());
      
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
      _settings.low = map.invTransform((double) p.y());
      
      _draw->setSettings(_settings);

      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectRetracement::click (int button, QPoint p)
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

void ChartObjectRetracement::dialog ()
{
  ChartObjectRetracementDialog *dialog = new ChartObjectRetracementDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartObjectRetracement::dialog2 (ChartObjectSettings set)
{
  _modified = 1;
  setSettings(set);
  _draw->plot()->replot();
}

void ChartObjectRetracement::create ()
{
  _modified = 1;
  _createFlag = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings.id);
  emit signalMoveStart(_settings.id);
}

