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

#include "TestPage.h"
#include "Tester.h"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include "SymbolDialog.h"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qlayout.h>
#include <qdir.h>

TestPage::TestPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  list = new QListBox(this);
  connect(list, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(testSelected(const QString &)));
  vbox->addWidget(list);
  
  menu = new QPopupMenu();
  menu->insertItem(QPixmap(open), tr("&Open Backtest Rule"), this, SLOT(openTest()), CTRL+Key_O);
  menu->insertItem(QPixmap(newchart), tr("&New Backtest Rule"), this, SLOT(newTest()), CTRL+Key_N);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Backtest Rule"), this, SLOT(deleteTest()), CTRL+Key_D);
  menu->insertItem(QPixmap(renam), tr("&Rename Backtest Rule"), this, SLOT(renameTest()), CTRL+Key_R);

  updateList();
  testNoSelection();
}

TestPage::~TestPage ()
{
  delete menu;
}

void TestPage::openTest ()
{
  Tester *dialog = new Tester(list->currentText());
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
  dialog->show();
}

void TestPage::deleteTest()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  config.getData(Config::TestPath),
					  "*",
					  QFileDialog::DirectoryOnly);
  dialog->setCaption(tr("Select Backtest rule To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete backtest rule?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QString s = "rm -r " + dialog->selectedFile();
    
    if (system(s.latin1()) == -1)
      qDebug("TestPage::deleteTest:command failed");
    
    updateList();

    testNoSelection();
  }

  delete dialog;
}

void TestPage::renameTest ()
{
  if (list->currentItem() == -1)
    return;

  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Backtest Rule"),
  					    tr("Enter new backtest rule name."),
  					    QLineEdit::Normal,
					    list->currentText(),
					    &ok,
					    this);

  if ((! ok) || (selection.isNull()))
    return;

  QString s = config.getData(Config::TestPath);
  s.append("/");
  s.append(selection);
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  QString s2 = config.getData(Config::TestPath);
  s2.append("/");
  s2.append(list->currentText());
  dir.rename(s2, s, TRUE);
  
  list->changeItem(selection, list->currentItem());
}

void TestPage::testSelected (const QString &) 
{
  menu->setItemEnabled(menu->idAt(0), TRUE);
  menu->setItemEnabled(menu->idAt(2), TRUE);
  menu->setItemEnabled(menu->idAt(3), TRUE);
}

void TestPage::testNoSelection ()
{
  menu->setItemEnabled(menu->idAt(0), FALSE);
  menu->setItemEnabled(menu->idAt(2), FALSE);
  menu->setItemEnabled(menu->idAt(3), FALSE);
}

void TestPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void TestPage::updateList ()
{
  list->clear();
  
  QDir dir(config.getData(Config::TestPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    list->insertItem(dir[loop], -1);
}


