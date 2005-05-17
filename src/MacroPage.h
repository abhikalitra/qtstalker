/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include "Macro.h"


class MacroPage : public QListBox
{
  Q_OBJECT

  signals:
    void signalRunMacro (QString);
    void signalRecordMacro (QString);
    void signalKeyPressed (int, int, int, int, QString);

  public:
  
    enum HotKey
    {
      NewMacro,
      DeleteMacro,
      EditMacro,
      RenameMacro,
      AssignMacro,
      RunMacro,
      Help
    };
  
    MacroPage (QWidget *);
    ~MacroPage ();

  public slots:
    void editMacro ();
    void renameMacro ();
    void newMacro ();
    void deleteMacro ();
    void assignMacro ();
    void macroSelected (const QString &);
    void rightClick (QListBoxItem *);
    void slotHelp ();
    void doubleClick (QListBoxItem *);
    void updateList ();
    void doKeyPress (QKeyEvent *);
    void runMacro ();
    void setKeyFlag (bool);
    void slotAccel (int);
    void runMacro (Macro *);

  private:
    virtual void keyPressEvent (QKeyEvent *);
  
    Config config;
    QPopupMenu *menu;
    bool keyFlag;
    Macro *tmacro;
    bool macroFlag;
};

#endif
