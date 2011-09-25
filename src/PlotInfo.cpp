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

#include "PlotInfo.h"
#include "Strip.h"

#include <QDebug>
#include <qwt_plot.h>
#include <qwt_scale_div.h>

PlotInfo::PlotInfo ()
{
  setYAxis(QwtPlot::yRight);
  setZ(9999);
}

PlotInfo::~PlotInfo ()
{
  detach();
}

int PlotInfo::rtti () const
{
  return Rtti_PlotUserItem;
}

void PlotInfo::clearData ()
{
  _name.clear();
  _textList.clear();
  _colorList.clear();
  _valList.clear();
}

void PlotInfo::setData (QString d, QColor c, double v)
{
  _textList << d;
  _colorList << c;
  _valList << v;
}

void PlotInfo::setName (QString d)
{
  _name = d;
}

void PlotInfo::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &, const QRect &) const
{
  QFontMetrics fm = p->fontMetrics();

  QFont font = p->font();
  font.setBold(TRUE);
  p->setFont(font);

  int x = 5;
  int y = fm.height();

  Strip strip;
  int loop = 0;
  for (; loop < _textList.count(); loop++)
  {
    p->setPen(_colorList.at(loop));

    QString ts;
    strip.strip(_valList.at(loop), 4, ts);

    QString s = _textList.at(loop) + "=" + ts;
    p->drawText(x, y, s);

    x += fm.width(s, -1) + 5;
  }
}
