/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>

PortfolioPage::PortfolioPage (QWidget *w, Config *c) : BaseDialog (w)
{
  config = c;

  nav = new Navigator(this, config->getData(Config::PortfolioPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(portfolioSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(portfolioNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this, SLOT(rightClick(QListBoxItem *)));
  nav->updateList();
  basebox->addWidget(nav);
  
  menu = new QPopupMenu();
  menu->insertItem(QPixmap(open), tr("Open Portfolio"), this, SLOT(openPortfolio()));
  menu->insertItem(QPixmap(newchart), tr("New Portfolio"), this, SLOT(newPortfolio()));
  menu->insertItem(QPixmap(deletefile), tr("Delete Portfolio"), this, SLOT(deletePortfolio()));
  menu->insertItem(QPixmap(renam), tr("Rename Portfolio"), this, SLOT(renamePortfolio()));

  portfolioNoSelection();
}

PortfolioPage::~PortfolioPage ()
{
  delete menu;
}

void PortfolioPage::openPortfolio ()
{
  PortfolioDialog *dialog = new PortfolioDialog(config, nav->currentText());
  dialog->show();
}

void PortfolioPage::newPortfolio()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Portfolio"),
  							   tr("Enter new portfolio name."),
							   QLineEdit::Normal,
							   tr("New Portfolio"),
							   &ok,
							   this);
  if ((ok) && (! selection.isNull()))
  {
    QString s = config->getData(Config::PortfolioPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    PortfolioDialog *dialog = new PortfolioDialog(config, selection);
    dialog->show();
  }
}

void PortfolioPage::deletePortfolio()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  							   nav->getCurrentPath(),
							   "*");
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

    nav->updateList();
    portfolioNoSelection();
  }

  delete dialog;
}

void PortfolioPage::renamePortfolio ()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Portfolio"),
  							   tr("Enter new portfolio name."),
							   QLineEdit::Normal,
							   nav->currentText(),
							   &ok,
							   this);
  if ((ok) && (! selection.isNull()))
  {
    QString s = config->getData(Config::PortfolioPath);
    s.append("/");
    s.append(selection);
    QDir dir(s);
    if (dir.exists(s, TRUE))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    QString s2 = config->getData(Config::PortfolioPath);
    s2.append("/");
    s2.append(nav->currentText());

    dir.rename(s2, s, TRUE);

    nav->updateList();
    portfolioNoSelection();
  }
}

void PortfolioPage::portfolioSelected (QString)
{
  menu->setItemEnabled(menu->idAt(0), TRUE);
  menu->setItemEnabled(menu->idAt(2), TRUE);
  menu->setItemEnabled(menu->idAt(3), TRUE);
}

void PortfolioPage::portfolioNoSelection ()
{
  menu->setItemEnabled(menu->idAt(0), FALSE);
  menu->setItemEnabled(menu->idAt(2), FALSE);
  menu->setItemEnabled(menu->idAt(3), FALSE);
}

void PortfolioPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}
