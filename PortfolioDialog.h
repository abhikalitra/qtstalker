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

#ifndef PORTFOLIODIALOG_HPP
#define PORTFOLIODIALOG_HPP

#include "Config.h"
#include <qstring.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qvalidator.h>

class PortfolioDialog : public QDialog
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
    void buttonStatus ();

  private:
    Config *config;
    QPushButton *modifyButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QListView *list;
    QListViewItem *item;
    QListViewItem *parent;
    QString portfolio;
};

#endif
