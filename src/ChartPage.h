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

#ifndef CHARTPAGE_HPP
#define CHARTPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qlineedit.h>
#include "Navigator.h"
#include "Config.h"

class ChartPage : public QWidget
{
  Q_OBJECT

  signals:
    void fileSelected (QString);

  public:
    ChartPage (QWidget *);
    ~ChartPage ();
    void exportChart (QString);
    void setFocus ();
    Navigator * getNav ();

  public slots:
    void deleteChart ();
    void editChart ();
    void newChart (int);
    void exportSymbol ();
    void dumpSymbol ();
    void exportChart (QString, bool);
    void chartSelected (QString);
    void chartNoSelection ();
    void rightClick (QListBoxItem *);
    void refreshList ();
    void searchChanged (const QString &);
    void slotHelp ();
    void doKeyPress (QKeyEvent *);

  protected:
    Navigator *nav;
    Config config;
    QPopupMenu *menu;
    QPopupMenu *newMenu;
    QLineEdit *search;
};

#endif
