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
#include "DialogConfirm.h"

#include "../pics/configure.xpm"
#include "../pics/delete.xpm"

#include <QPixmap>
#include <QCursor>
#include <QDebug>
#include <QUuid>

ScriptLaunchButton::ScriptLaunchButton (QString name)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  _row = 0;
  _col = 0;
  
  _name = name;
  if (_name.isEmpty())
    _name = QUuid::createUuid();
  
  loadSettings();

  connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));

  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("&Configure"), this, SLOT(configure()), Qt::ALT+Qt::Key_C);
  _menu->addAction(QPixmap(delete_xpm), tr("&Delete"), this, SLOT(remove()), Qt::ALT+Qt::Key_D);
}

void ScriptLaunchButton::buttonClicked ()
{
  emit signalButtonClicked(_script);
}

void ScriptLaunchButton::configure ()
{
  ScriptLaunchButtonDialog *dialog = new ScriptLaunchButtonDialog(this, _script, _icon, _useIcon, _row, _col, _text);
  connect(dialog, SIGNAL(signalDone(QString, QString, bool, int, int, QString)),
	  this, SLOT(configure2(QString, QString, bool, int, int, QString)));
  dialog->show();
}

void ScriptLaunchButton::configure2 (QString script, QString icon, bool use, int row, int col, QString text)
{
  setSettings(script, icon, use, row, col, text);
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
  settings.setName(_name);
  if (db.get(settings))
    return;

  KeyScriptLaunchButton keys;
  Data script, icon, iconUse, row, col, text;
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_SCRIPT), script);
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_ICON), icon);
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_ICON_USE), iconUse);
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_ROW), row);
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_COL), col);
  settings.toData(keys.indexToString(KeyScriptLaunchButton::_TEXT), text);
  
  setSettings(script.toString(), icon.toString(), iconUse.toInteger(), row.toInteger(),
	      col.toInteger(), text.toString());
}

void ScriptLaunchButton::setSettings (QString script, QString icon, bool use, int row, int col, QString text)
{
  _script = script;
  _icon = icon;
  _useIcon = use;
  _row = row;
  _col = col;
  _text = text;

  setToolTip(_script);

  setText(_text);

  if (_useIcon)
    setIcon(QIcon(_icon));
  else
    setIcon(QIcon());
}

void ScriptLaunchButton::saveSettings ()
{
  KeyScriptLaunchButton keys;
  Entity settings;
  settings.setName(_name);
  settings.set(keys.indexToString(KeyScriptLaunchButton::_SCRIPT), Data(_script));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_ICON), Data(_icon));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_ICON_USE), Data(_useIcon));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_ROW), Data(_row));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_COL), Data(_col));
  settings.set(keys.indexToString(KeyScriptLaunchButton::_TEXT), Data(_text));
  
  EAVDataBase db("scriptLaunchButtons");
  if (db.set(settings))
    qDebug() << "ScriptLaunchButton::saveSettings: db error";
}

int ScriptLaunchButton::row ()
{
  return _row;
}

int ScriptLaunchButton::col ()
{
  return _col;
}

QString ScriptLaunchButton::name ()
{
  return _name;
}

void ScriptLaunchButton::remove ()
{
  DialogConfirm *dialog = new DialogConfirm(this, QString(), Entity());
  connect(dialog, SIGNAL(accepted()), this, SLOT(remove2()));
  
  QStringList tl;
  tl << tr("Confirm delete button") << _text;
  dialog->setMessage(tl.join(" "));
  
  dialog->show();
}

void ScriptLaunchButton::remove2 ()
{
  emit signalButtonRemove(_name);
}
