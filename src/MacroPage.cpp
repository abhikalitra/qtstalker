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
#include "PrefDialog.h"
#include "Macro.h"
#include "help.xpm"
#include "edit.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qdir.h>
#include <qaccel.h>

MacroPage::MacroPage (QWidget *w) : QListBox (w)
{
  keyFlag = FALSE;
  
  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(macroSelected(const QString &)));
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  
  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Macro		Ctrl+N"), this, SLOT(newMacro()));
  menu->insertItem(QPixmap(edit), tr("&Edit Macro		Ctrl+E"), this, SLOT(editMacro()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Macro	Ctrl+D"), this, SLOT(deleteMacro()));
  menu->insertItem(QPixmap(renam), tr("&Rename Macro		Ctrl+R"), this, SLOT(renameMacro()));
  menu->insertItem(tr("&Assign Macro		Ctrl+A"), this, SLOT(assignMacro()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(renam), tr("R&un Macro		Ctrl+U"), this, SLOT(runMacro()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  a->insertItem(CTRL+Key_N, 0);
  a->connectItem(0, this, SLOT(newMacro()));
  a->insertItem(CTRL+Key_E, 1);
  a->connectItem(1, this, SLOT(editMacro()));
  a->insertItem(CTRL+Key_D, 2);
  a->connectItem(2, this, SLOT(deleteMacro()));
  a->insertItem(CTRL+Key_R, 3);
  a->connectItem(3, this, SLOT(renameMacro()));
  a->insertItem(CTRL+Key_A, 4);
  a->connectItem(4, this, SLOT(assignMacro()));
  a->insertItem(CTRL+Key_U, 5);
  a->connectItem(5, this, SLOT(runMacro()));
  a->insertItem(CTRL+Key_H, 6);
  a->connectItem(6, this, SLOT(slotHelp()));
  
  updateList();
  macroSelected(QString());
}

MacroPage::~MacroPage ()
{
}

void MacroPage::editMacro ()
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

    emit signalRecordMacro(s);
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

void MacroPage::assignMacro ()
{
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Assign Macro"));
  dialog->setHelpFile("macro.html");

  QString s = config.getData(Config::MacroPath);
  
  dialog->createPage(tr("Keys F1-F6"));
  dialog->addSymbolItem("CTRL+F1", tr("Keys F1-F6"), s, config.getData(Config::MacroF1));
  dialog->addSymbolItem("CTRL+F2", tr("Keys F1-F6"), s, config.getData(Config::MacroF2));
  dialog->addSymbolItem("CTRL+F3", tr("Keys F1-F6"), s, config.getData(Config::MacroF3));
  dialog->addSymbolItem("CTRL+F4", tr("Keys F1-F6"), s, config.getData(Config::MacroF4));
  dialog->addSymbolItem("CTRL+F5", tr("Keys F1-F6"), s, config.getData(Config::MacroF5));
  dialog->addSymbolItem("CTRL+F6", tr("Keys F1-F6"), s, config.getData(Config::MacroF6));
  
  dialog->createPage(tr("Keys F7-F12"));
  dialog->addSymbolItem("CTRL+F7", tr("Keys F7-F12"), s, config.getData(Config::MacroF7));
  dialog->addSymbolItem("CTRL+F8", tr("Keys F7-F12"), s, config.getData(Config::MacroF8));
  dialog->addSymbolItem("CTRL+F9", tr("Keys F7-F12"), s, config.getData(Config::MacroF9));
  dialog->addSymbolItem("CTRL+F10", tr("Keys F7-F12"), s, config.getData(Config::MacroF10));
  dialog->addSymbolItem("CTRL+F11", tr("Keys F7-F12"), s, config.getData(Config::MacroF11));
  dialog->addSymbolItem("CTRL+F12", tr("Keys F7-F12"), s, config.getData(Config::MacroF12));

  int rc = dialog->exec();
    
  if (rc == QDialog::Accepted)
  {
    config.setData(Config::MacroF1, dialog->getSymbol("CTRL+F1"));
    config.setData(Config::MacroF2, dialog->getSymbol("CTRL+F2"));
    config.setData(Config::MacroF3, dialog->getSymbol("CTRL+F3"));
    config.setData(Config::MacroF4, dialog->getSymbol("CTRL+F4"));
    config.setData(Config::MacroF5, dialog->getSymbol("CTRL+F5"));
    config.setData(Config::MacroF6, dialog->getSymbol("CTRL+F6"));
    config.setData(Config::MacroF7, dialog->getSymbol("CTRL+F7"));
    config.setData(Config::MacroF8, dialog->getSymbol("CTRL+F8"));
    config.setData(Config::MacroF9, dialog->getSymbol("CTRL+F9"));
    config.setData(Config::MacroF10, dialog->getSymbol("CTRL+F10"));
    config.setData(Config::MacroF11, dialog->getSymbol("CTRL+F11"));
    config.setData(Config::MacroF12, dialog->getSymbol("CTRL+F12"));
  }
  
  delete dialog;  
}

void MacroPage::macroSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(1), TRUE);
    menu->setItemEnabled(menu->idAt(2), TRUE);
    menu->setItemEnabled(menu->idAt(3), TRUE);
    menu->setItemEnabled(menu->idAt(6), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
    menu->setItemEnabled(menu->idAt(3), FALSE);
    menu->setItemEnabled(menu->idAt(6), FALSE);
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
    
  QString s = config.getData(Config::MacroPath) + "/" + item->text();
  emit signalRunMacro(s);
}

void MacroPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithmacros.html");
  hw->show();
}

void MacroPage::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (Macro::MacroPage, key->state(), key->key(), key->ascii(), key->text());
    
  doKeyPress(key);  
}

void MacroPage::doKeyPress (QKeyEvent *key)
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
      runMacro();
      break;
    default:
      key->accept();
      QListBox::keyPressEvent(key);
      break;
  }
}

void MacroPage::runMacro ()
{
  QString s = config.getData(Config::MacroPath) + "/" + currentText();
  emit signalRunMacro(s);
}

void MacroPage::setKeyFlag (bool d)
{
  keyFlag = d;
}


