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

#include "ColorButton.h"
#include <qcolordialog.h>

ColorButton::ColorButton (QWidget *w, QColor &c) : QPushButton (w)
{
  color = c;
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(colorDialog()));
  setMaximumHeight(25);
  setToggleButton(FALSE);
  pix.resize(50, 10);
  setToggleType(SingleShot);
}

ColorButton::~ColorButton ()
{
}

void ColorButton::setColorButton ()
{
  pix.fill(color);
  setPixmap(pix);
}

QColor ColorButton::getColor ()
{
  return color;
}

void ColorButton::colorDialog ()
{
  QColor c = QColorDialog::getColor(color, this, 0);
  if (c.isValid())
  {
    color = c;
    setColorButton();
  }
}

void ColorButton::setColor (QColor &c)
{
  color = c;
  pix.fill(color);
  setPixmap(pix);
}


