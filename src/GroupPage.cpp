/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "GroupPage.h"
#include "SymbolDialog.h"
#include "delete.xpm"
#include "newchart.xpm"
#include "insert.xpm"
#include "rename.xpm"
#include "stop.xpm"
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qinputdialog.h>
#include <qstringlist.h>
#include <qcursor.h>
#include <stdlib.h>
#include <qtooltip.h>

GroupPage::GroupPage (QWidget *w, Config *c) : BaseDialog(w)
{
  config = c;

  group = new QLineEdit(this);
  group->setReadOnly(TRUE);
  QToolTip::add(group, tr("Current Group"));
  basebox->addWidget(group);

  nav = new Navigator(this, config->getData(Config::GroupPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(groupSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(groupNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  nav->updateList();
  basebox->addWidget(nav);

  menu = new QPopupMenu();
  menu->insertItem(QPixmap(newchart), tr("New Group"), this, SLOT(newGroup()));
  menu->insertItem(QPixmap(insert), tr("Add Group Items"), this, SLOT(addGroupItem()));
  menu->insertItem(QPixmap(deletefile), tr("Delete Group Items"), this, SLOT(deleteGroupItem()));
  menu->insertItem(QPixmap(stop), tr("Delete Group"), this, SLOT(deleteGroup()));
  menu->insertItem(QPixmap(renam), tr("Rename Group"), this, SLOT(renameGroup()));

  groupNoSelection();
}

GroupPage::~GroupPage ()
{
  delete menu;
}

void GroupPage::newGroup()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Group"),
  							   tr("Enter new group symbol."),
							   QLineEdit::Normal,
							   tr("New Group"),
							   &ok,
							   this);
  if ((! ok) || (selection.isNull()))
    return;

  QString s = config->getData(Config::GroupPath);
  s.append("/");
  s.append(selection);
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  dir.mkdir(s, TRUE);
  nav->updateList();
}

void GroupPage::addGroupItem()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  							   config->getData(Config::DataPath),
							   "*");

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QFileInfo fi(nav->getCurrentPath());

    QString s = config->getData(Config::GroupPath);
    s.append("/");
    s.append(fi.fileName());

    int loop;
    QStringList l = dialog->selectedFiles();
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      QString s2 = "ln -s ";
      s2.append(l[loop]);
      s2.append(" ");
      s2.append(s);
      s2.append("/");
      QFileInfo fi(l[loop]);
      s2.append(fi.fileName());
      system (s2);
    }

    nav->updateList();
  }

  delete dialog;
}

void GroupPage::deleteGroupItem()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  							   nav->getCurrentPath(),
							   "*");
  dialog->setCaption(tr("Select Group Items To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  					  tr("Qtstalker: Warning"),
					  tr("Are you sure you want to delete group items?"),
					  QMessageBox::Yes,
					  QMessageBox::No,
					  QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QStringList l = dialog->selectedFiles();
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
      dir.remove(l[loop], TRUE);

    nav->updateList();

    groupNoSelection();
  }
  
  delete dialog;
}

void GroupPage::deleteGroup()
{
  int rc = QMessageBox::warning(this,
  					    tr("Qtstalker: Warning"),
					    tr("Are you sure you want to delete this group?"),
					    QMessageBox::Yes,
					    QMessageBox::No,
					    QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  QFileInfo fi(nav->getCurrentPath());

  QString s =  "rm -r ";
  s.append(config->getData(Config::GroupPath));
  s.append("/");
  s.append(fi.fileName());
  system (s);

  s = config->getData(Config::GroupPath);
  s.append("/x");
  nav->upDirectory();
  nav->updateList();
  groupNoSelection();
}

void GroupPage::renameGroup ()
{
  QFileInfo fi(nav->getCurrentPath());
  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Group"),
  							   tr("Enter new group symbol."),
							   QLineEdit::Normal,
							   fi.fileName(),
							   &ok,
							   this);
  if ((ok) && (! selection.isNull()))
  {
    QString s = config->getData(Config::GroupPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This chart group exists."));
      return;
    }

    dir.rename(fi.absFilePath(), s, TRUE);

    s.append("/x");
    nav->setDirectory(s);
  }
}

void GroupPage::groupSelected (QString d)
{
  menu->setItemEnabled(menu->idAt(1), TRUE);
  menu->setItemEnabled(menu->idAt(3), TRUE);
  menu->setItemEnabled(menu->idAt(4), TRUE);
  emit fileSelected(d);
}

void GroupPage::groupNoSelection ()
{
  QString s = config->getData(Config::GroupPath);
  QString s2 = nav->getCurrentPath();
  if (s.compare(s2))
  {
    group->setText(s2.right(s2.length() - s.length() - 1));

    menu->setItemEnabled(menu->idAt(1), TRUE);
    menu->setItemEnabled(menu->idAt(3), TRUE);
    menu->setItemEnabled(menu->idAt(4), TRUE);
  }
  else
  {
    group->clear();

    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(3), FALSE);
    menu->setItemEnabled(menu->idAt(4), FALSE);
  }
}

void GroupPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

