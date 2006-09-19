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

#include "Toolbar.h"
#include <qtooltip.h>

Toolbar::Toolbar (QWidget *w, int h, int wi, bool f) : QFrame (w)
{
  height = h;
  width = wi;
  pflag = f;
  list.setAutoDelete(TRUE);
  
  setFrameShape(Box);
  setFrameShadow(Sunken);
  setLineWidth(1);

  QHBoxLayout *hbox = 0;
  QVBoxLayout *vbox = 0;
  
  if (pflag)
  {
    setMinimumWidth(width + 3);
    setMaximumWidth(width + 3);
    vbox = new QVBoxLayout(this);  
    vbox->setSpacing(0);
    vbox->setMargin(1);
    grid = new QGridLayout(vbox, 1, 1);  
  }
  else
  {
    setMinimumHeight(height + 3);
    setMaximumHeight(height + 3);
    hbox = new QHBoxLayout(this);  
    hbox->setSpacing(0);
    hbox->setMargin(1);
    grid = new QGridLayout(hbox, 1, 1);  
  }
  
  grid->setSpacing(0);
  grid->setMargin(0);
  
  if (pflag)
    vbox->addStretch(1);
  else
    hbox->addStretch(1);
}

Toolbar::~Toolbar ()
{
}

void Toolbar::addButton (QString &name, QPixmap pix, QString &tt)
{
  QPushButton *button = new QPushButton(this);
  QToolTip::add(button, tt);
  button->setPixmap(pix);
  button->setMaximumWidth(width);
  button->setMaximumHeight(height);
  if (pflag)
    grid->addWidget(button, list.count(), 0);
  else
    grid->addWidget(button, 0, list.count());
  list.replace(name, button);
}

QPushButton * Toolbar::getButton (QString &name)
{
  return list[name];
}

void Toolbar::setButtonStatus (QString &name, bool d)
{
  QPushButton *button = list[name];
  if (button)
    button->setEnabled(d);
}

void Toolbar::addSeparator ()
{
  QFrame *vline = new QFrame(this);
  vline->setFrameStyle(QFrame::VLine | QFrame::Sunken);
  if (pflag)
    grid->addWidget(vline, -1, 0);
  else
    grid->addWidget(vline, 0, -1);
}

