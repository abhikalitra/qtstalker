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

#include "MacroPage.h"
#include "SymbolDialog.h"
//#include "MacroDialog.h"
#include "HelpWindow.h"
#include "help.xpm"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qfile.h>
#include <qdir.h>

MacroPage::MacroPage (QWidget *w) : QListBox (w)
{
  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(macroSelected(const QString &)));
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  
  menu = new QPopupMenu();
  menu->insertItem(QPixmap(newchart), tr("&New Macro"), this, SLOT(newMacro()), CTRL+Key_N);
  menu->insertItem(QPixmap(open), tr("&Open Macro"), this, SLOT(openMacro()), CTRL+Key_O);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Macro"), this, SLOT(deleteMacro()), CTRL+Key_D);
  menu->insertItem(QPixmap(renam), tr("&Rename Macro"), this, SLOT(renameMacro()), CTRL+Key_R);
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(renam), tr("R&un Macro"), this, SLOT(runMacro()), CTRL+Key_U);
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help"), this, SLOT(slotHelp()), CTRL+Key_H);

  updateList();
  macroSelected(QString());
}

MacroPage::~MacroPage ()
{
  delete menu;
}

void MacroPage::openMacro ()
{
//  MacroDialog *dialog = new MacroDialog(currentText());
//  dialog->show();
}

void MacroPage::openMacro (QString d)
{
//  MacroDialog *dialog = new MacroDialog(d);
//  dialog->show();
}

void MacroPage::newMacro()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Macro"),
  				    tr("Enter new macro name."),
				    QLineEdit::Normal,
				    tr("NewMacro"),
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
  
    s = config.getData(Config::MacroPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This macro already exists."));
      return;
    }

    // create the empty file    
    QFile f(s);
    if (! f.open(IO_WriteOnly))
      return;
    f.close();
    
    updateList();

    openMacro(selection);
  }
}

void MacroPage::deleteMacro()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  				          config.getData(Config::MacroPath),
					  "*",
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Macros To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete this macro?"),
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

    updateList();
    macroSelected(QString());
  }

  delete dialog;
}

void MacroPage::renameMacro ()
{
  bool ok;
  QString s = QInputDialog::getText(tr("Rename Macro"),
  				    tr("Enter new macro name."),
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
  
    s = config.getData(Config::MacroPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This macro already exists."));
      return;
    }

    QString s2 = config.getData(Config::MacroPath);
    s2.append("/");
    s2.append(currentText());

    dir.rename(s2, s, TRUE);

    updateList();
    macroSelected(QString());
  }
}

void MacroPage::macroSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(1), TRUE);
    menu->setItemEnabled(menu->idAt(2), TRUE);
    menu->setItemEnabled(menu->idAt(3), TRUE);
    menu->setItemEnabled(menu->idAt(5), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
    menu->setItemEnabled(menu->idAt(3), FALSE);
    menu->setItemEnabled(menu->idAt(5), FALSE);
  }
}

void MacroPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void MacroPage::updateList ()
{
  clear();
  
  QDir dir(config.getData(Config::MacroPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    insertItem(dir[loop], -1);
}

void MacroPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  openMacro(item->text());
}

void MacroPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithmacros.html");
  hw->show();
}

void MacroPage::keyPressEvent (QKeyEvent *key)
{
  doKeyPress(key);
}

void MacroPage::doKeyPress (QKeyEvent *key)
{
  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_N:
        key->accept();
        newMacro();
        break;
      case Qt::Key_O:
        key->accept();
        openMacro();
        break;
      case Qt::Key_R:
        key->accept();
        renameMacro();
        break;
      case Qt::Key_H:
        key->accept();
        slotHelp();
        break;
      case Qt::Key_U:
        key->accept();
        runMacro();
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
        key->accept();
        deleteMacro();
        break;
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        key->accept();
        break;      
      case Qt::Key_Enter:
      case Qt::Key_Return:
        key->accept();
        openMacro();
        break;
      default:
        key->ignore();
        QListBox::keyPressEvent(key);
        break;
    }
  }
}

void MacroPage::runMacro ()
{
  emit signalRunMacro(currentText());
}

