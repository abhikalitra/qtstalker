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
#include "Setting.h"
#include "IndicatorPlugin.h"


class IndicatorPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalDisableIndicator (QString);
    void signalEnableIndicator (QString);
    void signalEditIndicator (QString);
    void signalDeleteIndicator (QString);
    void signalNewIndicator (QString);
  
  public:
    IndicatorPage (QWidget *);
    ~IndicatorPage ();

  public slots:
    void doubleClick (QListWidgetItem *);
    void updateList ();
    void rightClick (const QPoint &);
    void newIndicator ();
    void editIndicator (QString);
    void editIndicator ();
    void deleteIndicator ();
    void itemSelected ();
    void changeIndicator (QString &);
    void doKeyPress (QKeyEvent *key);

  protected:
    virtual void keyPressEvent (QKeyEvent *);
    
    QListWidget *list;
    QMenu *menu;
    bool updateEnableFlag;
    QList<QAction *> actionList;
};

#endif
