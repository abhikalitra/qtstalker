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
#include "HelpWindow.h"
#include "help.xpm"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include "macro.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qaccel.h>
#include <qfileinfo.h>

ScannerPage::ScannerPage (QWidget *w) : QListBox (w)
{
  keyFlag = FALSE;
  macroFlag = FALSE;
  tmacro = 0;

  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(scannerSelected(const QString &)));
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  
  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Scanner		Ctrl+N"), this, SLOT(newScanner()));
  menu->insertItem(QPixmap(open), tr("&Open Scanner		Ctrl+O"), this, SLOT(openScanner()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Scanner	Ctrl+D"), this, SLOT(deleteScanner()));
  menu->insertItem(QPixmap(renam), tr("&Rename Scanner		Ctrl+R"), this, SLOT(renameScanner()));
  menu->insertItem(QPixmap(macro), tr("R&un Scanner		Ctrl+U"), this, SLOT(runScanner()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_N, NewScanner);
  a->insertItem(CTRL+Key_O, OpenScanner);
  a->insertItem(CTRL+Key_D, DeleteScanner);
  a->insertItem(CTRL+Key_R, RenameScanner);
  a->insertItem(CTRL+Key_U, RunScanner);
  a->insertItem(CTRL+Key_H, Help);
  
  refreshList();
  scannerSelected(QString());
}

ScannerPage::~ScannerPage ()
{
}

void ScannerPage::openScanner ()
{
  Scanner *dialog = new Scanner(currentText());
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
  dialog->show();
}

void ScannerPage::openScanner (QString d)
{
  Scanner *dialog = new Scanner(d);
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
  dialog->show();
}

void ScannerPage::runScanner ()
{
  SymbolDialog *dialog = new SymbolDialog(this, 
  					  config.getData(Config::ScannerPath),
					  "*",
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select scanners to run"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QStringList l = dialog->selectedFiles();
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      QFileInfo fi(l[loop]);
      Scanner *sdialog = new Scanner(fi.fileName());
      connect(sdialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
      connect(sdialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
      sdialog->show();
      sdialog->scan();
      delete sdialog;
    }
  }

  delete dialog;
}

void ScannerPage::newScanner()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Scanner"),
  				    tr("Enter new scanner name."),
				    QLineEdit::Normal,
				    tr("NewScanner"),
				    &ok,
				    this);
  if ((ok) && (! s.isNull()))
  {
    int loop;
    QString selection;
    for (loop = 0; loop < (int) s.length(); loop++)
    {
      QChar c = s.at(loop);
      if (c.isLetterOrNumber())
        selection.append(c);
    }
  
    s = config.getData(Config::ScannerPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This scanner already exists."));
      return;
    }

    openScanner(selection);
  }
}

void ScannerPage::deleteScanner()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  config.getData(Config::ScannerPath),
					  "*",
					  QFileDialog::ExistingFiles);
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

    refreshList();
    scannerSelected(QString());
  }

  delete dialog;
}

void ScannerPage::renameScanner ()
{
  bool ok;
  QString s = QInputDialog::getText(tr("Rename Scanner"),
  				    tr("Enter new scanner name."),
				    QLineEdit::Normal,
				    currentText(),
				    &ok,
				    this);
  if ((ok) && (! s.isNull()))
  {
    int loop;
    QString selection;
    for (loop = 0; loop < (int) s.length(); loop++)
    {
      QChar c = s.at(loop);
      if (c.isLetterOrNumber())
        selection.append(c);
    }
  
    s = config.getData(Config::ScannerPath);
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
    s2.append(currentText());

    dir.rename(s2, s, TRUE);

    refreshList();
    scannerSelected(QString());
  }
}

void ScannerPage::scannerSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(1), TRUE);
    menu->setItemEnabled(menu->idAt(2), TRUE);
    menu->setItemEnabled(menu->idAt(3), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
    menu->setItemEnabled(menu->idAt(3), FALSE);
  }
}

void ScannerPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void ScannerPage::refreshList ()
{
  clear();
  
  QDir dir(config.getData(Config::ScannerPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    insertItem(dir[loop], -1);
}

void ScannerPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  openScanner(item->text());
}

void ScannerPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithscanner.html");
  hw->show();
}

void ScannerPage::setKeyFlag (bool d)
{
  keyFlag = d;
}

void ScannerPage::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (Macro::ScannerPage, key->state(), key->key(), key->ascii(), key->text());
  
  doKeyPress(key);  
}

void ScannerPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        slotAccel(NewScanner);
	break;
      case Qt::Key_D:
        slotAccel(DeleteScanner);
	break;
      case Qt::Key_O:
        slotAccel(OpenScanner);
	break;
      case Qt::Key_R:
        slotAccel(RenameScanner);
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
        QListBox::keyPressEvent(key);
        break;
    }
  }
}

void ScannerPage::slotAccel (int id)
{
  switch (id)
  {
    case NewScanner:
      if (keyFlag)
        emit signalKeyPressed (Macro::ScannerPage, ControlButton, Key_N, 0, QString());
      newScanner();
      break;  
    case DeleteScanner:
      if (keyFlag)
        emit signalKeyPressed (Macro::ScannerPage, ControlButton, Key_D, 0, QString());
      deleteScanner();
      break;  
    case RenameScanner:
      if (keyFlag)
        emit signalKeyPressed (Macro::ScannerPage, ControlButton, Key_R, 0, QString());
      renameScanner();
      break;  
    case OpenScanner:
      if (keyFlag)
        emit signalKeyPressed (Macro::ScannerPage, ControlButton, Key_O, 0, QString());
      openScanner();
      break;  
    case RunScanner:
      if (keyFlag)
        emit signalKeyPressed (Macro::ScannerPage, ControlButton, Key_U, 0, QString());
      runScanner();
      break;  
    case Help:
      slotHelp();
      break;  
    default:
      break;
  }
}

void ScannerPage::runMacro (Macro *d)
{
  tmacro = d;
  macroFlag = TRUE;
  
  while (tmacro->getZone(tmacro->getIndex()) == Macro::ScannerPage)
  {
    doKeyPress(tmacro->getKey(tmacro->getIndex()));
    
    tmacro->incIndex();
    if (tmacro->getIndex() >= tmacro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}

