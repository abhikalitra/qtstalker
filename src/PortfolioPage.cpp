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

#include "PortfolioPage.h"
#include "PortfolioDialog.h"
#include "SymbolDialog.h"
#include "HelpWindow.h"
#include "Macro.h"
#include "help.xpm"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qfile.h>
#include <qaccel.h>

PortfolioPage::PortfolioPage (QWidget *w) : QListBox (w)
{
  keyFlag = FALSE;
  
  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(portfolioSelected(const QString &)));
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  
  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Portfolio	Ctrl+N"), this, SLOT(newPortfolio()));
  menu->insertItem(QPixmap(open), tr("&Open Portfolio		Ctrl+O"), this, SLOT(openPortfolio()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Portfolio	Ctrl+D"), this, SLOT(deletePortfolio()));
  menu->insertItem(QPixmap(renam), tr("&Rename Portfolio	Ctrl+R"), this, SLOT(renamePortfolio()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  a->insertItem(CTRL+Key_N, 0);
  a->connectItem(0, this, SLOT(newPortfolio()));
  a->insertItem(CTRL+Key_O, 1);
  a->connectItem(1, this, SLOT(openPortfolio()));
  a->insertItem(CTRL+Key_D, 2);
  a->connectItem(2, this, SLOT(deletePortfolio()));
  a->insertItem(CTRL+Key_R, 3);
  a->connectItem(3, this, SLOT(renamePortfolio()));
  a->insertItem(CTRL+Key_H, 4);
  a->connectItem(4, this, SLOT(slotHelp()));
  
  updateList();
  portfolioSelected(QString());
}

PortfolioPage::~PortfolioPage ()
{
}

void PortfolioPage::openPortfolio ()
{
  PortfolioDialog *dialog = new PortfolioDialog(currentText());
  dialog->show();
}

void PortfolioPage::openPortfolio (QString d)
{
  PortfolioDialog *dialog = new PortfolioDialog(d);
  dialog->show();
}

void PortfolioPage::newPortfolio()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Portfolio"),
  				    tr("Enter new portfolio name."),
				    QLineEdit::Normal,
				    tr("New Portfolio"),
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
  
    s = config.getData(Config::PortfolioPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    // create the empty file    
    QFile f(s);
    if (! f.open(IO_WriteOnly))
      return;
    f.close();
    
    updateList();

    openPortfolio(selection);
  }
}

void PortfolioPage::deletePortfolio()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  				          config.getData(Config::PortfolioPath),
					  "*",
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Portfolios To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete this portfolio?"),
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
    portfolioSelected(QString());
  }

  delete dialog;
}

void PortfolioPage::renamePortfolio ()
{
  bool ok;
  QString s = QInputDialog::getText(tr("Rename Portfolio"),
  				    tr("Enter new portfolio name."),
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
  
    s = config.getData(Config::PortfolioPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    QString s2 = config.getData(Config::PortfolioPath);
    s2.append("/");
    s2.append(currentText());

    dir.rename(s2, s, TRUE);

    updateList();
    portfolioSelected(QString());
  }
}

void PortfolioPage::portfolioSelected (const QString &d)
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

void PortfolioPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void PortfolioPage::updateList ()
{
  clear();
  
  QDir dir(config.getData(Config::PortfolioPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    insertItem(dir[loop], -1);
}

void PortfolioPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  openPortfolio(item->text());
}

void PortfolioPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithportfolios.html");
  hw->show();
}

void PortfolioPage::setKeyFlag (bool d)
{
  keyFlag = d;
}

void PortfolioPage::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (Macro::PortfolioPage, key->state(), key->key(), key->ascii(), key->text());
    
  doKeyPress(key);  
}

void PortfolioPage::doKeyPress (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_Delete:
      key->accept();
      deletePortfolio();
      break;
    case Qt::Key_Left: // segfaults if we dont trap this
    case Qt::Key_Right: // segfaults if we dont trap this
      key->accept();
      break;      
    case Qt::Key_Enter:
    case Qt::Key_Return:
      key->accept();
      openPortfolio();
      break;
    default:
      key->accept();
      QListBox::keyPressEvent(key);
      break;
  }
}

