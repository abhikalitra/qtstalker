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

#include "ChartObject.h"
#include "../pics/delete.xpm"
#include "../pics/edit.xpm"

#include <QDebug>

ChartObject::ChartObject ()
{
  _status = _None;
  _modified = 0;
  
  _settings.setData(QString("ID"), -1);
  _settings.setData(QString("Type"),-1);

  _menu = new QMenu;
  _menu->addAction(QPixmap(edit_xpm), tr("&Edit"), this, SLOT(dialog()), Qt::ALT+Qt::Key_E);
  _menu->addAction(QPixmap(delete_xpm), tr("&Delete"), this, SLOT(deleteChartObject()), Qt::ALT+Qt::Key_D);
}

ChartObject::~ChartObject ()
{
  delete _menu;
  delete _draw;
}

void ChartObject::info (Setting &)
{
}

int ChartObject::isSelected (QPoint p)
{
  return _draw->isSelected(p);
}

int ChartObject::highLow (int, int, double &, double &)
{
  return 0;
}

void ChartObject::move (QPoint)
{
}

void ChartObject::click (int, QPoint)
{
}

Setting & ChartObject::settings ()
{
  return _settings;
}

ChartObject::Status ChartObject::status ()
{
  return _status;
}

int ChartObject::isModified ()
{
  return _modified;
}

void ChartObject::create ()
{
}

void ChartObject::dialog ()
{
}

void ChartObject::deleteChartObject ()
{
// FIXME  
/*  
  ChartObjectDataBase db;
  db.transaction();
  db.deleteChartObject(_settings.id);
  db.commit();

  emit signalDelete(_settings.id);
*/
}

void ChartObject::setZ (int d)
{
  _draw->setZ(d);
}

void ChartObject::attach (QwtPlot *p)
{
  _draw->attach(p);
}

void ChartObject::setSettings (Setting &d)
{
  _settings = d;
  _draw->setSettings(d);
}

