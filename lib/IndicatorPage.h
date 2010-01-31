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

#ifndef INDICATORPAGE_HPP
#define INDICATORPAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QListWidget>
#include <QKeyEvent>
#include <QList>
#include <QAction>
#include <QToolButton>

#include "Setting.h"


class IndicatorPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalDisableIndicator (QString);
    void signalEnableIndicator (QString);
    void signalDeleteIndicator (QString);
    void signalNewIndicator (QString);

  public:
    IndicatorPage (QWidget *);

  public slots:
    void doubleClick (QListWidgetItem *);
    void rightClick (const QPoint &);
    void newIndicator ();
    void editIndicator ();
    void editIndicator (QString &);
    void deleteIndicator ();
    void doKeyPress (QKeyEvent *key);
    void indicatorSearch ();
    void showAll ();
    void showActive ();

  protected:
    virtual void keyPressEvent (QKeyEvent *);

    int listFlag;
    QListWidget *list;
    QMenu *menu;
    QList<QAction *> actions;
    QToolButton *searchButton;
    QToolButton *allButton;
    QToolButton *activeButton;
};

#endif

