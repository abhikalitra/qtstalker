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
#include <QLabel>
#include <QMessageBox>

GroupNewDialog::GroupNewDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle(tr("QtStalker: New Group"));

  createMainPage();
}

void GroupNewDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // name
  QLabel *label = new QLabel(tr("Group Name"));
  grid->addWidget(label, row, col++);

  _name = new QLineEdit;
  grid->addWidget(_name, row++, col--);
  
  grid->setRowStretch(row, 1);

  _tabs->addTab(w, QString());
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
    QMessageBox *dialog = new QMessageBox(this);
    dialog->setWindowTitle(tr("Qtstalker: New Group"));
    dialog->setText(tr("A group with this name already exists."));
    dialog->setWindowModality(Qt::NonModal);
    connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
    dialog->show();
    return;
  }

  Group g;
  g.setName(name);
  g_mutex.lock();
  db.transaction();
  db.setGroup(g);
  db.commit();
  g_mutex.unlock();

  QStringList ml;
  ml << tr("Group") << name << tr("created");
  emit signalMessage(ml.join(" "));

  emit signalNewGroup();
  
  accept();
}

