/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include "../pics/help.xpm"
#include "../pics/edit.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/rename.xpm"
#include "../pics/macro.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qdir.h>
#include <qaccel.h>

MacroPage::MacroPage (QWidget *w) : QListBox (w)
{
  keyFlag = FALSE;
  macroFlag = FALSE;
  tmacro = 0;
  
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
  menu->insertItem(QPixmap(macro), tr("R&un Macro		Ctrl+U"), this, SLOT(runMacro()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_N, NewMacro);
  a->insertItem(CTRL+Key_E, EditMacro);
  a->insertItem(CTRL+Key_D, DeleteMacro);
  a->insertItem(CTRL+Key_R, RenameMacro);
  a->insertItem(CTRL+Key_A, AssignMacro);
  a->insertItem(CTRL+Key_U, RunMacro);
  a->insertItem(CTRL+Key_H, Help);
  
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
  QString s(config.getData(Config::MacroPath));
  QString s2("*");
  SymbolDialog *dialog = new SymbolDialog(this,
  				          s,
					  s2,
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
  
  QString s("macro.html");
  dialog->setHelpFile(s);

  QString path = config.getData(Config::MacroPath);
  s = tr("Keys F1-F6");
  dialog->createPage(s);
  
  QString s2("CTRL+F1");
  QString s3 = config.getData(Config::Macro1);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F2";
  s3 = config.getData(Config::Macro2);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F3";
  s3 = config.getData(Config::Macro3);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F4";
  s3 = config.getData(Config::Macro4);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F5";
  s3 = config.getData(Config::Macro5);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F6";
  s3 = config.getData(Config::Macro6);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s = tr("Keys F7-F12");
  dialog->createPage(s);
  
  s2 = "CTRL+F7";
  s3 = config.getData(Config::Macro7);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F8";
  s3 = config.getData(Config::Macro8);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F9";
  s3 = config.getData(Config::Macro9);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F10";
  s3 = config.getData(Config::Macro10);
  dialog->addSymbolItem(s2, s, path, s3);
  
  s2 = "CTRL+F11";
  s3 = config.getData(Config::Macro11);
  dialog->addSymbolItem(s2, s, path, s3);
  s2 = "CTRL+F12";
  s3 = config.getData(Config::Macro12);
  dialog->addSymbolItem(s2, s, path, s3);

  int rc = dialog->exec();
    
  if (rc == QDialog::Accepted)
  {
    s = "CTRL+F1";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro1, s2);
    
    s = "CTRL+F2";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro2, s2);
    
    s = "CTRL+F3";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro3, s2);
    
    s = "CTRL+F4";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro4, s2);
    
    s = "CTRL+F5";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro5, s2);
    
    s = "CTRL+F6";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro6, s2);
    
    s = "CTRL+F7";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro7, s2);
    
    s = "CTRL+F8";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro8, s2);
    
    s = "CTRL+F9";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro9, s2);
    
    s = "CTRL+F10";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro10, s2);
    
    s = "CTRL+F11";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro11, s2);
    
    s = "CTRL+F12";
    s2 = dialog->getSymbol(s);
    config.setData(Config::Macro12, s2);
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
  QString s = "workwithmacros.html";
  HelpWindow *hw = new HelpWindow(this, s);
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
  key->accept();
  
  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        slotAccel(NewMacro);
	break;
      case Qt::Key_D:
        slotAccel(DeleteMacro);
	break;
      case Qt::Key_E:
        slotAccel(EditMacro);
	break;
      case Qt::Key_R:
        slotAccel(RenameMacro);
	break;
      case Qt::Key_A:
        slotAccel(AssignMacro);
	break;
      case Qt::Key_U:
        slotAccel(RunMacro);
	break;
      default:
        break;
    }
  }
  else
  {
    switch (key->key())
    {
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        break;      
      case Qt::Key_Enter:
      case Qt::Key_Return:
        runMacro();
        break;
      default:
        QListBox::keyPressEvent(key);
        break;
    }
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

void MacroPage::slotAccel (int id)
{
  switch (id)
  {
    case NewMacro:
      if (keyFlag)
        emit signalKeyPressed (Macro::MacroPage, ControlButton, Key_N, 0, QString());
      newMacro();
      break;  
    case EditMacro:
      if (keyFlag)
        emit signalKeyPressed (Macro::MacroPage, ControlButton, Key_E, 0, QString());
      editMacro();
      break;  
    case DeleteMacro:
      if (keyFlag)
        emit signalKeyPressed (Macro::MacroPage, ControlButton, Key_D, 0, QString());
      deleteMacro();
      break;  
    case RenameMacro:
      if (keyFlag)
        emit signalKeyPressed (Macro::MacroPage, ControlButton, Key_R, 0, QString());
      renameMacro();
      break;  
    case AssignMacro:
      if (keyFlag)
        emit signalKeyPressed (Macro::MacroPage, ControlButton, Key_A, 0, QString());
      assignMacro();
      break;  
    case RunMacro:
      runMacro();
      break;  
    case Help:
      slotHelp();
      break;  
    default:
      break;
  }
}

void MacroPage::runMacro (Macro *d)
{
  tmacro = d;
  macroFlag = TRUE;
  
  while (tmacro->getZone(tmacro->getIndex()) == Macro::MacroPage)
  {
    doKeyPress(tmacro->getKey(tmacro->getIndex()));
    
    tmacro->incIndex();
    if (tmacro->getIndex() >= tmacro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}



