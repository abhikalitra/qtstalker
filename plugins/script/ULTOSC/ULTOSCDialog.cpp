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

#include "ULTOSCDialog.h"
#include "Globals.h"
#include "ULTOSC.h"

#include <QtDebug>
#include <QStringList>

ULTOSCDialog::ULTOSCDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "ULTOSCDialog_window_size";
  _keyPos = "ULTOSCDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "ULTOSC" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createRefPage();
  createRef2Page();
  createRef3Page();

  loadSettings();
}

void ULTOSCDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _color = new ColorButton(this, QColor(_settings->data(ULTOSC::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(ULTOSC::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // short period
  _speriod = new QSpinBox;
  _speriod->setRange(1, 100000);
  _speriod->setValue(_settings->getInt(ULTOSC::_PERIOD_SHORT));
  form->addRow(tr("Short Period"), _speriod);

  // med period
  _mperiod = new QSpinBox;
  _mperiod->setRange(1, 100000);
  _mperiod->setValue(_settings->getInt(ULTOSC::_PERIOD_MED));
  form->addRow(tr("Medium Period"), _mperiod);

  // long period
  _lperiod = new QSpinBox;
  _lperiod->setRange(1, 100000);
  _lperiod->setValue(_settings->getInt(ULTOSC::_PERIOD_LONG));
  form->addRow(tr("Long Period"), _lperiod);

  _tabs->addTab(w, "ULTOSC");
}

void ULTOSCDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data(ULTOSC::_COLOR_REF1)));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(0, 100);
  _ref->setValue(_settings->getDouble(ULTOSC::_REF1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void ULTOSCDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data(ULTOSC::_COLOR_REF2)));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(0, 100);
  _ref2->setValue(_settings->getDouble(ULTOSC::_REF2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void ULTOSCDialog::createRef3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref3Color = new ColorButton(this, QColor(_settings->data(ULTOSC::_COLOR_REF3)));
  _ref3Color->setColorButton();
  form->addRow(tr("Color"), _ref3Color);

  // ref
  _ref3 = new QDoubleSpinBox;
  _ref3->setRange(0, 100);
  _ref3->setValue(_settings->getDouble(ULTOSC::_REF3));
  form->addRow(tr("Value"), _ref3);

  _tabs->addTab(w, "REF 3");
}

void ULTOSCDialog::done ()
{
  _settings->setData(ULTOSC::_COLOR, _color->color().name());
  _settings->setData(ULTOSC::_STYLE, _style->currentText());
  _settings->setData(ULTOSC::_PERIOD_SHORT, _speriod->value());
  _settings->setData(ULTOSC::_PERIOD_MED, _mperiod->value());
  _settings->setData(ULTOSC::_PERIOD_LONG, _lperiod->value());
  _settings->setData(ULTOSC::_REF1, _ref->value());
  _settings->setData(ULTOSC::_REF2, _ref2->value());
  _settings->setData(ULTOSC::_REF3, _ref3->value());
  _settings->setData(ULTOSC::_COLOR_REF1, _refColor->color().name());
  _settings->setData(ULTOSC::_COLOR_REF2, _ref2Color->color().name());
  _settings->setData(ULTOSC::_COLOR_REF3, _ref3Color->color().name());

  saveSettings();

  accept();
}
