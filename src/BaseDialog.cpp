/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "BaseDialog.h"
#include <qtooltip.h>

#define BUTTON_SIZE 24

BaseDialog::BaseDialog (QWidget *w) : QWidget(w)
{
  buttonList.setAutoDelete(TRUE);

  basebox = new QVBoxLayout(this);
  basebox->setMargin(2);
  basebox->setSpacing(5);

  toolbar = new QGridLayout(basebox, 1, 1);
  toolbar->setSpacing(1);
}

BaseDialog::~BaseDialog ()
{
}

void BaseDialog::setButton (QPixmap pix, QString tt, int pos)
{
  QToolButton *button = new QToolButton(this);
  QToolTip::add(button, tt);
  button->setPixmap(pix);
  button->setMaximumWidth(BUTTON_SIZE);
  button->setMaximumHeight(BUTTON_SIZE);
  button->setAutoRaise(TRUE);
  toolbar->addWidget(button, 0, pos);
  buttonList.append(button);
  toolbar->expand(1, toolbar->numCols() + 1);
}

QToolButton * BaseDialog::getButton (int pos)
{
  return buttonList.at(pos);
}

void BaseDialog::setButtonStatus (int pos, bool status)
{
  QToolButton *button = buttonList.at(pos);
  button->setEnabled(status);
}


