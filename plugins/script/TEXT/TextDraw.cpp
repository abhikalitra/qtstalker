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


#include "TextDraw.h"
#include "DateScaleDraw.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QPolygon>
#include <QSettings>

TextDraw::TextDraw ()
{
  _settings = new Setting;
  _selected = 0;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);

  QSettings set(g_globalSettings);
  _settings->setData("COLOR", set.value("default_text_color", "white").toString());
  QFont font;
  _settings->setData("FONT", set.value("default_text_font", font.toString()).toString());
  _settings->setData("TEXT", set.value("default_text_text", "Text").toString());
  _settings->setData("TYPE", QString("Text"));
  _settings->setData("PRICE", 0);
  _settings->setData("DATE", QDateTime::currentDateTime());
  _settings->setData("PLUGIN", QString("TEXT"));
  _settings->setData("Z", 99);
}

TextDraw::~TextDraw ()
{
  delete _settings;
  detach();
}

void TextDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->dateTime("DATE")));

  int y = yMap.transform(_settings->getDouble("PRICE"));

  p->setPen(_settings->color("COLOR"));

  p->setFont(_settings->font("FONT"));

  p->drawText(x, y, _settings->data("TEXT"));

  QFontMetrics fm = p->fontMetrics();

  _selectionArea.clear();

  _selectionArea.append(QRegion(x,
		                y - fm.height(),
		                fm.width(_settings->data("TEXT"), -1),
		                fm.height(),
		                QRegion::Rectangle));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - _handleWidth - 1,
		                y - (fm.height() / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x - _handleWidth - 1,
		y - (fm.height() / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("COLOR"));
  }
}

int TextDraw::rtti () const
{
  return Rtti_PlotUserItem;
}

int TextDraw::isSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _selectionArea.count(); loop++)
  {
    QRegion r = _selectionArea.at(loop);
    if (r.contains(p))
      return 1;
  }

  return 0;
}

int TextDraw::isGrabSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _grabHandles.count(); loop++)
  {
    QRegion r = _grabHandles.at(loop);
    if (r.contains(p))
      return loop + 1;
  }

  return 0;
}

void TextDraw::setSelected (int d)
{
  _selected = d;
}

Setting * TextDraw::settings ()
{
  return _settings;
}
