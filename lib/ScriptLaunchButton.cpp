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

#include "../pics/configure.xpm"

#include <QPixmap>
#include <QCursor>
#include <QInputDialog>
#include <QSettings>
#include <QDebug>
#include <QFileInfo>

ScriptLaunchButton::ScriptLaunchButton (int pos)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  _position = pos;

  QSettings settings(g_localSettings);
  _command = settings.value("script_launch_button_command_" + QString::number(_position)).toString();
  _scriptName = settings.value("script_launch_button_file_" + QString::number(_position)).toString();
  _icon = settings.value("script_launch_button_icon_" + QString::number(_position)).toString();
  _useIcon = settings.value("script_launch_button_use_icon_" + QString::number(_position), 0).toInt();

  QFileInfo fi(_scriptName);
  setToolTip(fi.baseName());

  setText(QString::number(_position));

  if (_useIcon)
    setIcon(QIcon(_icon));

  connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));

  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("&Configure"), this, SLOT(configure()), Qt::ALT+Qt::Key_C);
}

void ScriptLaunchButton::buttonClicked ()
{
  if (_scriptName.isEmpty())
    return;

  emit signalButtonClicked(_command, _scriptName);
}

void ScriptLaunchButton::configure ()
{
  ScriptLaunchButtonDialog *dialog = new ScriptLaunchButtonDialog(this, _command, _scriptName, _icon, _useIcon);
  connect(dialog, SIGNAL(signalDone(QString, QString, QString, int)), this, SLOT(configure2(QString, QString, QString, int)));
  dialog->show();
}

void ScriptLaunchButton::configure2 (QString command, QString file, QString icon, int use)
{
  _command = command;
  _scriptName = file;
  _icon = icon;
  _useIcon = use;

  QFileInfo fi(_scriptName);
  setToolTip(fi.baseName());

  setText(QString::number(_position));

  if (_useIcon)
    setIcon(QIcon(_icon));
  else
    setIcon(QIcon());

  QSettings settings(g_localSettings);
  settings.setValue("script_launch_button_command_" + QString::number(_position), _command);
  settings.setValue("script_launch_button_file_" + QString::number(_position), _scriptName);
  settings.setValue("script_launch_button_icon_" + QString::number(_position), _icon);
  settings.setValue("script_launch_button_use_icon_" + QString::number(_position), _useIcon);
  settings.sync();
}

void ScriptLaunchButton::contextMenu()
{
  _menu->exec(QCursor::pos());
}
