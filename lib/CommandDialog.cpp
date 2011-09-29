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
  _name = "DIALOG";
  _type = _DIALOG;
}

int CommandDialog::runScript (Message *sg, Script *script)
{
  DataDialog *dialog = new DataDialog(_widgetParent);
  connect(dialog, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
  connect(dialog, SIGNAL(rejected()), this, SLOT(dialogRejected()));

  // verify TITLE
  QStringList l;
  l << "QtStalker:" << sg->value("TITLE");
  dialog->setWindowTitle(l.join(" "));

  int loop = 0;
  for (; loop < 20; loop++)
  {
    // get DATA_SETTING
    QString s = "SETTING_" + QString::number(loop);
    QString s2 = sg->value(s);
    Data *d = script->data(s2);
    if (! d)
      continue;

    if (d->type() != DataFactory::_DATA_SETTING)
    {
      qDebug() << "CommandDialog::runScript: invalid" << s;
      continue;
    }

    dialog->set(d);
  }

  for (loop = 0; loop < 5; loop++)
  {
    QString s = "TAB_TITLE_" + QString::number(loop);
    QString s2 = sg->value(s);
    if (s2.isEmpty())
      continue;

    dialog->setTabTitle(loop, s2);
  }

  dialog->show();

  return _OK;
}

void CommandDialog::dialogAccepted ()
{
  _returnString = "OK";
  emit signalResume((void *) this);
}

void CommandDialog::dialogRejected ()
{
  emit signalResume((void *) this);
}
