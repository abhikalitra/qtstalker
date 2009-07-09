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
  connect(list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  vbox->addWidget(list);
  
  menu = new QMenu(this);
  QAction *action = menu->addAction(QIcon(newchart), tr("&New Rule		Ctrl+N"), this, SLOT(newTest()), QKeySequence(Qt::CTRL+Qt::Key_N));
  actionList.append(action);
  action = menu->addAction(QIcon(openp), tr("&Open Rule		Ctrl+O"), this, SLOT(openTest()), QKeySequence(Qt::CTRL+Qt::Key_O));
  actionList.append(action);
  action = menu->addAction(QIcon(deleteitem), tr("&Delete Rule	Ctrl+D"), this, SLOT(deleteTest()), QKeySequence(Qt::CTRL+Qt::Key_D));
  actionList.append(action);

  updateList();

  selectionChanged();
}

TestPage::~TestPage ()
{
}

void TestPage::openTest ()
{
  QString s = list->currentItem()->text();
  Tester *dialog = new Tester(s);
  connect(dialog, SIGNAL(signalSaved()), this, SLOT(updateList()));
  dialog->show();
}

void TestPage::newTest()
{
  bool ok;
  QString s = QInputDialog::getText(this, tr("New Backtest Rule"), tr("Enter new backtest rule name."), QLineEdit::Normal, tr("NewRule"), &ok);
  if ((! ok) || (s.isEmpty()))
    return;

  DataBase db;
  QStringList l;
  db.getTestList(l);
  if (l.contains(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }
  
  Tester *dialog = new Tester(s);
  connect(dialog, SIGNAL(signalSaved()), this, SLOT(updateList()));
  dialog->show();
}

void TestPage::deleteTest()
{
  DataBase db;
  QStringList l;
  bool ok;
  db.getTestList(l);
  QString item = QInputDialog::getItem(this, tr("Delete Tester"), tr("Select tester to delete"), l, 0, FALSE, &ok, 0);
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

  selectionChanged();
}

void TestPage::selectionChanged () 
{
  bool flag = FALSE;
  QListWidgetItem *item = list->currentItem();
  if (item)
    flag = TRUE;

  int loop;
  for (loop = 1; loop < 3; loop++)
    actionList.at(loop)->setEnabled(flag);
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

  QString s = item->text();     
  Tester *dialog = new Tester(s);
  connect(dialog, SIGNAL(signalSaved()), this, SLOT(updateList()));
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


