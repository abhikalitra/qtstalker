/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>

PortfolioPage::PortfolioPage (QWidget *w, Config *c) : BaseDialog (w)
{
  config = c;

  nav = new Navigator(this, config->getData(Config::PortfolioPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(portfolioSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(portfolioNoSelection()));
  nav->updateList();
  basebox->addWidget(nav);

  setButton(QPixmap(open), tr("Open Portfolio"), 0);
  connect(getButton(0), SIGNAL(clicked()), this, SLOT(openPortfolio()));
  setButtonStatus(0, FALSE);

  setButton(QPixmap(newchart), tr("New Portfolio"), 1);
  connect(getButton(1), SIGNAL(clicked()), this, SLOT(newPortfolio()));

  setButton(QPixmap(deletefile), tr("Delete Portfolio"), 2);
  connect(getButton(2), SIGNAL(clicked()), this, SLOT(deletePortfolio()));
  setButtonStatus(2, FALSE);

  setButton(QPixmap(renam), tr("Rename Portfolio"), 3);
  connect(getButton(3), SIGNAL(clicked()), this, SLOT(renamePortfolio()));
  setButtonStatus(3, FALSE);
}

PortfolioPage::~PortfolioPage ()
{
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
    QStringList l = config->getPortfolio(selection);

    if (l.count())
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
  config->deletePortfolio(nav->currentText());
  nav->updateList();
  portfolioNoSelection();
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
    QStringList l = config->getPortfolio(selection);
    if (l.count())
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    l = config->getPortfolio(nav->currentText());
    config->deletePortfolio(nav->currentText());
    config->setPortfolio(selection, l);

    nav->updateList();
    portfolioNoSelection();
  }
}

void PortfolioPage::portfolioSelected (QString)
{
  setButtonStatus(0, TRUE);
  setButtonStatus(2, TRUE);
  setButtonStatus(3, TRUE);
}

void PortfolioPage::portfolioNoSelection ()
{
  setButtonStatus(0, FALSE);
  setButtonStatus(2, FALSE);
  setButtonStatus(3, FALSE);
}

