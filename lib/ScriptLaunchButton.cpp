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

#include "ScriptLaunchButton.h"
#include "ScriptLaunchButtonDialog.h"
#include "Global.h"
#include "EAVDataBase.h"
#include "KeyScriptLaunchButton.h"

#include "../pics/configure.xpm"

#include <QPixmap>
#include <QCursor>
#include <QInputDialog>
#include <QDebug>

ScriptLaunchButton::ScriptLaunchButton (int pos)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  _position = pos;

  loadSettings();

  connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));

  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("&Configure"), this, SLOT(configure()), Qt::ALT+Qt::Key_C);
}

void ScriptLaunchButton::buttonClicked ()
{
  if (_name.isEmpty())
    return;

  emit signalButtonClicked(_name);
}

void ScriptLaunchButton::configure ()
{
  ScriptLaunchButtonDialog *dialog = new ScriptLaunchButtonDialog(this, _name, _icon, _useIcon);
  connect(dialog, SIGNAL(signalDone(QString, QString, bool)), this, SLOT(configure2(QString, QString, bool)));
  dialog->show();
}

void ScriptLaunchButton::configure2 (QString name, QString icon, bool use)
{
  setSettings(name, icon, use);
  saveSettings();
}

void ScriptLaunchButton::contextMenu ()
{
  _menu->exec(QCursor::pos());
}

void ScriptLaunchButton::loadSettings ()
{
  EAVDataBase db("scriptLaunchButtons");
  Entity settings;
  settings.setName(QString::number(_position));
  if (db.get(settings))
    return;

  KeyScriptLaunchButton keys;
  Data name, icon, iconUse;
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_NAME), name);
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_ICON), icon);
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_ICON_USE), iconUse);
  
  setSettings(name.toString(), icon.toString(), iconUse.toInteger());
}

void ScriptLaunchButton::setSettings (QString name, QString icon, bool use)
{
  _name = name;
  _icon = icon;
  _useIcon = use;

  setToolTip(name);

  setText(QString::number(_position));

  if (_useIcon)
    setIcon(QIcon(_icon));
  else
    setIcon(QIcon());
}

void ScriptLaunchButton::saveSettings ()
{
  KeyScriptLaunchButton keys;
  EAVDataBase db("scriptLaunchButtons");
  Entity settings;
  settings.setName(QString::number(_position));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_NAME), Data(_name));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_ICON), Data(_icon));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_ICON_USE), Data(_useIcon));
  if (db.set(settings))
    qDebug() << "ScriptLaunchButton::saveSettings: db error";
}
