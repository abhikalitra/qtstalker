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

#include "ScannerPage.h"
#include "Scanner.h"
#include "SymbolDialog.h"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qlayout.h>

ScannerPage::ScannerPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  nav = new Navigator(this, config.getData(Config::ScannerPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(scannerSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(scannerNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  nav->updateList();
  vbox->addWidget(nav);

  menu = new QPopupMenu();
  menu->insertItem(QPixmap(open), tr("Open Scanner"), this, SLOT(openScanner()));
  menu->insertItem(QPixmap(newchart), tr("New Scanner"), this, SLOT(newScanner()));
  menu->insertItem(QPixmap(deleteitem), tr("Delete Scanner"), this, SLOT(deleteScanner()));
  menu->insertItem(QPixmap(renam), tr("Rename Scanner"), this, SLOT(renameScanner()));

  scannerNoSelection();
}

ScannerPage::~ScannerPage ()
{
  delete menu;
}

void ScannerPage::openScanner ()
{
  Scanner *dialog = new Scanner(nav->currentText());
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
  dialog->show();
}

void ScannerPage::newScanner()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Scanner"),
  					    tr("Enter new scanner name."),
					    QLineEdit::Normal,
					    tr("New Scanner"),
					    &ok,
					    this);
  if ((ok) && (! selection.isNull()))
  {
    QString s = config.getData(Config::ScannerPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This scanner already exists."));
      return;
    }

    Scanner *dialog = new Scanner(selection);
    connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
    dialog->show();
  }
}

void ScannerPage::deleteScanner()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  nav->getCurrentPath(),
					  "*");
  dialog->setCaption(tr("Select Scanners To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete this scanner?"),
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

    scannerNoSelection();
  }

  delete dialog;
}

void ScannerPage::renameScanner ()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Scanner"),
  					    tr("Enter new scanner name."),
					    QLineEdit::Normal,
					    nav->currentText(),
					    &ok,
					    this);
  if ((ok) && (! selection.isNull()))
  {
    QString s = config.getData(Config::ScannerPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This scanner already exists."));
      return;
    }

    QString s2 = config.getData(Config::ScannerPath);
    s2.append("/");
    s2.append(nav->currentText());

    dir.rename(s2, s, TRUE);

    nav->updateList();

    scannerNoSelection();
  }
}

void ScannerPage::scannerSelected (QString)
{
  menu->setItemEnabled(menu->idAt(0), TRUE);
  menu->setItemEnabled(menu->idAt(2), TRUE);
  menu->setItemEnabled(menu->idAt(3), TRUE);
}

void ScannerPage::scannerNoSelection ()
{
  menu->setItemEnabled(menu->idAt(0), FALSE);
  menu->setItemEnabled(menu->idAt(2), FALSE);
  menu->setItemEnabled(menu->idAt(3), FALSE);
}

void ScannerPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void ScannerPage::refreshList ()
{
  nav->updateList();
}

void ScannerPage::slotMessage (QString d)
{
  emit message(d);
}

