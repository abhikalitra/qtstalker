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

#ifndef MACROPAGE_HPP
#define MACROPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qlistbox.h>
#include "Config.h"

class MacroPage : public QListBox
{
  Q_OBJECT

  signals:
    void signalRunMacro (QString);

  public:
    MacroPage (QWidget *);
    ~MacroPage ();

  public slots:
    void openMacro ();
    void openMacro (QString);
    void renameMacro ();
    void newMacro ();
    void deleteMacro ();
    void macroSelected (const QString &);
    void rightClick (QListBoxItem *);
    void slotHelp ();
    void doubleClick (QListBoxItem *);
    void updateList ();
    void doKeyPress (QKeyEvent *);
    void runMacro ();

  private:
    virtual void keyPressEvent (QKeyEvent *);
  
    Config config;
    QPopupMenu *menu;
};

#endif
