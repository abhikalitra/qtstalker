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
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qlayout.h>

ScannerPage::ScannerPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  list = new QListBox(this);
  connect(list, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(scannerSelected(const QString &)));
  connect(list, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  vbox->addWidget(list);
  
  menu = new QPopupMenu();
  menu->insertItem(QPixmap(open), tr("Open Scanner"), this, SLOT(openScanner()));
  menu->insertItem(QPixmap(newchart), tr("New Scanner"), this, SLOT(newScanner()));
  menu->insertItem(QPixmap(deleteitem), tr("Delete Scanner"), this, SLOT(deleteScanner()));
  menu->insertItem(QPixmap(renam), tr("Rename Scanner"), this, SLOT(renameScanner()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help"), this, SLOT(slotHelp()), CTRL+Key_H);

  refreshList();
  scannerSelected(QString());
}

ScannerPage::~ScannerPage ()
{
  delete menu;
}

void ScannerPage::openScanner ()
{
  Scanner *dialog = new Scanner(list->currentText());
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
  dialog->show();
}

void ScannerPage::openScanner (QString d)
{
  Scanner *dialog = new Scanner(d);
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
  dialog->show();
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
				    list->currentText(),
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
    s2.append(list->currentText());

    dir.rename(s2, s, TRUE);

    refreshList();
    scannerSelected(QString());
  }
}

void ScannerPage::scannerSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(0), TRUE);
    menu->setItemEnabled(menu->idAt(2), TRUE);
    menu->setItemEnabled(menu->idAt(3), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(0), FALSE);
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
  list->clear();
  
  QDir dir(config.getData(Config::ScannerPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    list->insertItem(dir[loop], -1);
}

void ScannerPage::slotMessage (QString d)
{
  emit message(d);
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

