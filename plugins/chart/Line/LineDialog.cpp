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

#include "LineDialog.h"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>

LineDialog::LineDialog () : QTabDialog (0, "LineDialog", TRUE)
{
  setCaption(tr("Line Chart Parms"));
  
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  QGridLayout *grid = new QGridLayout(vbox, 3, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  QLabel *label = new QLabel(tr("Color"), w);
  grid->addWidget(label, 0, 0);
  
  color = new ColorButton(w, QColor("green"));
  grid->addWidget(color, 0, 1);
  color->setColorButton();
    
  label = new QLabel(tr("Min Bar Spacing"), w);
  grid->addWidget(label, 1, 0);
    
  spacing = new QSpinBox(1, 99, 1, w);
  grid->addWidget(spacing, 1, 1);
  
  vbox->addSpacing(10);
  
  list = new FormulaEdit(w);
  vbox->addWidget(list);
  
  addTab(w, tr("Parms"));
  
  setOkButton();
  setCancelButton();
  resize(400, 300);
}

LineDialog::~LineDialog ()
{
}

int LineDialog::getLines ()
{
  return list->getLines();
}

QString LineDialog::getLine (int row)
{
  return list->getLine(row);
}

void LineDialog::setLine (QString d)
{
  list->setLine(d);
}

void LineDialog::setColor (QColor c)
{
  color->setColor(c);
}

QColor LineDialog::getColor ()
{
  return color->getColor();
}

void LineDialog::setSpacing (int d)
{
  spacing->setValue(d);
}

int LineDialog::getSpacing ()
{
  return spacing->value();
}



