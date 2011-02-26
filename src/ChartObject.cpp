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
#include "Globals.h"
#include "ChartObjectDataBase.h"
#include "Script.h"

#include <QDebug>

ChartObject::ChartObject ()
{
  _settings = new Setting;
  _status = _None;

  _menu = new QMenu;
  _editAction = _menu->addAction(QPixmap(edit_xpm), tr("&Edit"), this, SLOT(dialog()), Qt::ALT+Qt::Key_E);
  _deleteAction = _menu->addAction(QPixmap(delete_xpm), tr("&Delete"), this, SLOT(deleteChartObject()), Qt::ALT+Qt::Key_D);
}

ChartObject::~ChartObject ()
{
  delete _menu;
  delete _draw;
  delete _settings;
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

ChartObject::Status ChartObject::status ()
{
  return _status;
}

void ChartObject::create ()
{
}

void ChartObject::dialog ()
{
  QSettings lsettings(g_localSettings);
  lsettings.setValue("chart_object_edit_id", _settings->data("ID"));
  lsettings.sync();

  QSettings gsettings(g_globalSettings);

  Script *script = new Script(this);
  script->setName("ChartObjectEdit");
  script->setFile(gsettings.value("chart_object_edit_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ChartObject::deleteChartObject ()
{
  QSettings lsettings(g_localSettings);
  lsettings.setValue("chart_object_delete_id", _settings->data("ID"));
  lsettings.sync();

  QSettings gsettings(g_globalSettings);

  Script *script = new Script(this);
  script->setName("ChartObjectDelete");
  script->setFile(gsettings.value("chart_object_delete_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ChartObject::setZ (int d)
{
  _draw->setZ(d);
}

void ChartObject::attach (QwtPlot *p)
{
  _draw->attach(p);
}

Setting * ChartObject::settings ()
{
  return _settings;
}

void ChartObject::setSettings (Setting *d)
{
  QString s;
  d->string(s);
  _settings->parse(s);
}

void ChartObject::load ()
{
  if (_settings->getInt("RO"))
    return;
  
  ChartObjectDataBase db;
  db.load(_settings);
}

void ChartObject::save ()
{
  if (_settings->getInt("RO"))
    return;

  ChartObjectDataBase db;
  db.save(_settings);
}
