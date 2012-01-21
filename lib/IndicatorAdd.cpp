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

#include "IndicatorAdd.h"
#include "GlobalParent.h"
#include "EAVDataBase.h"
#include "ScriptRunDialog.h"
#include "KeyIndicatorDataBase.h"
#include "NewDialog.h"
#include "GlobalSidePanel.h"
#include "WindowTitle.h"
#include "Global.h"

#include <QtDebug>
#include <QSettings>
#include <QTimer>

IndicatorAdd::IndicatorAdd (QObject *p) : QObject (p)
{
}

void IndicatorAdd::run ()
{
  QTimer::singleShot(0, this, SLOT(add()));
}

void IndicatorAdd::add ()
{
  EAVDataBase db("indicators");
  QStringList names;
  db.names(names);
  
  NewDialog *dialog = new NewDialog(g_parent);
  dialog->setTitle(tr("New indicator name"));
  dialog->setItems(names);

  WindowTitle wt;
  dialog->setWindowTitle(wt.title(tr("Add Indicator"), QString()));
  
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(add2(QString)));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->show();
}

void IndicatorAdd::add2 (QString name)
{
  _name = name;
  
  QSettings settings(g_localSettings);

  ScriptRunDialog *dialog = new ScriptRunDialog(g_parent,
                                                settings.value("add_indicator_last_script").toString(),
                                                settings.value("add_indicator_last_command", "perl").toString());
  connect(dialog, SIGNAL(signalDone(QString, QString)), this, SLOT(add3(QString, QString)));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  
  WindowTitle wt;
  dialog->setWindowTitle(wt.title(tr("Add Indicator"), QString()));
  
  dialog->show();
}

void IndicatorAdd::add3 (QString command, QString file)
{
  KeyIndicatorDataBase keys;
  Entity i;
  i.setName(_name);
  i.set(keys.indexToString(KeyIndicatorDataBase::_SESSION), Data(g_session));
  i.set(keys.indexToString(KeyIndicatorDataBase::_FILE), Data(file));
  i.set(keys.indexToString(KeyIndicatorDataBase::_COMMAND), Data(command));

  EAVDataBase db("indicators");
  db.transaction();
  if (db.set(i))
  {
    done();
    return;
  }
  db.commit();
  
  QSettings settings(g_localSettings);
  settings.setValue("add_indicator_last_script", file);
  settings.setValue("add_indicator_last_command", command);
  settings.sync();

  // launch indicator
  g_sidePanel->scriptPanel()->runScript(command, file);

  done();
}

void IndicatorAdd::done ()
{
  deleteLater();
}
