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

#include "CommandDialog.h"
#include "DataDialog.h"
#include "SettingString.h"
#include "DataFactory.h"

#include <QtDebug>
#include <QDialog>

CommandDialog::CommandDialog (QObject *p) : Command (p)
{
  _type = "DIALOG";
}

int CommandDialog::runScript (Message *sg, Script *script)
{
  DataDialog dialog(0);
  dialog.addTab(0, tr("Settings"));

  // verify TITLE
  QStringList l;
  l << "QtStalker" + script->session() + ":" << sg->value("TITLE");
  dialog.setWindowTitle(l.join(" "));

  int loop = 0;
  for (; loop < 10; loop++)
  {
    // get DATA_SETTING
    QString s = "SETTING_" + QString::number(loop);
    QString s2 = sg->value(s);
    Data *d = script->data(s2);
    if (! d)
      continue;

    if (d->type() != DataFactory::_DATA_SETTING)
    {
      _message << s + " invalid";
      continue;
    }

    dialog.set(d);
  }

  int rc = dialog.exec();

  if (rc == QDialog::Rejected)
    return _ERROR;

  return _OK;
}
