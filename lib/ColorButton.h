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

#ifndef COLORBUTTON_HPP
#define COLORBUTTON_HPP

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qwidget.h>

class ColorButton : public QPushButton
{
  Q_OBJECT

  public:
    ColorButton (QWidget *, QColor);
    ~ColorButton ();
    QColor getColor ();
    void setColorButton ();
    void setColor (QColor);

  public slots:
    void colorDialog ();
        
  private:
    QColor color;
    QPixmap pix;
};

#endif
