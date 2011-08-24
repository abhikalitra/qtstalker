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

#include "CommandFileDialog.h"
#include "Script.h"
#include "SettingFile.h"

#include <QtDebug>
#include <QFileDialog>

CommandFileDialog::CommandFileDialog (QObject *p) : Command (p)
{
  _type = "FILE_DIALOG";
  _isDialog = 1;
}

int CommandFileDialog::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QStringList l;
  l << "QtStalker" + script->session() + ":" << tr("Select File");

  QStringList files = QFileDialog::getOpenFileNames(0,
                                                    tr("Select file(s)"),
                                                    QString(),
                                                    QString());

  if (! files.count())
    return _ERROR;

  Setting *set = sg->get("FILES");
  set->setFile(files);

  return _OK;
}

SettingGroup * CommandFileDialog::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingFile *sf = new SettingFile(QStringList());
  sf->setKey("FILES");
  sg->set(sf);

  return sg;
}
