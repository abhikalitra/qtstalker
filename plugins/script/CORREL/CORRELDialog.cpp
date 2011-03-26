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

#include "CORRELDialog.h"
#include "CORREL.h"
#include "InputType.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>

CORRELDialog::CORRELDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "CORRELDialog_window_size";
  _keyPos = "CORRELDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "CORREL" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));
  
  createGeneralPage();
  createRefPage();
  createRef2Page();
  createRef3Page();
  
  loadSettings();
}

void CORRELDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // index
  _index = new QLineEdit(_settings->data(CORREL::_INDEX));
  _index->setToolTip(tr("Index symbol used for comparison eg. SP500"));
  form->addRow(tr("Index Symbol"), _index);

  // input
  InputType it;
  QStringList l = it.list();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data(CORREL::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings->getInt(CORREL::_PERIOD));
  form->addRow(tr("Period"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(CORREL::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(CORREL::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "CORREL");
}

void CORRELDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data(CORREL::_COLOR_REF1)));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings->getDouble(CORREL::_REF1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void CORRELDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data(CORREL::_COLOR_REF2)));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings->getDouble(CORREL::_REF2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void CORRELDialog::createRef3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref3Color = new ColorButton(this, QColor(_settings->data(CORREL::_COLOR_REF3)));
  _ref3Color->setColorButton();
  form->addRow(tr("Color"), _ref3Color);

  // ref
  _ref3 = new QDoubleSpinBox;
  _ref3->setRange(-100000, 100000);
  _ref3->setValue(_settings->getDouble(CORREL::_REF3));
  form->addRow(tr("Value"), _ref3);

  _tabs->addTab(w, "REF 3");
}

void CORRELDialog::done ()
{
  _settings->setData(CORREL::_INDEX, _index->text());
  _settings->setData(CORREL::_COLOR, _color->color().name());
  _settings->setData(CORREL::_STYLE, _style->currentText());
  _settings->setData(CORREL::_INPUT, _input->currentText());
  _settings->setData(CORREL::_PERIOD, _period->value());
  _settings->setData(CORREL::_REF1, _ref->value());
  _settings->setData(CORREL::_REF2, _ref2->value());
  _settings->setData(CORREL::_REF3, _ref3->value());
  _settings->setData(CORREL::_COLOR_REF1, _refColor->color().name());
  _settings->setData(CORREL::_COLOR_REF2, _ref2Color->color().name());
  _settings->setData(CORREL::_COLOR_REF3, _ref3Color->color().name());

  saveSettings();

  accept();
}
