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

#ifndef NAVIGATORTAB_HPP
#define NAVIGATORTAB_HPP

#include <qpopupmenu.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qbuttongroup.h>

class NavigatorTab : public QWidget
{
  Q_OBJECT

  signals:
    void signalPositionChanged (int);
  
  public:
    NavigatorTab (QWidget *);
    ~NavigatorTab ();
    void loadSettings();
    void saveSettings();
    int getPosition ();
    void addWidget (QWidget *w, int);

  public slots:
    void togglePosition (int);
    void buttonPressed (int);
    void pressButton (int);

  protected:
    virtual void contextMenuEvent (QContextMenuEvent *);
  
  private:
    int position;
    QPopupMenu *menu;
    QPopupMenu *positionMenu;
    QWidgetStack *stack;
    QButtonGroup *bg;
};

#endif
