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

#include "MyComboBox.h"


MyComboBox::MyComboBox (QWidget *w, int t) : QComboBox (w)
{
  keyFlag = FALSE;
  type = t;
}

MyComboBox::~MyComboBox ()
{
}

void MyComboBox::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (type, key->state(), key->key(), key->ascii(), key->text());
    
  QComboBox::keyPressEvent(key);  
}

void MyComboBox::setKeyFlag (bool d)
{
  keyFlag = d;
}

void MyComboBox::doKeyPress (QKeyEvent *key)
{
  QComboBox::keyPressEvent(key);  
}

