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

#include <QDebug>
#include <QSettings>

ChartObjectText::ChartObjectText ()
{
  _draw = new ChartObjectTextDraw;
  _draw->setSettings(_settings);

  QSettings set(g_settingsFile);
  _settings->setData("Color", set.value("default_chart_object_text_color", "red").toString());
  _settings->setData("Font", set.value("default_chart_object_text_font", "Helvetica,9,50,0").toString());
  _settings->setData("Text", set.value("default_chart_object_text_text", "Text").toString());
  _settings->setData("Type", ChartObject::_Text);
}

void ChartObjectText::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("Text"));

  QDateTime dt = _settings->dateTime("Date");
  info.setData(QObject::tr("D"), dt.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), dt.toString("HH:mm:ss"));

  info.setData(QObject::tr("Price"), _settings->data("Price"));
  
  info.setData(QObject::tr("Text"), _settings->data("Text"));
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

/*
int ChartObjectText::CUS (QStringList &l)
{
  // CO,<TYPE>,<EXCHANGE>,<SYMBOL>,<INDICATOR>,<DATE>,<PRICE>,<LABEL>,<COLOR>
  //  0   1         2        3          4         5      6       7       8

  if (l.count() != 9)
  {
    qDebug() << "ChartObjectText::CUS: invalid parm count" << l.count();
    return 1;
  }

  // verify exchange
  Strip strip;
  QString s = l.at(2);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectText::CUS: invalid exchange" << l.at(2);
    return 1;
  }
  _settings->exchange = s;

  // verify symbol
  s = l.at(3);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectText::CUS: invalid symbol" << l.at(3);
    return 1;
  }
  _settings->symbol = s;

  // verify indicator
  s = l.at(4);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectText::CUS: invalid indicator" << l.at(4);
    return 1;
  }
  _settings->indicator = s;

  // verify date
  _settings->date = QDateTime::fromString(l.at(5), Qt::ISODate);
  if (! _settings->date.isValid())
  {
    qDebug() << "ChartObjectText::CUS: invalid date" << l.at(5);
    return 1;
  }

  // verify price
  bool ok;
  _settings->price = l.at(6).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectText::CUS: invalid price" << l.at(6);
    return 1;
  }

  // verify text
  s = l.at(7);
  strip.verifyText(s);
  if (s.isEmpty())
  {
    qDebug() << "ChartObjectText::CUS: invalid text" << l.at(7);
    return 1;
  }
  _settings->text = s;

  // verify color
  _settings->color.setNamedColor(l.at(8));
  if (! _settings->color.isValid())
  {
    qDebug() << "ChartObjectText::CUS: invalid color" << l.at(8);
    return 1;
  }

  return 0;
}
*/

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
        case Qt::RightButton:
          _editAction->setText(tr("Edit") + " " + _settings->data("ID"));
          _deleteAction->setText(tr("Delete") + " " + _settings->data("ID"));
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
          _settings->setData("Modified", 1);
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
  _settings->setData("Modified", 1);
  _status = _Move;
  _draw->setSelected(TRUE);
  emit signalSelected(_settings->data("ID"));
  emit signalMoveStart(_settings->data("ID"));
}
