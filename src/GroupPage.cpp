/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "ChartDb.h"
#include "HelpWindow.h"
#include "help.xpm"
#include "delete.xpm"
#include "newchart.xpm"
#include "insert.xpm"
#include "stop.xpm"
#include "edit.xpm"
#include "rename.xpm"
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qinputdialog.h>
#include <qstringlist.h>
#include <qcursor.h>
#include <stdlib.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qfileinfo.h>

GroupPage::GroupPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);

  group = new QLineEdit(this);
  group->setReadOnly(TRUE);
  QToolTip::add(group, tr("Current Group"));
  group->setFocusPolicy(QWidget::NoFocus);
  vbox->addWidget(group);
  
  nav = new Navigator(this, config.getData(Config::GroupPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(groupSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(groupNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(nav, SIGNAL(keyPress(QKeyEvent *)), this, SLOT(doKeyPress(QKeyEvent *)));
  nav->updateList();
  nav->setId(1);
  vbox->addWidget(nav);

  menu = new QPopupMenu();
  menu->insertItem(QPixmap(newchart), tr("&New Group"), this, SLOT(newGroup()), CTRL+Key_N);
  menu->insertItem(QPixmap(insert), tr("&Add Group Items"), this, SLOT(addGroupItem()), CTRL+Key_A);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Group Items"), this, SLOT(deleteGroupItem()), CTRL+Key_D);
  menu->insertItem(QPixmap(stop), tr("Delete &Group"), this, SLOT(deleteGroup()), CTRL+Key_G);
  menu->insertItem(QPixmap(renam), tr("&Rename Group"), this, SLOT(renameGroup()), CTRL+Key_R);
  menu->insertItem(tr("&Refresh"), this, SLOT(refreshList()), CTRL+Key_F);
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(edit), tr("&Edit Chart"), this, SLOT(editChart()), CTRL+Key_E);
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help"), this, SLOT(slotHelp()), CTRL+Key_H);

  groupNoSelection();
}

GroupPage::~GroupPage ()
{
  delete menu;
}

void GroupPage::newGroup()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Group"),
  				    tr("Enter new group symbol."),
				    QLineEdit::Normal,
				    tr("NewGroup"),
				    &ok,
				    this);
  if ((! ok) || (s.isNull()))
    return;

  int loop;
  QString selection;
  for (loop = 0; loop < (int) s.length(); loop++)
  {
    QChar c = s.at(loop);
    if (c.isLetterOrNumber())
      selection.append(c);
  }
  
  s = nav->getCurrentPath();
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
  					  config.getData(Config::DataPath),
					  "*",
					  QFileDialog::ExistingFiles);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    int loop;
    QStringList l = dialog->selectedFiles();
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      QFileInfo fi(l[loop]);
    
      QString s = "ln -s ";
      s.append(l[loop]);
      s.append(" ");
      s.append(nav->getCurrentPath());
      s.append("/");
      s.append(fi.fileName());
      system (s);
    }

    nav->updateList();
  }

  delete dialog;
}

void GroupPage::deleteGroupItem()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  nav->getCurrentPath(),
					  "*",
					  QFileDialog::ExistingFiles);
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
    {
      if (! dir.remove(l[loop], TRUE))
        qDebug("GroupPage::deleteGroupItem:failed to delete symlink");
    }

    nav->updateList();

    groupNoSelection();
  }
  
  delete dialog;
}

void GroupPage::deleteGroup()
{
  QFileInfo fi(nav->getCurrentPath());
  QString s = tr("Are you sure you want to delete current group ");
  s.append(fi.fileName());
  int rc = QMessageBox::warning(this,
  				tr("Qtstalker: Warning"),
				s,
				QMessageBox::Yes,
				QMessageBox::No,
				QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  s = nav->getCurrentPath();
  if (! s.compare(config.getData(Config::GroupPath)))
    return;
  
  s =  "rm -r ";
  s.append(nav->getCurrentPath());
  system (s);
  nav->upDirectory();
  nav->updateList();
  groupNoSelection();
}

void GroupPage::renameGroup ()
{
  QFileInfo fi(nav->getCurrentPath());
  bool ok;
  QString s = QInputDialog::getText(tr("Rename Group"),
  				    tr("Enter new group symbol."),
				    QLineEdit::Normal,
				    fi.fileName(),
				    &ok,
				    this);
  if ((! ok) || (s.isNull()))
    return;

  int loop;
  QString selection;
  for (loop = 0; loop < (int) s.length(); loop++)
  {
    QChar c = s.at(loop);
    if (c.isLetterOrNumber())
      selection.append(c);
  }
        
  s = nav->getCurrentPath();
    
  QStringList l = QStringList::split("/", s, FALSE);
  l[l.count() - 1] = selection;
  QString s2 = l.join("/");
  s2.prepend("/");
  
  QDir dir(s2);
  if (dir.exists(s2, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This chart group exists."));
    return;
  }

  if (! dir.rename(s, s2, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Group rename failed."));
    return;
  }
  
  nav->setDirectory(s2 + "/x");
  nav->updateList();
  
  groupNoSelection();
}

void GroupPage::groupSelected (QString d)
{
  menu->setItemEnabled(menu->idAt(1), TRUE);
  menu->setItemEnabled(menu->idAt(3), TRUE);
  menu->setItemEnabled(menu->idAt(4), TRUE);
  
  QFileInfo fi(d);
  emit fileSelected(fi.readLink());
}

void GroupPage::groupNoSelection ()
{
  QString s = config.getData(Config::GroupPath);
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

void GroupPage::editChart ()
{
  QString symbol = nav->getFileSelection();
  if (! symbol.length())
    return;

  QFileInfo fi(symbol);
  ChartDb *db = new ChartDb;
  db->dbPrefDialog(fi.readLink());
  delete db;
}

void GroupPage::refreshList ()
{
  nav->updateList();
}

void GroupPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithgroups.html");
  hw->show();
}

void GroupPage::setFocus ()
{
  nav->setFocus();
}

void GroupPage::doKeyPress (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_N:
      newGroup();
      break;
    case Qt::Key_A:
      addGroupItem();
      break;
    case Qt::Key_Delete:
      deleteGroupItem();
      break;
    case Qt::Key_G:
      deleteGroup();
      break;
    case Qt::Key_R:
      renameGroup();
      break;
    case Qt::Key_F:
      refreshList();
      break;
    case Qt::Key_E:
      editChart();
      break;
    case Qt::Key_H:
      slotHelp();
      break;
    default:
      break;
  }
}

Navigator * GroupPage::getNav ()
{
  return nav;
}

