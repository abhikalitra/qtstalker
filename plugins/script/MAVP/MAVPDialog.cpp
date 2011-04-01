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

#include "MAVPDialog.h"
#include "Globals.h"
#include "InputType.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

MAVPDialog::MAVPDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
}

void MAVPDialog::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  setLayout(form);

  // name
  _output = new QLineEdit(_settings->data("OUTPUT"));
  form->addRow(tr("Output"), _output);

  // input
  InputType it;
  QStringList l = it.list();
  l.append(_settings->data("INPUT"));
  l.append(_settings->data("INPUT2"));
  l.removeDuplicates();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data("INPUT"), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // input2
  _input2 = new QComboBox;
  _input2->addItems(l);
  _input2->setCurrentIndex(_input2->findText(_settings->data("INPUT2"), Qt::MatchExactly));
  form->addRow(tr("Input"), _input2);

  // ma type
  MAType mat;
  l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data("MA_TYPE"), Qt::MatchExactly));
  form->addRow(tr("MA"), _maType);

  // min
  _min = new QSpinBox;
  _min->setRange(2, 100000);
  _min->setValue(_settings->getInt("PERIOD_MIN"));
  form->addRow(tr("Min Period"), _min);

  // max
  _max = new QSpinBox;
  _max->setRange(2, 100000);
  _max->setValue(_settings->getInt("PERIOD_MAX"));
  form->addRow(tr("Max Period"), _max);

  // color
  _color = new ColorButton(this, QColor(_settings->data("COLOR")));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

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

void MAVPDialog::save ()
{
  _settings->setData("MA_TYPE", _maType->currentText());
  _settings->setData("INPUT", _input->currentText());
  _settings->setData("INPUT2", _input2->currentText());
  _settings->setData("PERIOD_MIN", _min->value());
  _settings->setData("PERIOD_MAX", _max->value());
  _settings->setData("COLOR", _color->color().name());
  _settings->setData("STYLE", _style->currentText());
  _settings->setData("OUTPUT", _output->text());
  _settings->setData("Z", _z->text());
}
