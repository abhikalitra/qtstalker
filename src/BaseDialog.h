/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#ifndef BASEDIALOG_HPP
#define BASEDIALOG_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qlist.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qtoolbutton.h>

class BaseDialog : public QWidget
{
  public:
    BaseDialog (QWidget *);
    ~BaseDialog ();
    void setButton (QPixmap, QString, int);
    QToolButton * getButton (int);
    void setButtonStatus (int, bool);

  protected:
    QList<QToolButton> buttonList;
    QGridLayout *toolbar;
    QVBoxLayout *basebox;
};

#endif
