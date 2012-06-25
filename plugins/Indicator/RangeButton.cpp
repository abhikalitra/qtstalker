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

#include "RangeButton.h"
#include "DateRange.h"

#include <QtDebug>
#include <QMenu>


RangeButton::RangeButton ()
{
  createGUI();
}

void
RangeButton::createGUI ()
{
  QMenu *menu = new QMenu(this);
  connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(rangeAction(QAction *)));
  
  DateRange dr;
  QStringList tl = dr.list();
  for (int pos = 0; pos < tl.size(); pos++)
  {
    QAction *a = menu->addAction(tl.at(pos));
    a->setData(QVariant(tl.at(pos)));
  }
  
  setPopupMode(QToolButton::InstantPopup);
  setText(dr.indexToString(DateRange::_YEAR));
  setToolTip(tr("Date Range"));
  setStatusTip(tr("Date Range"));
  setMenu(menu);
  setMaximumSize(QSize(25, 25));
}

void
RangeButton::rangeAction (QAction *d)
{
  setText(d->data().toString());
  emit signalRange();
}

int
RangeButton::range ()
{
  DateRange dr;
  return dr.stringToIndex(text());
}

QString
RangeButton::rangeText ()
{
  return text();
}

void
RangeButton::setRange (int d)
{
  DateRange dr;
  setText(dr.indexToString(d));
}

int
RangeButton::loadSettings (QSettings &)
{
  return 0;
}

int
RangeButton::saveSettings (QSettings &)
{
  return 0;
}
