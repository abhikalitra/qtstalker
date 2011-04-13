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

#include "SARDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

SARDialog::SARDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
}

void SARDialog::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  setLayout(form);

  // output
  _output = new QLineEdit(_settings->data("OUTPUT"));
  form->addRow(tr("Output"), _output);

  // color
  _color = new ColorButton(this, QColor(_settings->data("COLOR")));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // initial
  _init = new QDoubleSpinBox;
  _init->setRange(0, 0.2);
  _init->setDecimals(4);
  _init->setValue(_settings->getDouble("STEP_INITIAL"));
  form->addRow(tr("Initial Step"), _init);

  // max
  _max = new QDoubleSpinBox;
  _max->setRange(0, 0.2);
  _max->setDecimals(4);
  _max->setValue(_settings->getDouble("STEP_MAX"));
  form->addRow(tr("Maximum Step"), _max);

  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_settings->getInt("Z"));
  form->addRow(tr("Plot Order"), _z);
}

void SARDialog::save ()
{
  _settings->setData("COLOR", _color->color().name());
  _settings->setData("STEP_INITIAL", _init->value());
  _settings->setData("STEP_MAX", _max->value());
  _settings->setData("OUTPUT", _output->text());
  _settings->setData("Z", _z->text());
}
