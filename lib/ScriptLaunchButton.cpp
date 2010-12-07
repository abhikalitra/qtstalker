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
#include "ScriptPluginFactory.h"

#include "../pics/configure.xpm"

#include <QPixmap>
#include <QCursor>
#include <QInputDialog>
#include <QSettings>
#include <QDebug>

ScriptLaunchButton::ScriptLaunchButton (int pos, int label)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  _position = pos;
  setMinimumWidth(32);
  setText(QString::number(label));

  QSettings settings;
  settings.beginGroup("main");
  _scriptName = settings.value("script_launch_button_" + QString::number(_position)).toString();

  setToolTip(_scriptName);

  connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));
  
  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("&Configure"), this, SLOT(configure()), Qt::ALT+Qt::Key_C);
}

void ScriptLaunchButton::buttonClicked ()
{
  if (_scriptName.isEmpty())
    return;
  
  emit signalButtonClicked(_scriptName);
}

void ScriptLaunchButton::configure ()
{
  QStringList cl;
  cl << "SCRIPT_DATABASE" << "SCRIPTS";

  Command command(cl.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ScriptLaunchButton::configure: no plugin" << command.plugin();
    return;
  }

  plug->command(&command);
  delete plug;
  
  QStringList l = command.stringData().split(",");
  int index = l.indexOf(_scriptName);

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setLabelText(tr("Select Script"));
  dialog->setWindowTitle(tr("Configure Script Launcher"));
  if (index != -1)
    dialog->setTextValue(l.at(index));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(configure2(QString)));
  dialog->show();
}

void ScriptLaunchButton::configure2 (QString d)
{
  _scriptName = d;

  setToolTip(_scriptName);

  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("script_launch_button_" + QString::number(_position), _scriptName);
  settings.sync();
}

void ScriptLaunchButton::contextMenu()
{
  _menu->exec(QCursor::pos());
}

