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

#ifndef WORKWITHCHARTS_HPP
#define WORKWITHCHARTS_HPP

#include <qstring.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qpixmap.h>
#include "Config.h"

class WorkwithCharts : public QDialog
{
  Q_OBJECT

  signals:
    void chartOpened (QString);
    void groupOpened (QString);

  public:
    WorkwithCharts (Config *, QString);
    ~WorkwithCharts ();
    void updateList ();
    void updateChartList ();
    void deleteChart ();
    void editChart ();
    void editComposite ();
    void newComposite ();
    void newGroup();
    void editGroup();
    void deleteGroup();
    void renamePortfolio ();
    void openPortfolio ();
    void editPortfolio ();
    void deletePortfolio ();
    void newPortfolio ();

  public slots:
    void deleteSymbol ();
    void openSymbol (QListViewItem *);
    void openSymbol ();
    void newSymbol ();
    void editSymbol ();
    void buttonStatus();
    void exportSymbol ();
    void renameGroup ();
    void renameSymbol ();

  private:
    QString type;
    Config *config;
    QListView *list;
    QListViewItem *item;
    QPushButton *openButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *newButton;
    QPushButton *renameButton;
    QPushButton *exportButton;
};

#endif
