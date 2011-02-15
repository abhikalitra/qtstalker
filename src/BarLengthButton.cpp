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

#include "BarLengthButton.h"
#include "BarData.h"
#include "Globals.h"

#include <QDebug>
#include <QSettings>

BarLengthButton::BarLengthButton ()
{
  QFont font;
  font.setPointSize(9);
  setFont(font);

  BarData bd;
  bd.barLengthList(_lengthList);
  
  int loop = 0;
  for (; loop < _lengthList.count(); loop++)
  {
    QString s;
    bd.barLengthText((BarData::BarLength) loop, s);
    _shortList << s;
  }

  createMenu();

  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Bar Length"));
  setStatusTip(tr("Bar Length"));
}

void BarLengthButton::createMenu ()
{
  _menu = new QMenu(this);
  _menu->setTitle(tr("Bar Length"));
  connect(_menu, SIGNAL(triggered(QAction *)), this, SLOT(lengthChanged(QAction *)));
  setMenu(_menu);

  QSettings settings(g_settingsFile);
  int index = settings.value("bar_length", 6).toInt();

  QActionGroup *group = new QActionGroup(this);
  
  int loop = 0;
  for (; loop < _lengthList.count(); loop++)
  {
    QAction *a = _menu->addAction(_lengthList.at(loop));
    a->setCheckable(TRUE);
    group->addAction(a);

    if (loop == index)
    {
      a->setChecked(TRUE);
      setText(_shortList.at(loop));
    }
  }
}

void BarLengthButton::lengthChanged (QAction *d)
{
  int index = _lengthList.indexOf(d->text());
  
  QSettings settings(g_settingsFile);
  settings.setValue("bar_length", index);
  settings.sync();
  
  setText(_shortList.at(index));

  emit signalBarLengthChanged(index);
}

int BarLengthButton::length ()
{
  QSettings settings(g_settingsFile);
  return settings.value("bar_length", 6).toInt();
}
