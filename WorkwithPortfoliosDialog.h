/*
 *  Qtstalker stock charter
 *
 *  Copyright (c) 2001,2002 Stefan S. Stratigakos
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

#ifndef WORKWITHPORTFOLIOSDIALOG_HPP
#define WORKWITHPORTFOLIOSDIALOG_HPP

#include <qtoolbutton.h>
#include <qlistview.h>
#include <qdialog.h>
#include <qlayout.h>
#include "Config.h"

class WorkwithPortfoliosDialog : public QDialog
{
  Q_OBJECT

  public:
    WorkwithPortfoliosDialog (Config *);
    ~WorkwithPortfoliosDialog ();
    void updateList ();

  public slots:
    void openPortfolio ();
    void renamePortfolio ();
    void newPortfolio ();
    void deletePortfolio ();
    void editPortfolio ();
    void portfolioSelected (QListViewItem *);

  private:
    QListView *list;
    QListViewItem *item;
    Config *config;
    QToolButton *cancelButton;
    QToolButton *openButton;
    QToolButton *editButton;
    QToolButton *deleteButton;
    QToolButton *newButton;
    QToolButton *renameButton;
    QGridLayout *toolbar;
};

#endif
