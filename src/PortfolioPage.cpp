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
#include "help.xpm"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qlayout.h>
#include <qfile.h>

PortfolioPage::PortfolioPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  list = new QListBox(this);
  connect(list, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(portfolioSelected(const QString &)));
  connect(list, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  vbox->addWidget(list);
  
  menu = new QPopupMenu();
  menu->insertItem(QPixmap(open), tr("&Open Portfolio"), this, SLOT(openPortfolio()), CTRL+Key_O);
  menu->insertItem(QPixmap(newchart), tr("&New Portfolio"), this, SLOT(newPortfolio()), CTRL+Key_N);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Portfolio"), this, SLOT(deletePortfolio()), CTRL+Key_D);
  menu->insertItem(QPixmap(renam), tr("&Rename Portfolio"), this, SLOT(renamePortfolio()), CTRL+Key_R);
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help"), this, SLOT(slotHelp()), CTRL+Key_H);

  updateList();
  portfolioSelected(QString());
}

PortfolioPage::~PortfolioPage ()
{
  delete menu;
}

void PortfolioPage::openPortfolio ()
{
  PortfolioDialog *dialog = new PortfolioDialog(list->currentText());
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
    s2.append(list->currentText());

    dir.rename(s2, s, TRUE);

    updateList();
    portfolioSelected(QString());
  }
}

void PortfolioPage::portfolioSelected (const QString &d)
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

void PortfolioPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void PortfolioPage::updateList ()
{
  list->clear();
  
  QDir dir(config.getData(Config::PortfolioPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    list->insertItem(dir[loop], -1);
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

