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

#include "GroupNewDialog.h"
#include "GroupDataBase.h"
#include "Globals.h"

#include <QtDebug>

GroupNewDialog::GroupNewDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Group"));

  createMainPage();
}

void GroupNewDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  w->setLayout(vbox);

  _name = new QLineEdit;
  vbox->addWidget(_name);
  
  _tabs->addTab(w, tr("Group Name"));
}

void GroupNewDialog::done ()
{
  QString name = _name->text();
  if (name.isEmpty())
  {
    reject();
    return;
  }

  // remove any forbidden sql characters
  name = name.remove(QString("'"), Qt::CaseSensitive);

  QStringList l;
  GroupDataBase db;
  db.getAllGroupsList(l);
  if (l.contains(name))
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Error New Group"));
    dialog->setMessage(tr("A group with this name already exists."));
    dialog->show();
    return;
  }

  Group g;
  g.setName(name);
  db.transaction();
  db.setGroup(g);
  db.commit();

  QStringList ml;
  ml << tr("Group") << name << tr("created");
  emit signalMessage(ml.join(" "));

  emit signalNewGroup();
  
  accept();
}

