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

#include "CommandSymbolDialog.h"
#include "Script.h"
#include "SettingList.h"
#include "SymbolDialog.h"

#include <QtDebug>

CommandSymbolDialog::CommandSymbolDialog (QObject *p) : Command (p)
{
  _type = "SYMBOL_DIALOG";
  _isDialog = 1;
}

int CommandSymbolDialog::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  SymbolDialog dialog(0);
  int rc = dialog.exec();
  if (rc == QDialog::Rejected)
    return _ERROR;

  Setting *set = sg->get("SYMBOLS");
  set->setList(dialog.symbols());

  return _OK;
}

SettingGroup * CommandSymbolDialog::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingList *sl = new SettingList(QStringList(), QString());
  sl->setKey("SYMBOLS");
  sg->set(sl);

  return sg;
}
