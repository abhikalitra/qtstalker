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
  
  _settings.id = -1;
  _settings.type = -1;
  _settings.color = QColor(Qt::red);
  _settings.date = QDateTime::currentDateTime();
  _settings.date2 = _settings.date;
  _settings.price = 0;
  _settings.price2 = 0;
  _settings.high = 0;
  _settings.low = 0;
  _settings.extend = 0;
  _settings.line1 = 0;
  _settings.line2 = 0;
  _settings.line3 = 0;
  _settings.line4 = 0;
  _settings.line5 = 0;
  _settings.line6 = 0;

  _menu = new QMenu;
  _menu->addAction(QPixmap(edit), tr("&Edit"), this, SLOT(dialog()), Qt::ALT+Qt::Key_E);
  _menu->addAction(QPixmap(delete_xpm), tr("&Delete"), this, SLOT(deleteChartObject()), Qt::ALT+Qt::Key_D);
}

ChartObject::~ChartObject ()
{
  delete _menu;
  
  _draw->detach();
  delete _draw;
}

void ChartObject::info (Setting &)
{
}

void ChartObject::load ()
{
}

int ChartObject::CUS (QStringList &)
{
  return 0;
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

void ChartObject::settings (ChartObjectSettings &d)
{
  d = _settings;
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
  emit signalDelete(_settings.id);
}

void ChartObject::setZ (int d)
{
  _draw->setZ(d);
}

void ChartObject::attach (QwtPlot *p)
{
  _draw->attach(p);
}

void ChartObject::setSettings (ChartObjectSettings &d)
{
  _settings = d;
  _draw->setSettings(d);
}

