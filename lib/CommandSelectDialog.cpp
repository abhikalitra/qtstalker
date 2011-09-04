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

#include <QtDebug>
#include <QDialog>

CommandSelectDialog::CommandSelectDialog (QObject *p) : Command (p)
{
  _type = "SELECT_DIALOG";
  _isDialog = 1;
}

int CommandSelectDialog::runScript (Data *sg, Script *script)
{
  // mode
  bool mode = sg->get("MODE").toBool();

  // title
  QString title = sg->get("TITLE").toString();

  // list
  QStringList items = sg->get("LIST").toStringList();

  QStringList l;
  l << "QtStalker" + script->session() + ":" << tr("Select");

  SelectDialog *dialog = new SelectDialog(0);
  dialog->setWindowTitle(l.join(" "));
  dialog->setMode(mode);
  dialog->setTitle(title);
  dialog->setItems(items);

  int rc = dialog->exec();
  if (rc == QDialog::Accepted)
    sg->set("SELECTED", QVariant(dialog->selected()));
  else
    return _ERROR;

  return _OK;
}

Data * CommandSelectDialog::settings ()
{
  Data *sg = new Data;
  sg->set("LIST", QVariant(QStringList()));
  sg->set("TITLE", QVariant(QString()));
  sg->set("MODE", QVariant(FALSE));
  sg->set("SELECTED", QVariant(QStringList()));
  return sg;
}
