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
#include "Config.h"
#include "ScriptDataBase.h"

#include "../pics/configure.xpm"

#include <QPixmap>
#include <QCursor>
#include <QInputDialog>

ScriptLaunchButton::ScriptLaunchButton (int pos, int label)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  _position = pos;
  setMinimumWidth(32);
  setText(QString::number(label));

  Config config;
  config.getData((Config::Parm) _position, _scriptName);

  Script script;
  script.setName(_scriptName);

  ScriptDataBase db;
  if (! db.getScript(&script))
    setToolTip(_scriptName);
  else
    _scriptName.clear();

  connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));
  
  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("&Configure"), this, SLOT(configure()), Qt::CTRL+Qt::Key_C);
}

void ScriptLaunchButton::buttonClicked ()
{
  Script *script = new Script;
  script->setName(_scriptName);

  ScriptDataBase db;
  if (db.getScript(script))
  {
    delete script;
    return;
  }

  emit signalButtonClicked(script);
}

void ScriptLaunchButton::configure ()
{
  ScriptDataBase db;
  QStringList l;
  db.getScripts(l);

  int index = l.indexOf(_scriptName);

  bool ok = FALSE;
  QString s = QInputDialog::getItem(this,
                                    tr("Configure Script Launcher"),
                                    tr("Script"),
                                    l,
                                    index,
                                    FALSE,
                                    &ok,
                                    0);
  
  if (! ok)
    return;

  _scriptName = s;

  Config config;
  config.setData((Config::Parm) _position, _scriptName);

  setToolTip(_scriptName);
}

void ScriptLaunchButton::contextMenu()
{
  _menu->exec(QCursor::pos());
}
