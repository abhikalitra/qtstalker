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


#include "ChartObjectBuy.h"
#include "DateScaleDraw.h"
#include "ChartObjectBuyDraw.h"
#include "Strip.h"
#include "Globals.h"

#include <QDebug>
#include <QSettings>

ChartObjectBuy::ChartObjectBuy ()
{
  _draw = new ChartObjectBuyDraw;
  _draw->setSettings(_settings);
  
  _settings->setData(QString("Type"), ChartObject::_Buy);

  QSettings set(g_settingsFile);
  _settings->setData("Color", set.value("default_chart_object_buy_color", "green").toString());
}

void ChartObjectBuy::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("Buy"));

  QDateTime dt = _settings->dateTime("Date");
  info.setData(QObject::tr("D"), dt.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), dt.toString("HH:mm:ss"));

  info.setData(QObject::tr("Price"), _settings->data("Price"));
}

/*
int ChartObjectBuy::CUS (QStringList &l)
{
  // CO,<TYPE>,<EXCHANGE>,<SYMBOL>,<INDICATOR>,<DATE>,<PRICE>,<COLOR>
  //  0   1         2        3         4          5      6       7

  if (l.count() != 8)
  {
    qDebug() << "ChartObjectBuy::CUS: invalid parm count" << l.count();
    return 1;
  }

  // verify exchange
  Strip strip;
  QString s = l.at(2);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectBuy::CUS: invalid exchange" << l.at(2);
    return 1;
  }
  _settings->exchange = s;

  // verify symbol
  s = l.at(3);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectBuy::CUS: invalid symbol" << l.at(3);
    return 1;
  }
  _settings->symbol = s;

  // verify indicator
  s = l.at(4);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectBuy::CUS: invalid indicator" << l.at(4);
    return 1;
  }
  _settings->indicator = s;

  // verify date
  _settings->date = QDateTime::fromString(l.at(5), Qt::ISODate);
  if (! _settings->date.isValid())
  {
    qDebug() << "ChartObjectBuy::CUS: invalid date" << l.at(5);
    return 1;
  }

  // verify price
  bool ok;
  _settings->price = l.at(6).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectBuy::CUS: invalid price" << l.at(6);
    return 1;
  }

  // verify color
  _settings->color.setNamedColor(l.at(7));
  if (! _settings->color.isValid())
  {
    qDebug() << "ChartObjectBuy::CUS: invalid color" << l.at(7);
    return 1;
  }

  return 0;
}
*/

int ChartObjectBuy::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) _draw->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings->dateTime("Date"));

  if (x < start || x > end)
    return 0;
  
  h = _settings->getDouble("Price");
  l = _settings->getDouble("Price");

  return 1;
}

void ChartObjectBuy::move (QPoint p)
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

void ChartObjectBuy::click (int button, QPoint p)
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
            emit signalMoveStart(_settings->getInt("ID"));
            _modified = 1;
            return;
          }

          if (! _draw->isSelected(p))
          {
            _status = _None;
            _draw->setSelected(FALSE);
            emit signalUnselected(_settings->getInt("ID"));
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
          emit signalMoveEnd(_settings->getInt("ID"));
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
            emit signalSelected(_settings->getInt("ID"));
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

void ChartObjectBuy::dialog ()
{
/*  
  ChartObjectBuyDialog *dialog = new ChartObjectBuyDialog;
  dialog->setSettings(_settings);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(dialog2(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
*/
}

void ChartObjectBuy::dialog2 (Setting)
{
  _modified = 1;
//  setSettings(set);
  _draw->plot()->replot();
}

void ChartObjectBuy::create ()
{
  _modified = 1;
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->getInt("ID"));
  emit signalMoveStart(_settings->getInt("ID"));
}
