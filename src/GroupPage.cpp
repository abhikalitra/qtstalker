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
#include "HelpWindow.h"
#include "help.xpm"
#include "delete.xpm"
#include "newchart.xpm"
#include "insert.xpm"
#include "stop.xpm"
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
#include <qaccel.h>

GroupPage::GroupPage (QWidget *w) : QWidget (w)
{
  keyFlag = FALSE;
  macroFlag = FALSE;
  macro = 0;
  
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
  connect(nav, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  nav->updateList();
  nav->setId(Macro::GroupPage);
  vbox->addWidget(nav);

  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Group		Ctrl+N"), this, SLOT(newGroup()));
  menu->insertItem(QPixmap(insert), tr("&Add Group Items	Ctrl+A"), this, SLOT(addGroupItem()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Group Items	Ctrl+D"), this, SLOT(deleteGroupItem()));
  menu->insertItem(QPixmap(stop), tr("De&lete Group	Ctrl+L"), this, SLOT(deleteGroup()));
  menu->insertItem(QPixmap(renam), tr("&Rename Group	Ctrl+R"), this, SLOT(renameGroup()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  a->insertItem(CTRL+Key_N, NewGroup);
  a->insertItem(CTRL+Key_A, AddGroupItem);
  a->insertItem(CTRL+Key_D, DeleteGroupItem);
  a->insertItem(CTRL+Key_L, DeleteGroup);
  a->insertItem(CTRL+Key_R, RenameGroup);
  a->insertItem(CTRL+Key_H, Help);
  
  groupNoSelection();
}

GroupPage::~GroupPage ()
{
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
  menu->setItemEnabled(menu->idAt(2), TRUE);
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
    menu->setItemEnabled(menu->idAt(2), TRUE);
    menu->setItemEnabled(menu->idAt(3), TRUE);
    menu->setItemEnabled(menu->idAt(4), TRUE);
  }
  else
  {
    group->clear();

    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
    menu->setItemEnabled(menu->idAt(3), FALSE);
    menu->setItemEnabled(menu->idAt(4), FALSE);
  }
}

void GroupPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
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

void GroupPage::setKeyFlag (bool d)
{
  keyFlag = d;
  nav->setKeyFlag(d);
}

void GroupPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        slotAccel(NewGroup);
	break;
      case Qt::Key_A:
        slotAccel(AddGroupItem);
	break;
      case Qt::Key_D:
        slotAccel(DeleteGroupItem);
	break;
      case Qt::Key_L:
        slotAccel(DeleteGroup);
	break;
      case Qt::Key_R:
        slotAccel(RenameGroup);
	break;
      case Qt::Key_H:
        slotAccel(Help);
	break;
      default:
        break;
    }
  }
  else
    nav->doKeyPress(key);
}

void GroupPage::slotAccel (int id)
{
  switch (id)
  {
    case NewGroup:
      if (keyFlag)
        emit signalKeyPressed (Macro::GroupPage, ControlButton, Key_N, 0, QString());
      newGroup();
      break;  
    case AddGroupItem:
      if (keyFlag)
        emit signalKeyPressed (Macro::GroupPage, ControlButton, Key_A, 0, QString());
      addGroupItem();
      break;  
    case DeleteGroupItem:
      if (keyFlag)
        emit signalKeyPressed (Macro::GroupPage, ControlButton, Key_D, 0, QString());
      deleteGroupItem();
      break;  
    case DeleteGroup:
      if (keyFlag)
        emit signalKeyPressed (Macro::GroupPage, ControlButton, Key_L, 0, QString());
      deleteGroup();
      break;  
    case Help:
      slotHelp();
      break;  
    case RenameGroup:
      if (keyFlag)
        emit signalKeyPressed (Macro::GroupPage, ControlButton, Key_R, 0, QString());
      renameGroup();
      break;
    default:
      break;
  }
}

void GroupPage::runMacro (Macro *d)
{
  macro = d;
  macroFlag = TRUE;
  
  while (macro->getZone(macro->getIndex()) == Macro::GroupPage)
  {
    doKeyPress(macro->getKey(macro->getIndex()));
    
    macro->incIndex();
    if (macro->getIndex() >= macro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}

