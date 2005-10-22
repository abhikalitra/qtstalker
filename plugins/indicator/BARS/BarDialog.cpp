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

#include "BarDialog.h"
#include "HelpWindow.h"
#include <qlayout.h>

BarDialog::BarDialog (QString d) : QTabDialog (0, "BarDialog", TRUE)
{
  helpFile = d;
  setCaption(tr("Paint Bar Script"));

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  list = new FormulaEdit(w, FormulaEdit::Logic);
  vbox->addWidget(list);
  
  addTab(w, QString::null);

  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
  resize(400, 300);
}

BarDialog::~BarDialog ()
{
}

void BarDialog::getList (QStringList &d)
{
  d.clear();
  d = QStringList::split("\n", list->getText(), FALSE);
}

void BarDialog::setList (QStringList &d)
{
  int loop;
  for (loop = 0; loop < (int) d.count(); loop++)
    list->setLine(d[loop]);
}

void BarDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}


