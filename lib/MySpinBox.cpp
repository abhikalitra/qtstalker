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

#include "MySpinBox.h"


MySpinBox::MySpinBox (QWidget *w, int t) : QSpinBox (w)
{
  keyFlag = FALSE;
  type = t;
}

MySpinBox::~MySpinBox ()
{
}

void MySpinBox::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (type, key->state(), key->key(), key->ascii(), key->text());
    
  QSpinBox::keyPressEvent(key);  
}

void MySpinBox::setKeyFlag (bool d)
{
  keyFlag = d;
}

void MySpinBox::doKeyPress (QKeyEvent *key)
{
  QSpinBox::keyPressEvent(key);  
}

