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

#include "VBPDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QFormLayout>

VBPDialog::VBPDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
}

void VBPDialog::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  setLayout(form);

  // output
  _output = new QLineEdit(_settings->data("OUTPUT"));
  form->addRow(tr("Output"), _output);
  
  // up color
  _upColor = new ColorButton(this, QColor(_settings->data("COLOR_UP")));
  _upColor->setColorButton();
  form->addRow(tr("Up Volume Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data("COLOR_DOWN")));
  _downColor->setColorButton();
  form->addRow(tr("Down Volume Color"), _downColor);

  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_settings->getInt("Z"));
  form->addRow(tr("Plot"), _z);
}

void VBPDialog::save ()
{
  _settings->setData("COLOR_UP", _upColor->color().name());
  _settings->setData("COLOR_DOWN", _downColor->color().name());
  _settings->setData("Z", _z->text());
  _settings->setData("OUTPUT", _output->text());
}
