/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#ifndef REFRESH_ACTION_HPP
#define REFRESH_ACTION_HPP

#include <QAction>
#include <QTimer>
#include <QContextMenuEvent>

class RefreshAction : public QAction
{
  Q_OBJECT
  
  signals:
    void signalRefresh ();

  public:
    RefreshAction (QObject *);
    
  public slots:
    void refreshChart (bool status);
    void refreshUpdated (int);
    void dialog ();
    void changeText ();

  protected:
    virtual void contextMenuEvent (QContextMenuEvent *);
    
  protected:
    QTimer *_timer;
    int _minutes;
};

#endif


