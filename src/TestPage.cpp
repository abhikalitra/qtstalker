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
#include "HelpWindow.h"
#include "Macro.h"
#include "help.xpm"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include "copy.xpm"
#include "SymbolDialog.h"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qdir.h>
#include <stdlib.h>
#include <qaccel.h>

TestPage::TestPage (QWidget *w) : QListBox (w)
{
  keyFlag = FALSE;

  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(testSelected(const QString &)));
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  
  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Rule		Ctrl+N"), this, SLOT(newTest()));
  menu->insertItem(QPixmap(open), tr("&Open Rule		Ctrl+O"), this, SLOT(openTest()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Rule	Ctrl+D"), this, SLOT(deleteTest()));
  menu->insertItem(QPixmap(renam), tr("&Rename Rule		Ctrl+R"), this, SLOT(renameTest()));
  menu->insertItem(QPixmap(copy), tr("&Copy Rule		Ctrl+Y"), this, SLOT(copyTest()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  a->insertItem(CTRL+Key_N, 0);
  a->connectItem(0, this, SLOT(newTest()));
  a->insertItem(CTRL+Key_O, 1);
  a->connectItem(1, this, SLOT(openTest()));
  a->insertItem(CTRL+Key_D, 2);
  a->connectItem(2, this, SLOT(deleteTest()));
  a->insertItem(CTRL+Key_R, 3);
  a->connectItem(3, this, SLOT(renameTest()));
  a->insertItem(CTRL+Key_Y, 4);
  a->connectItem(4, this, SLOT(copyTest()));
  a->insertItem(CTRL+Key_H, 5);
  a->connectItem(5, this, SLOT(slotHelp()));
  
  updateList();
  testNoSelection();
}

TestPage::~TestPage ()
{
}

void TestPage::openTest ()
{
  Tester *dialog = new Tester(currentText());
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
  if (currentItem() == -1)
    return;

  bool ok;
  QString s = QInputDialog::getText(tr("Rename Backtest Rule"),
  				    tr("Enter new backtest rule name."),
  				    QLineEdit::Normal,
				    currentText(),
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
    
  s = config.getData(Config::TestPath);
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
  s2.append(currentText());
  dir.rename(s2, s, TRUE);
  
  changeItem(selection, currentItem());
}

void TestPage::copyTest ()
{
  if (currentItem() == -1)
    return;

  bool ok;
  QString s = QInputDialog::getText(tr("Copy Backtest Rule"),
  				    tr("Enter new name of copy."),
  				    QLineEdit::Normal,
				    currentText(),
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
    
  s = config.getData(Config::TestPath) + "/" + selection;
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  s = "cp -R " + config.getData(Config::TestPath) + "/" + currentText() + " ";
  s.append(config.getData(Config::TestPath) + "/" + selection);

  if (system(s.latin1()) == -1)
    qDebug("TestPage::copyTest:command failed");
    
  updateList();
  testNoSelection();
}

void TestPage::testSelected (const QString &) 
{
  menu->setItemEnabled(menu->idAt(1), TRUE);
  menu->setItemEnabled(menu->idAt(2), TRUE);
  menu->setItemEnabled(menu->idAt(3), TRUE);
  menu->setItemEnabled(menu->idAt(4), TRUE);
}

void TestPage::testNoSelection ()
{
  menu->setItemEnabled(menu->idAt(1), FALSE);
  menu->setItemEnabled(menu->idAt(2), FALSE);
  menu->setItemEnabled(menu->idAt(3), FALSE);
  menu->setItemEnabled(menu->idAt(4), FALSE);
}

void TestPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void TestPage::updateList ()
{
  clear();
  
  QDir dir(config.getData(Config::TestPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    insertItem(dir[loop], -1);
}

void TestPage::slotMessage (QString d)
{
  emit message(d);
}

void TestPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  Tester *dialog = new Tester(item->text());
  dialog->show();
}

void TestPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithbacktest.html");
  hw->show();
}

void TestPage::setKeyFlag (bool d)
{
  keyFlag = d;
}

void TestPage::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (Macro::TestPage, key->state(), key->key(), key->ascii(), key->text());
  
  doKeyPress(key);
}

void TestPage::doKeyPress (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_Delete:
      key->accept();
      deleteTest();
      break;
    case Qt::Key_Left: // segfaults if we dont trap this
    case Qt::Key_Right: // segfaults if we dont trap this
      key->accept();
      break;      
    case Qt::Key_Enter:
    case Qt::Key_Return:
      key->accept();
      openTest();
      break;
    default:
      key->accept();
      QListBox::keyPressEvent(key);
      break;
  }
}


