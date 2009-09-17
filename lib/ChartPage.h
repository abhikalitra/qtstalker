/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef CHARTPAGE_HPP
#define CHARTPAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QLineEdit>
#include <QListWidget>
#include <QKeyEvent>
#include <QPoint>
#include <QToolButton>
#include <QComboBox>



class ChartPage : public QWidget
{
  Q_OBJECT

  signals:
    void fileSelected (QString);
    void signalAddToGroup ();
    void signalReloadChart ();
    void addRecentChart (QString);

  public:
    ChartPage (QWidget *);

  public slots:
    void rightClick (const QPoint &);
    void doKeyPress (QKeyEvent *);
    void chartOpened (QListWidgetItem *);
    void addToGroup ();
    void updateList ();
    void symbolSearch ();

  protected:
    int activeSearch;
    QString searchString;
    QListWidget *nav;
    QMenu *menu;
    QToolButton *allButton;
    QToolButton *symbolButton;
};

#endif

