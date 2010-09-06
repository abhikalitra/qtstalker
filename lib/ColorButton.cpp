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

#include "ColorButton.h"

#include <QColorDialog>

ColorButton::ColorButton (QWidget *w, QColor &c) : QPushButton (w)
{
  _color = c;
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(colorDialog()));
  setMaximumHeight(25);
  _pix = QPixmap(50, 15);
  _readonly = FALSE;
  _changed = FALSE;
  
  setColorButton();
}

void ColorButton::setColorButton ()
{
  _pix.fill(_color);
  setIcon(QIcon(_pix));
}

QColor & ColorButton::color ()
{
  return _color;
}

void ColorButton::colorDialog ()
{
  if (_readonly)
    emit robPressed(_color);
  else
  {
    QColor c = QColorDialog::getColor(_color, this, 0);
    if (c.isValid())
    {
      if (_color != c)
      {
        _color = c;
        setColorButton();
        _changed = TRUE;
        emit valueChanged();
      }
    }
  }
}

void ColorButton::setColor (QColor c)
{
  _color = c;
  _pix.fill(_color);
  setIcon(QIcon(_pix));
}

void ColorButton::setDialogOff ()
{
  _readonly = TRUE;
}

int ColorButton::isChanged()
{
  return _changed;
}
