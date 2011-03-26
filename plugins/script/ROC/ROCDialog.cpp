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

#include "ROCDialog.h"
#include "Globals.h"
#include "InputType.h"
#include "ROC.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>

ROCDialog::ROCDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "ROCDialog_window_size";
  _keyPos = "ROCDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "ROC" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();

  loadSettings();
}

void ROCDialog::createGeneralPage ()
{
  // method
  ROC roc;
  QStringList l = roc.method();

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(_settings->data(ROC::_METHOD), Qt::MatchExactly));
  _form->addRow(tr("Method"), _method);

  // input
  InputType it;
  l = it.list();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data(ROC::_INPUT), Qt::MatchExactly));
  _form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(ROC::_PERIOD));
  _form->addRow(tr("Period"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(ROC::_COLOR)));
  _color->setColorButton();
  _form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(ROC::_STYLE), Qt::MatchExactly));
  _form->addRow(tr("Style"), _style);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings->getInt(ROC::_SMOOTHING));
  _form->addRow(tr("Smoothing"), _smoothing);

  // type
  MAType mat;
  l = mat.list();

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(_settings->data(ROC::_SMOOTHING_TYPE), Qt::MatchExactly));
  _form->addRow(tr("Smoothing Type"), _smoothingType);

  // make room unused
  _message->hide();
}

void ROCDialog::done ()
{
  _settings->setData(ROC::_METHOD, _method->currentText());
  _settings->setData(ROC::_INPUT, _input->currentText());
  _settings->setData(ROC::_COLOR, _color->color().name());
  _settings->setData(ROC::_STYLE, _style->currentText());
  _settings->setData(ROC::_SMOOTHING, _smoothing->value());
  _settings->setData(ROC::_SMOOTHING_TYPE, _smoothingType->currentText());
  _settings->setData(ROC::_PERIOD, _period->value());

  saveSettings();

  accept();
}
