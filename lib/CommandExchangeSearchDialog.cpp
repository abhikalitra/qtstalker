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

#include "CommandExchangeSearchDialog.h"
#include "Script.h"
#include "SettingString.h"
#include "ExchangeSearchDialog.h"

#include <QtDebug>

CommandExchangeSearchDialog::CommandExchangeSearchDialog (QObject *p) : Command (p)
{
  _type = "EXCHANGE_SEARCH_DIALOG";
  _isDialog = 1;
}

int CommandExchangeSearchDialog::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

//  QStringList l;
//  l << "QtStalker" + script->session() + ":" << tr("Select File");

  ExchangeSearchDialog dialog(0);
  int rc = dialog.exec();
  if (rc == QDialog::Rejected)
    return _ERROR;

  Setting *set = sg->get("EXCHANGE");
  set->setString(dialog.exchange());

  return _OK;
}

SettingGroup * CommandExchangeSearchDialog::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(QString());
  ss->setKey("EXCHANGE");
  sg->set(ss);

  return sg;
}
