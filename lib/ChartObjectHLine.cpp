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
#include "Config.h"
#include "ChartObjectHLineDialog.h"
#include "DateScaleDraw.h"
#include "ChartObjectHLineDraw.h"

#include <QDebug>

ChartObjectHLine::ChartObjectHLine ()
{
  _draw = new ChartObjectHLineDraw;

  _settings.type = (int) ChartObject::_HLine;

  Config config;
  config.getData(Config::DefaultChartObjectHLineColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::red);

    config.transaction();
    config.setData(Config::DefaultChartObjectHLineColor, _settings.color);
    config.commit();
  }
}

void ChartObjectHLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("HLine"));
  info.setData(QObject::tr("Price"), _settings.price);
}

int ChartObjectHLine::highLow (int, int, double &h, double &l)
{
  h = _settings.price;
  l = _settings.price;

  return 1;
}

int ChartObjectHLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<PRICE>,<COLOR>
  //  0    1      2       3

  if (l.count() != 4)
  {
    qDebug() << "ChartObjectHLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  bool ok;
  _settings.price = l[2].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectHLine::CUS: invalid price" << l[2];
    return 1;
  }

  _settings.color.setNamedColor(l[3]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectHLine::CUS: invalid color" << l[3];
    return 1;
  }

  return 0;
}

void ChartObjectHLine::move (QPoint p)
{
  switch (_status)
  {
    case _Move:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings.price = map.invTransform((double) p.y());
      
      _draw->setSettings(_settings);

      _draw->plot()->replot();
      break;
    }
    default:
      break;
  }
}

void ChartObjectHLine::click (int button, QPoint p)
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

void ChartObjectHLine::dialog ()
{
  ChartObjectHLineDialog *dialog = new ChartObjectHLineDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartObjectHLine::dialog2 (ChartObjectSettings set)
{
  _modified = 1;
  setSettings(set);
  _draw->plot()->replot();
}

void ChartObjectHLine::create ()
{
  _modified = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings.id);
  emit signalMoveStart(_settings.id);
}

