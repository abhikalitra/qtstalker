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

#include "MyListBox.h"


MyListBox::MyListBox (QWidget *w, int t) : QListBox (w)
{
  keyFlag = FALSE;
  type = t;
}

MyListBox::~MyListBox ()
{
}

void MyListBox::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (type, key->state(), key->key(), key->ascii(), key->text());
    
  QListBox::keyPressEvent(key);  
}

void MyListBox::setKeyFlag (bool d)
{
  keyFlag = d;
}

void MyListBox::doKeyPress (QKeyEvent *key)
{
  QListBox::keyPressEvent(key);  
}

