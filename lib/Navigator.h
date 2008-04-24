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

#ifndef NAVIGATOR_HPP
#define NAVIGATOR_HPP

#include <qstring.h>
#include <qlistbox.h>
#include <qdir.h>

class Navigator : public QListBox
{
  Q_OBJECT

  signals:
    void fileSelected (QString);
    void noSelection ();
    void keyPress (int, int);
    void signalKeyPressed (int, int, int, int, QString);
    void fileOpened (QString);

  public:
    Navigator (QWidget *, QString &);
    ~Navigator ();
    void getFileSelection (QString &);
    void setDirectory (QString &);
    void getCurrentPath (QString &);
    void setFilter (QString &);
    void setId (int);
    void setKeyFlag (bool);
    void setHome ();
    bool isSelected ();
    void setNavItem (QString);

  public slots:
    void upDirectory ();
    void fileSelection (QListBoxItem *);
    void checkDirectory (QListBoxItem *);
    void updateList ();
    void doKeyPress (QKeyEvent *);
    //void highL (QListBoxItem *);
    void slotMouseClicked(int , QListBoxItem *, const QPoint &);

  protected:
    virtual void keyPressEvent (QKeyEvent *);
  
    QString basePath;
    QDir currentDir;
    int id;
    bool keyFlag;
    bool selectedFlag;
};

#endif
