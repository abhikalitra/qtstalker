/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
#include "up.xpm"
#include "newchart.xpm"
#include "insert.xpm"
#include "rename.xpm"
#include "stop.xpm"
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qinputdialog.h>
#include <qstringlist.h>

GroupPage::GroupPage (QWidget *w, Config *c) : BaseDialog(w)
{
  config = c;

  nav = new Navigator(this, config->getData(Config::GroupPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(groupSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(groupNoSelection()));
  connect(nav, SIGNAL(directoryStatus(bool)), this, SLOT(directoryStatus(bool)));
  nav->updateList();
  basebox->addWidget(nav);

  setButton(QPixmap(up), tr("Up Directory"), 0);
  connect(getButton(0), SIGNAL(clicked()), nav, SLOT(upDirectory()));
  setButtonStatus(0, FALSE);

  setButton(QPixmap(newchart), tr("New Group"), 1);
  connect(getButton(1), SIGNAL(clicked()), this, SLOT(newGroup()));
  setButtonStatus(1, TRUE);

  setButton(QPixmap(insert), tr("Add Group Items"), 2);
  connect(getButton(2), SIGNAL(clicked()), this, SLOT(addGroupItem()));
  setButtonStatus(2, FALSE);

  setButton(QPixmap(deletefile), tr("Delete Group Items"), 3);
  connect(getButton(3), SIGNAL(clicked()), this, SLOT(deleteGroupItem()));
  setButtonStatus(3, FALSE);

  setButton(QPixmap(stop), tr("Delete Group"), 4);
  connect(getButton(4), SIGNAL(clicked()), this, SLOT(deleteGroup()));
  setButtonStatus(4, FALSE);

  setButton(QPixmap(renam), tr("Rename Group"), 5);
  connect(getButton(5), SIGNAL(clicked()), this, SLOT(renameGroup()));
  setButtonStatus(5, FALSE);
}

GroupPage::~GroupPage ()
{
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

  QStringList l = config->getGroup(selection);
  if (l.count() != 0)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  l.clear();
  config->setGroup(selection, l);
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
    config->setGroup(fi.fileName(), dialog->selectedFiles());
    nav->updateList();
  }

  delete dialog;
}

void GroupPage::deleteGroupItem()
{
  QString s = nav->getFileSelection();
  if (! s.length())
    return;

  QDir dir(s);
  dir.remove(s, TRUE);

  nav->updateList();

  groupNoSelection();
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
  config->deleteGroup(fi.fileName());

  QString s = config->getData(Config::GroupPath);
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
    QStringList l = config->getGroup(selection);
    if (l.count() != 0)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This chart group exists."));
      return;
    }

    QDir dir;
    QString s = config->getData(Config::GroupPath);
    s.append("/");
    s.append(selection);
    dir.rename(fi.absFilePath(), s, TRUE);

    s.append("/x");
    nav->setDirectory(s);
  }
}

void GroupPage::groupSelected (QString d)
{
  setButtonStatus(2, TRUE);
  setButtonStatus(3, TRUE);
  setButtonStatus(4, TRUE);
  setButtonStatus(5, TRUE);
  emit fileSelected(d);
}

void GroupPage::groupNoSelection ()
{
  QString s = config->getData(Config::GroupPath);
  if (s.compare(nav->getCurrentPath()))
  {
    setButtonStatus(2, TRUE);
    setButtonStatus(3, FALSE);
    setButtonStatus(4, TRUE);
    setButtonStatus(5, TRUE);
  }
  else
  {
    setButtonStatus(2, FALSE);
    setButtonStatus(3, FALSE);
    setButtonStatus(4, FALSE);
    setButtonStatus(5, FALSE);
  }
}

void GroupPage::directoryStatus (bool d)
{
  setButtonStatus(0, d);
}

