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

#include "CCIDialog.h"
#include "MAType.h"
#include "CCI.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>

CCIDialog::CCIDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "CCIDialog_window_size";
  _keyPos = "CCIDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "CCI" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createRefPage();
  createRef2Page();

  loadSettings();
}

void CCIDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(CCI::_PERIOD));
  form->addRow(tr("Period"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(CCI::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(CCI::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings->getInt(CCI::_SMOOTHING));
  form->addRow(tr("Smoothing"), _smoothing);

  // smoothing type
  MAType mat;
  l = mat.list();

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(_settings->data(CCI::_SMOOTHING_TYPE), Qt::MatchExactly));
  form->addRow(tr("Smoothing Type"), _smoothingType);

  // make room unused
  _message->hide();

  _tabs->addTab(w, tr("General"));
}

void CCIDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data(CCI::_COLOR_REF1)));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings->getDouble(CCI::_REF1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void CCIDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data(CCI::_COLOR_REF2)));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings->getDouble(CCI::_REF2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void CCIDialog::done ()
{
  _settings->setData(CCI::_COLOR, _color->color().name());
  _settings->setData(CCI::_STYLE, _style->currentText());
  _settings->setData(CCI::_SMOOTHING, _smoothing->value());
  _settings->setData(CCI::_SMOOTHING_TYPE, _smoothingType->currentText());
  _settings->setData(CCI::_PERIOD, _period->value());
  _settings->setData(CCI::_REF1, _ref->value());
  _settings->setData(CCI::_REF2, _ref2->value());
  _settings->setData(CCI::_COLOR_REF1, _refColor->color().name());
  _settings->setData(CCI::_COLOR_REF2, _ref2Color->color().name());

  saveSettings();

  accept();
}
