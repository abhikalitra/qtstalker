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

#include "TestPage.h"
#include "Tester.h"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>

TestPage::TestPage (QWidget *w, Config *c) : BaseDialog (w)
{
  config = c;

  nav = new Navigator(this, config->getData(Config::TestPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(testSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(testNoSelection()));
  nav->updateList();
  basebox->addWidget(nav);

  setButton(QPixmap(open), tr("Open Backtest Rule"), 0);
  connect(getButton(0), SIGNAL(clicked()), this, SLOT(openTest()));
  setButtonStatus(0, FALSE);

  setButton(QPixmap(newchart), tr("New Backtest Rule"), 1);
  connect(getButton(1), SIGNAL(clicked()), this, SLOT(newTest()));

  setButton(QPixmap(deletefile), tr("Delete Backtest Rule"), 2);
  connect(getButton(2), SIGNAL(clicked()), this, SLOT(deleteTest()));
  setButtonStatus(2, FALSE);

  setButton(QPixmap(renam), tr("Rename Backtest Rule"), 3);
  connect(getButton(3), SIGNAL(clicked()), this, SLOT(renameTest()));
  setButtonStatus(3, FALSE);
}

TestPage::~TestPage ()
{
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

  QStringList l = config->getTest(selection);

  if (l.count())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  Tester *dialog = new Tester(config, selection);
  dialog->show();
}

void TestPage::deleteTest()
{
  int rc = QMessageBox::warning(this,
  					    tr("Qtstalker: Warning"),
					    tr("Are you sure you want to delete this backtest rule?"),
					    QMessageBox::Yes,
					    QMessageBox::No,
					    QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  config->deleteTest(nav->currentText());
  nav->updateList();
  testNoSelection();
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

  QStringList l = config->getTest(selection);
  if (l.count())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  l = config->getTest(nav->currentText());
  config->deleteTest(nav->currentText());
  config->setTest(selection, l);
  nav->updateList();
  testNoSelection();
}

void TestPage::testSelected (QString)
{
  setButtonStatus(0, TRUE);
  setButtonStatus(2, TRUE);
  setButtonStatus(3, TRUE);
}

void TestPage::testNoSelection ()
{
  setButtonStatus(0, FALSE);
  setButtonStatus(2, FALSE);
  setButtonStatus(3, FALSE);
}

