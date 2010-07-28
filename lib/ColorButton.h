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

// *************************************************************************************************
// Custom class for PrefDialog to display a nice color on a button so we can display the color
// without having to bring up the color dialog first. When pressed, displays the color selection dialog.
// Used by PrefDialog
// *************************************************************************************************

#ifndef COLORBUTTON_HPP
#define COLORBUTTON_HPP

#include <QPushButton>
#include <QPixmap>
#include <QColor>
#include <QWidget>

class ColorButton : public QPushButton
{
  Q_OBJECT

  public:
    ColorButton (QWidget *, QColor &);
    void getColor (QColor &);
    void setColorButton ();
    void setDialogOff();
    QPixmap pix;
    bool isChanged();
    
  public 
    slots:
      void colorDialog ();
      void setColor (QColor);
    
    signals:
      void valueChanged();
        
  private:
    QColor color;
    //QPixmap pix;
    bool readonly;
    bool changed;
  
    signals:
      void robPressed(QColor); // read only button pressed

};

#endif
