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

#ifndef SCANNERPAGE_HPP
#define SCANNERPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qlistbox.h>
#include "Config.h"
#include "Macro.h"


class ScannerPage : public QListBox
{
  Q_OBJECT

  signals:
    void message (QString);
    void signalKeyPressed (int, int, int, int, QString);
  
  public:
  
    enum HotKey
    {
      NewScanner,
      OpenScanner,
      DeleteScanner,
      RenameScanner,
      RunScanner,
      Help
    };
  
    ScannerPage (QWidget *);
    ~ScannerPage ();

  public slots:
    void openScanner ();
    void openScanner (QString);
    void renameScanner ();
    void newScanner ();
    void deleteScanner ();
    void runScanner ();
    void scannerSelected (const QString &);
    void rightClick (QListBoxItem *);
    void refreshList ();
    void slotHelp ();
    void doubleClick (QListBoxItem *);
    void doKeyPress (QKeyEvent *);
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
