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

#include "RSIDialog.h"
#include "Globals.h"
#include "InputType.h"
#include "RSI.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>

RSIDialog::RSIDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "RSIDialog_window_size";
  _keyPos = "RSIDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "RSI" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createRefPage();
  createRef2Page();

  loadSettings();
}

void RSIDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // input
  InputType it;
  QStringList l = it.list();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data(RSI::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(RSI::_PERIOD));
  form->addRow(tr("Period"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(RSI::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(RSI::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings->getInt(RSI::_SMOOTHING));
  form->addRow(tr("Smoothing"), _smoothing);

  // type
  MAType mat;
  l = mat.list();

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(_settings->data(RSI::_SMOOTHING_TYPE), Qt::MatchExactly));
  form->addRow(tr("Smoothing Type"), _smoothingType);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "RSI");
}

void RSIDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data(RSI::_COLOR_REF1)));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings->getDouble(RSI::_REF1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void RSIDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data(RSI::_COLOR_REF2)));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings->getDouble(RSI::_REF2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void RSIDialog::done ()
{
  _settings->setData(RSI::_INPUT, _input->currentText());
  _settings->setData(RSI::_COLOR, _color->color().name());
  _settings->setData(RSI::_STYLE, _style->currentText());
  _settings->setData(RSI::_SMOOTHING, _smoothing->value());
  _settings->setData(RSI::_SMOOTHING_TYPE, _smoothingType->currentText());
  _settings->setData(RSI::_PERIOD, _period->value());
  _settings->setData(RSI::_REF1, _ref->value());
  _settings->setData(RSI::_REF2, _ref2->value());
  _settings->setData(RSI::_COLOR_REF1, _refColor->color().name());
  _settings->setData(RSI::_COLOR_REF2, _ref2Color->color().name());

  saveSettings();

  accept();
}
