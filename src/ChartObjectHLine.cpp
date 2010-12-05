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
#include "ChartObjectHLineDraw.h"
#include "Strip.h"

#include <QDebug>
#include <QSettings>

ChartObjectHLine::ChartObjectHLine ()
{
  _draw = new ChartObjectHLineDraw;

  _settings.setData("Type", ChartObject::_HLine);

  QSettings set;
  set.beginGroup("main");

  QString s = set.value("default_chart_object_hline_color", "red").toString();
  _settings.setData("Color", s);
}

void ChartObjectHLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("HLine"));
  info.setData(QObject::tr("Price"), _settings.data("Price"));
}

int ChartObjectHLine::highLow (int, int, double &h, double &l)
{
  h = _settings.getDouble("Price");
  l = _settings.getDouble("Price");

  return 1;
}

/*
int ChartObjectHLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<EXCHANGE>,<SYMBOL>,<INDICATOR>,<PRICE>,<COLOR>
  //  0    1       2         3          4         5       6

  if (l.count() != 7)
  {
    qDebug() << "ChartObjectHLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  // verify exchange
  Strip strip;
  QString s = l.at(2);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectHLine::CUS: invalid exchange" << l.at(2);
    return 1;
  }
  _settings.exchange = s;

  // verify symbol
  s = l.at(3);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectHLine::CUS: invalid symbol" << l.at(3);
    return 1;
  }
  _settings.symbol = s;

  // verify indicator
  s = l.at(4);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectHLine::CUS: invalid indicator" << l.at(4);
    return 1;
  }
  _settings.indicator = s;

  // verify price
  bool ok;
  _settings.price = l.at(5).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectHLine::CUS: invalid price" << l.at(5);
    return 1;
  }

  // verify color
  _settings.color.setNamedColor(l.at(6));
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectHLine::CUS: invalid color" << l.at(6);
    return 1;
  }

  return 0;
}
*/

void ChartObjectHLine::move (QPoint p)
{
  switch (_status)
  {
    case _Move:
    {
      QwtScaleMap map = _draw->plot()->canvasMap(QwtPlot::yRight);
      _settings.setData("Price", map.invTransform((double) p.y()));
      
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

void ChartObjectHLine::dialog ()
{
/*  
  ChartObjectHLineDialog *dialog = new ChartObjectHLineDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
*/
}

void ChartObjectHLine::dialog2 (Setting set)
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
  emit signalSelected(_settings.getInt("ID"));
  emit signalMoveStart(_settings.getInt("ID"));
}