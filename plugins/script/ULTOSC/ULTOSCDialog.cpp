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

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

ULTOSCDialog::ULTOSCDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createRefPage();
  createRef2Page();
  createRef3Page();
}

void ULTOSCDialog::createGeneralPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _output = new QLineEdit(_settings->data("OUTPUT"));
  form->addRow(tr("Output"), _output);

  // color
  _color = new ColorButton(this, QColor(_settings->data("COLOR")));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  QStringList l = c.list();

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data("STYLE"), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // short period
  _speriod = new QSpinBox;
  _speriod->setRange(1, 100000);
  _speriod->setValue(_settings->getInt("PERIOD_SHORT"));
  form->addRow(tr("Short Period"), _speriod);

  // med period
  _mperiod = new QSpinBox;
  _mperiod->setRange(1, 100000);
  _mperiod->setValue(_settings->getInt("PERIOD_MED"));
  form->addRow(tr("Medium Period"), _mperiod);

  // long period
  _lperiod = new QSpinBox;
  _lperiod->setRange(1, 100000);
  _lperiod->setValue(_settings->getInt("PERIOD_LONG"));
  form->addRow(tr("Long Period"), _lperiod);

  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_settings->getInt("Z"));
  form->addRow(tr("Plot Order"), _z);
  
  _tabs->addTab(w, "ULTOSC");
}

void ULTOSCDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data("COLOR_REF1")));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(0, 100);
  _ref->setValue(_settings->getDouble("REF1"));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void ULTOSCDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data("COLOR_REF2")));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(0, 100);
  _ref2->setValue(_settings->getDouble("REF2"));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void ULTOSCDialog::createRef3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref3Color = new ColorButton(this, QColor(_settings->data("COLOR_REF3")));
  _ref3Color->setColorButton();
  form->addRow(tr("Color"), _ref3Color);

  // ref
  _ref3 = new QDoubleSpinBox;
  _ref3->setRange(0, 100);
  _ref3->setValue(_settings->getDouble("REF3"));
  form->addRow(tr("Value"), _ref3);

  _tabs->addTab(w, "REF 3");
}

void ULTOSCDialog::save ()
{
  _settings->setData("COLOR", _color->color().name());
  _settings->setData("STYLE", _style->currentText());
  _settings->setData("PERIOD_SHORT", _speriod->value());
  _settings->setData("PERIOD_MED", _mperiod->value());
  _settings->setData("PERIOD_LONG", _lperiod->value());
  _settings->setData("REF1", _ref->value());
  _settings->setData("REF2", _ref2->value());
  _settings->setData("REF3", _ref3->value());
  _settings->setData("COLOR_REF1", _refColor->color().name());
  _settings->setData("COLOR_REF2", _ref2Color->color().name());
  _settings->setData("COLOR_REF3", _ref3Color->color().name());
  _settings->setData("OUTPUT", _output->text());
  _settings->setData("Z", _z->text());
}
