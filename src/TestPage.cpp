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

#include "TestPage.h"
#include "Tester.h"
#include "SymbolDialog.h"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>

TestPage::TestPage (QWidget *w, Config *c) : BaseDialog (w)
{
  config = c;

  nav = new Navigator(this, config->getData(Config::TestPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(testSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(testNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  nav->updateList();
  basebox->addWidget(nav);
  
  menu = new QPopupMenu();
  menu->insertItem(QPixmap(open), tr("Open Backtest Rule"), this, SLOT(openTest()));
  menu->insertItem(QPixmap(newchart), tr("New Backtest Rule"), this, SLOT(newTest()));
  menu->insertItem(QPixmap(deletefile), tr("Delete Backtest Rule"), this, SLOT(deleteTest()));
  menu->insertItem(QPixmap(renam), tr("Rename Backtest Rule"), this, SLOT(renameTest()));

  testNoSelection();
}

TestPage::~TestPage ()
{
  delete menu;
}

void TestPage::openTest ()
{
  Tester *dialog = new Tester(config, nav->currentText());
  dialog->show();
}

void TestPage::newTest()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Backtest Rule"),
  					    		   tr("Enter new backtest rule name."),
  					    		   QLineEdit::Normal,
							   tr("New Rule"),
					    		   &ok,
					    		   this);

  if ((! ok) || (selection.isNull()))
    return;

  QString s = config->getData(Config::TestPath);
  s.append("/");
  s.append(selection);
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  Tester *dialog = new Tester(config, selection);
  dialog->show();
}

void TestPage::deleteTest()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  							   nav->getCurrentPath(),
							   "*");
  dialog->setCaption(tr("Select Portfolios To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  					  tr("Qtstalker: Warning"),
					  tr("Are you sure you want to delete this backtest rule?"),
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
    testNoSelection();
  }
  
  delete dialog;
}

void TestPage::renameTest ()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Backtest Rule"),
  					    		   tr("Enter new backtest rule name."),
  					    		   QLineEdit::Normal,
					    		   nav->currentText(),
					    		   &ok,
					    		   this);

  if ((! ok) || (selection.isNull()))
    return;

  QString s = config->getData(Config::TestPath);
  s.append("/");
  s.append(selection);
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  QString s2 = config->getData(Config::TestPath);
  s2.append("/");
  s2.append(nav->currentText());

  dir.rename(s2, s, TRUE);
  
  nav->updateList();
  testNoSelection();
}

void TestPage::testSelected (QString)
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


