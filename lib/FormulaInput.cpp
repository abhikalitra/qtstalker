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

#include "FormulaInput.h"
#include "BarData.h"
#include <qlayout.h>
#include <qvalidator.h>

#define BUTTON_SIZE 24

FormulaInput::FormulaInput (QWidget *w, bool f, QString &in) : QWidget(w)
{
  flag = f;
  
  BarData *it = new BarData;
  it->getInputFields(inputTypeList);
  delete it;

  QGridLayout *grid = new QGridLayout(this, 1, 2);
  grid->setMargin(0);
  grid->setSpacing(5);
  grid->setColStretch(0, 1);
  
  method = new QComboBox(this);
  method->insertItem(tr("Step"), -1);
  if (flag)
    method->insertItem(tr("Value"), -1);
  QObject::connect(method, SIGNAL(activated(int)), this, SLOT(buttonChecked(int)));
  grid->addWidget(method, 0, 0);
  
  stack = new QWidgetStack(this);
  grid->addWidget(stack, 0, 1);
  
  line = new QSpinBox(1, 100, 1, this);
  stack->addWidget(line, 1);

  QDoubleValidator *dv = new QDoubleValidator(-99999999, 99999999, 4, this, 0);
  
  edit = new QLineEdit(this);
  edit->setValidator(dv);
  edit->setText("0");
  stack->addWidget(edit, 2);
  
  setInput(in);
}

FormulaInput::~FormulaInput ()
{
}

void FormulaInput::buttonChecked (int id)
{
  stack->raiseWidget(id + 1);
}

QString FormulaInput::getInput ()
{
  QString s;
  int id = stack->id(stack->visibleWidget());
  switch (id)
  {
    case 1:
      s = line->text();
      break;
    case 2:
      s = edit->text();
      if (! s.length())
        s = "#0";
      else
        s.prepend("#");
      break;
    default:
      break;
  }
  
  return s;
}

void FormulaInput::setInput (QString &d)
{
  if (d.contains("#") && flag)
  {
    method->setCurrentItem(1);
    edit->setText(d.right(d.length() - 1));
    stack->raiseWidget(2);
  }
  else
  {
    method->setCurrentItem(0);
    line->setValue(d.toInt());
    stack->raiseWidget(1);
  }
}


