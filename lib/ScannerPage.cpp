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

#include "ScannerPage.h"
#include "Scanner.h"
#include "DataBase.h"
#include "../pics/open.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/macro.xpm"
#include <QInputDialog>
#include <QMessageBox>
#include <QCursor>
#include <QLayout>
#include <QVBoxLayout>

ScannerPage::ScannerPage (QWidget *w) : QWidget (w)
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
  QAction *action = menu->addAction(QIcon(newchart), tr("&New Scanner		Ctrl+N"), this, SLOT(newScanner()), QKeySequence(Qt::CTRL+Qt::Key_N));
  actionList.append(action);
  action = menu->addAction(QIcon(openp), tr("&Open Scanner		Ctrl+O"), this, SLOT(openScanner()), QKeySequence(Qt::CTRL+Qt::Key_O));
  actionList.append(action);
  action = menu->addAction(QIcon(deleteitem), tr("&Delete Scanner	Ctrl+D"), this, SLOT(deleteScanner()), QKeySequence(Qt::CTRL+Qt::Key_D));
  actionList.append(action);
  action = menu->addAction(QIcon(macro), tr("R&un Scanner		Ctrl+U"), this, SLOT(runScanner()), QKeySequence(Qt::CTRL+Qt::Key_U));
  actionList.append(action);

  refreshList();

  scannerSelected();
}

ScannerPage::~ScannerPage ()
{
}

void ScannerPage::openScanner ()
{
  Scanner *dialog = new Scanner(list->currentItem()->text());
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
  connect(dialog, SIGNAL(scanComplete()), this, SIGNAL(refreshGroup()));
  dialog->show();
}

void ScannerPage::openScanner (QString d)
{
  Scanner *dialog = new Scanner(d);
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
  connect(dialog, SIGNAL(scanComplete()), this, SIGNAL(refreshGroup()));
  dialog->show();
}

void ScannerPage::runScanner ()
{
  DataBase db;
  QStringList l;
  bool ok;
  db.getScannerList(l);
  QString item = QInputDialog::getItem(this, QString(tr("Run Scanner")), QString(tr("Select scanner to run")), l, 0, FALSE, &ok, 0);
  if (! item.length())
    return;

  Scanner *sdialog = new Scanner(item);
  connect(sdialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(sdialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
  sdialog->show();
  sdialog->scan();
  delete sdialog;
}

void ScannerPage::newScanner()
{
  bool ok;
  QString s = QInputDialog::getText(this,
				    tr("New Scanner"),
  				    tr("Enter new scanner name."),
				    QLineEdit::Normal,
				    tr("NewScanner"),
				    &ok);
  if (! s.length())
    return;

  DataBase db;
  QStringList l;
  db.getScannerList(l);
  if (l.contains(s, Qt::CaseSensitive))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This scanner already exists."));
    return;
  }

  openScanner(s);
}

void ScannerPage::deleteScanner()
{
  DataBase db;
  QStringList l;
  bool ok;
  db.getScannerList(l);
  QString item = QInputDialog::getItem(this, QString(tr("Delete Scanner")), QString(tr("Select scanner to delete")), l, 0, FALSE, &ok, 0);
  if (! item.length())
    return;

  int rc = QMessageBox::warning(this,
		      	        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to delete this scanner?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  db.deleteScanner(item);

  refreshList();
  scannerSelected();
}

void ScannerPage::scannerSelected ()
{
  bool b = FALSE;
  int loop;
  if (list->currentItem())
    b = TRUE;

  // 1,2,3
  for (loop = 1; loop < 4; loop++)
    actionList.at(loop)->setEnabled(b);
}

void ScannerPage::rightClick (const QPoint &)
{
  menu->exec(QCursor::pos());
}

void ScannerPage::refreshList ()
{
  list->clear();

  DataBase db;
  QStringList l;
  db.getScannerList(l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
    list->addItem(l[loop]);
}

void ScannerPage::doubleClick (QListWidgetItem *item)
{
  if (! item)
    return;
    
  openScanner(item->text());
}

void ScannerPage::keyPressEvent (QKeyEvent *key)
{
  doKeyPress(key);  
}

void ScannerPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->modifiers() == Qt::ControlModifier)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        newScanner();
	break;
      case Qt::Key_D:
        deleteScanner();
	break;
      case Qt::Key_O:
        openScanner();
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
        deleteScanner();
        break;
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        break;      
      case Qt::Key_Enter:
      case Qt::Key_Return:
        openScanner();
        break;
      default:
//        list->keyPressEvent(key);
        break;
    }
  }
}


