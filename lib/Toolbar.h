/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include <qstring.h>
#include <qpushbutton.h>
//#include <qtoolbutton.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qdict.h>
#include <qframe.h>

class Toolbar : public QFrame
{
  public:
    Toolbar (QWidget *w, int h, int w, bool);
    ~Toolbar ();
    void addButton (QString &name, QPixmap pix, QString &tt);
    QPushButton * getButton (QString &name);
    void setButtonStatus (QString &name, bool d);
    void addSeparator ();

  private:
    QPushButton *cancelButton;
    QDict<QPushButton> list;
    int height;
    int width;
    QGridLayout *grid;
    bool pflag;
};

#endif
