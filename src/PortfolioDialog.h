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

#ifndef PORTFOLIODIALOG_HPP
#define PORTFOLIODIALOG_HPP

#include <qlistview.h>
#include <qstring.h>
#include <qtabdialog.h>
#include "Config.h"
#include "Toolbar.h"

class PortfolioDialog : public QTabDialog
{
  Q_OBJECT

  public:
    PortfolioDialog (Config *, QString);
    ~PortfolioDialog ();
    void updatePortfolio ();
    float futuresProfit (QString, float);
    void updatePortfolioItems ();

  public slots:
    void modifyItem ();
    void addItem ();
    void deleteItem ();
    void savePortfolio ();
    void buttonStatus (QListViewItem *);

  private:
    QListView *plist;
    QListViewItem *item;
    QString portfolio;
    Toolbar *toolbar;
    Config *config;
};

#endif
