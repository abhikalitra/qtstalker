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


#ifndef TOOLBARBTN_HPP
#define TOOLBARBTN_HPP

#include <qpushbutton.h>
    
    
// This Class is intended to use at all tool bar buttons
// So it is more ease to keep a consistent look
// loh.tar

class ToolBarBtn : public QPushButton
{
  Q_OBJECT
      
  public:
    ToolBarBtn (QWidget *parent);
   ~ToolBarBtn ();
   
   enum Dimensions
   {
      btnMinWidth = 25,
      btnMaxWidth = 25,
      btnMinHeight = 25,
      btnMaxHeight = 25
   };
};

#endif

