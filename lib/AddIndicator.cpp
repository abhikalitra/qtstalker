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

#include "AddIndicator.h"
#include "Global.h"
#include "GlobalParent.h"
#include "GlobalSymbol.h"
#include "IndicatorDataBase.h"
#include "ScriptRunDialog.h"
#include "Script.h"

#include <QtDebug>
#include <QSettings>
#include <QTimer>

AddIndicator::AddIndicator (QObject *p) : QObject (p)
{
}

void AddIndicator::run ()
{
  QTimer::singleShot(0, this, SLOT(addIndicator()));
}

void AddIndicator::addIndicator ()
{
  QSettings settings(g_localSettings);

  QStringList wt;
  wt << "QtStalker" + g_session + ":" << tr("Add Indicator");

  ScriptRunDialog *dialog = new ScriptRunDialog(0,
                                                settings.value("add_indicator_last_script").toString(),
                                                settings.value("add_indicator_last_command", "perl").toString());
  connect(dialog, SIGNAL(signalDone(QString, QString)), this, SLOT(addIndicator2(QString, QString)));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->setWindowTitle(wt.join(" "));
  dialog->show();
}

void AddIndicator::addIndicator2 (QString command, QString file)
{
  IndicatorDataBase i;
  if (i.add(command, file))
  {
    done();
    return;
  }

  QSettings settings(g_localSettings);
  settings.setValue("add_indicator_last_script", file);
  settings.setValue("add_indicator_last_command", command);
  settings.sync();

  // launch indicator
  Script *script = new Script(g_parent);
  script->setFile(file);
  script->setCommand(command);
  script->setSymbol(g_currentSymbol);
  script->run();

  done();
}

void AddIndicator::done ()
{
  deleteLater();
}
