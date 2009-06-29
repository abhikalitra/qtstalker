/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "TestPage.h"
#include "../pics/open.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "DataBase.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QCursor>
#include <QMenu>
#include <QVBoxLayout>
#include <stdlib.h>
#include <QLayout>


TestPage::TestPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(5);
  setLayout(vbox);
  
  list = new QListWidget;
  list->setContextMenuPolicy(Qt::CustomContextMenu);
  list->setSortingEnabled(TRUE);
  connect(list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(doubleClick(QListWidgetItem *)));
  vbox->addWidget(list);
  
  menu = new QMenu(this);
  QAction *action = menu->addAction(QIcon(newchart), tr("&New Rule		Ctrl+N"), this, SLOT(newTest()), QKeySequence(Qt::CTRL+Qt::Key_N));
  actionList.append(action);
  action = menu->addAction(QIcon(openp), tr("&Open Rule		Ctrl+O"), this, SLOT(openTest()), QKeySequence(Qt::CTRL+Qt::Key_O));
  actionList.append(action);
  action = menu->addAction(QIcon(deleteitem), tr("&Delete Rule	Ctrl+D"), this, SLOT(deleteTest()), QKeySequence(Qt::CTRL+Qt::Key_D));
  actionList.append(action);

  updateList();

  testNoSelection();
}

TestPage::~TestPage ()
{
}

void TestPage::openTest ()
{
  Tester *dialog = new Tester(list->currentItem()->text());
//  connect(menubar, SIGNAL(signalScale(bool)), dialog, SLOT(slotScaleToScreen(bool)));
  dialog->show();
}

void TestPage::newTest()
{
  Tester *dialog = new Tester;
  QString name = dialog->newTest();
  delete dialog;
  
  if (! name.length())
    return;
  
  updateList();
  
  dialog = new Tester(name);
//  connect(menubar, SIGNAL(signalScale(bool)), dialog, SLOT(slotScaleToScreen(bool)));
  dialog->show();
}

void TestPage::deleteTest()
{
  DataBase db;
  QStringList l;
  bool ok;
  db.getTestList(l);
  QString item = QInputDialog::getItem(this, QString(tr("Delete Tester")), QString(tr("Select tester to delete")), l, 0, FALSE, &ok, 0);
  if (! item.length())
    return;

  int rc = QMessageBox::warning(this,
		      	        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to delete this tester?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  db.deleteTest(item);

  updateList();

  testNoSelection();
}

void TestPage::testSelected () 
{
  if (! list->currentItem())
    return;

  int loop;
  for (loop = 1; loop < 3; loop++)
    actionList.at(loop)->setEnabled(TRUE);
}

void TestPage::testNoSelection ()
{
  int loop;
  for (loop = 1; loop < 3; loop++)
    actionList.at(loop)->setEnabled(FALSE);
}

void TestPage::rightClick (const QPoint &)
{
  menu->exec(QCursor::pos());
}

void TestPage::updateList ()
{
  list->clear();

  DataBase db;
  QStringList l;
  db.getTestList(l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
    list->addItem(l[loop]);
}

void TestPage::slotMessage (QString d)
{
  emit message(d);
}

void TestPage::doubleClick (QListWidgetItem *item)
{
  if (! item)
    return;
    
  Tester *dialog = new Tester(item->text());
  dialog->show();
}

void TestPage::keyPressEvent (QKeyEvent *key)
{
  doKeyPress(key);
}

void TestPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->modifiers() == Qt::ControlModifier)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        newTest();
	break;
      case Qt::Key_D:
        deleteTest();
	break;
      case Qt::Key_O:
        openTest();
	break;
      default:
        break;
    }
  }
  else
  {
    switch (key->key())
    {
      case Qt::Key_Delete:
        deleteTest();
        break;
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        break;      
      case Qt::Key_Enter:
      case Qt::Key_Return:
        openTest();
        break;
      default:
//        QListBox::keyPressEvent(key);
        break;
    }
  }
}


