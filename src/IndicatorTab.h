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

#ifndef INDICATORTAB_HPP
#define INDICATORTAB_HPP

#include <qtabbar.h>
#include <qwidgetstack.h>
#include <qlayout.h>

#include "Plot.h"
#include "Setting.h"


class IndicatorTab : public QWidget
{
  Q_OBJECT

  public:
    IndicatorTab (QWidget *);
    ~IndicatorTab ();
    void drawCurrent ();
    int getInsertIndex (QString &);
    bool deleteTab (QString &);
    void getTabList (QStringList &);
    void insertTab (QWidget *, QString, int);

  private slots:
    void slotTabChanged (int);
    
  private:
    QTabBar *tab;
    QWidgetStack *stack;
    QVBoxLayout *vbox;
    Setting idList;
};

#endif
