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

#include "CommandSelectDialog.h"
#include "SelectDialog.h"
#include "Script.h"
#include "SettingString.h"
#include "SettingList.h"
#include "SettingBool.h"

#include <QtDebug>
#include <QDialog>

CommandSelectDialog::CommandSelectDialog (QObject *p) : Command (p)
{
  _type = "SELECT_DIALOG";
  _isDialog = 1;
}

int CommandSelectDialog::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  // mode
  bool mode = sg->get("MODE")->getBool();

  // title
  QString title = sg->get("TITLE")->getString();

  // list
  QString key = sg->get("LIST")->getString();
  QStringList items;
  Setting *set = script->setting(key);
  if (set)
    items = set->getList();

  QStringList l;
  l << "QtStalker" + script->session() + ":" << tr("Select");

  SelectDialog *dialog = new SelectDialog(0);
  dialog->setWindowTitle(l.join(" "));
  dialog->setMode(mode);
  dialog->setTitle(title);
  dialog->setItems(items);
//  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
//  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
//  connect(dialog, SIGNAL(accepted()), this, SIGNAL(signalResume()));
//  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(command2(QStringList)));

qDebug() << "CommandSelectDialog::runScript: about to show dialog";

  int rc = dialog->exec();
  if (rc == QDialog::Accepted)
  {
    Setting *sl = sg->get("SELECTED");
    sl->setList(dialog->selected());
  }
  else
    return _ERROR;

  return _OK;
}

SettingGroup * CommandSelectDialog::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_LIST, Setting::_NONE, QString());
  ss->setKey("LIST");
  sg->set(ss);

  ss = new SettingString(QString());
  ss->setKey("TITLE");
  sg->set(ss);

  SettingBool *sb = new SettingBool(FALSE);
  sb->setKey("MODE");
  sg->set(sb);

  SettingList *sl = new SettingList(Setting::_NONE, Setting::_LIST, QStringList(), QString());
  sl->setKey("SELECTED");
  sg->set(sl);

  return sg;
}
