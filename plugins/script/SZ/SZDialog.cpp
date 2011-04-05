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

#include "SZDialog.h"
#include "Globals.h"
#include "SZ.h"

#include <QtDebug>
#include <QFormLayout>

SZDialog::SZDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
}

void SZDialog::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  setLayout(form);

  // output
  _output = new QLineEdit(_settings->data("OUTPUT"));
  form->addRow(tr("Output"), _output);

  // method
  SZ sz;
  QStringList l = sz.method();
  
  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(_settings->data("METHOD"), Qt::MatchExactly));
  form->addRow(tr("Method"), _method);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt("PERIOD"));
  form->addRow(tr("Period"), _period);

  // no decline period
  _ndperiod = new QSpinBox;
  _ndperiod->setRange(1, 100000);
  _ndperiod->setValue(_settings->getInt("PERIOD_NO_DECLINE"));
  form->addRow(tr("No Decline Period"), _ndperiod);

  // coefficient
  _coeff = new QDoubleSpinBox;
  _coeff->setRange(0, 100000);
  _coeff->setValue(_settings->getDouble("COEFFICIENT"));
  form->addRow(tr("Coefficient"), _coeff);

  // color
  _color = new ColorButton(this, QColor(_settings->data("COLOR")));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  l = c.list();

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data("STYLE"), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_settings->getInt("Z"));
  form->addRow(tr("Plot Order"), _z);
}

void SZDialog::save ()
{
  _settings->setData("METHOD", _method->currentText());
  _settings->setData("COLOR", _color->color().name());
  _settings->setData("STYLE", _style->currentText());
  _settings->setData("PERIOD_NO_DECLINE", _ndperiod->value());
  _settings->setData("COEFFICIENT", _coeff->value());
  _settings->setData("PERIOD", _period->value());
  _settings->setData("OUTPUT", _output->text());
  _settings->setData("Z", _z->text());
}
