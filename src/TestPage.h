/*
 *  Qtstalker stock charter
 *
 *  Copyright (c) 2001-2004 Stefan S. Stratigakos
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

#ifndef TESTPAGE_HPP
#define TESTPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qlistbox.h>
#include "Config.h"

class TestPage : public QWidget
{
  Q_OBJECT

  public:
    TestPage (QWidget *, Config *);
    ~TestPage ();

  public slots:
    void openTest ();
    void renameTest ();
    void newTest ();
    void deleteTest ();
    void testSelected (const QString &);
    void testNoSelection ();
    void rightClick (QListBoxItem *);
    void updateList ();

  private:
    Config *config;
    QListBox *list;
    QPopupMenu *menu;
};

#endif
