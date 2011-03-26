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

#include "CMODialog.h"
#include "MAType.h"
#include "InputType.h"
#include "CMO.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>

CMODialog::CMODialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "CMODialog_window_size";
  _keyPos = "CMODialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "CMO" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createMAPage();
  createRefPage();
  createRef2Page();

  loadSettings();
}

void CMODialog::createGeneralPage ()
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
  _input->setCurrentIndex(_input->findText(_settings->data(CMO::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(CMO::_PERIOD));
  form->addRow(tr("Period"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(CMO::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(CMO::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "CMO");
}

void CMODialog::createMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // type
  MAType mat;
  QStringList l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data(CMO::_MA_TYPE), Qt::MatchExactly));
  form->addRow(tr("Type"), _maType);

  // period
  _maPeriod = new QSpinBox;
  _maPeriod->setRange(1, 100000);
  _maPeriod->setValue(_settings->getInt(CMO::_MA_PERIOD));
  form->addRow(tr("Period"), _maPeriod);

  // color
  _maColor = new ColorButton(this, QColor(_settings->data(CMO::_MA_COLOR)));
  _maColor->setColorButton();
  form->addRow(tr("Color"), _maColor);

  // plot style
  Curve c;
  c.list(l, 1);

  _maStyle = new QComboBox;
  _maStyle->addItems(l);
  _maStyle->setCurrentIndex(_maStyle->findText(_settings->data(CMO::_MA_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _maStyle);

  _tabs->addTab(w, "MA");
}

void CMODialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data(CMO::_COLOR_REF1)));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings->getDouble(CMO::_REF1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void CMODialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data(CMO::_COLOR_REF2)));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings->getDouble(CMO::_REF2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void CMODialog::done ()
{
  _settings->setData(CMO::_INPUT, _input->currentText());
  _settings->setData(CMO::_PERIOD, _period->value());
  _settings->setData(CMO::_COLOR, _color->color().name());
  _settings->setData(CMO::_STYLE, _style->currentText());
  _settings->setData(CMO::_MA_TYPE, _maType->currentText());
  _settings->setData(CMO::_MA_PERIOD, _maPeriod->value());
  _settings->setData(CMO::_MA_COLOR, _maColor->color().name());
  _settings->setData(CMO::_MA_STYLE, _maStyle->currentText());
  _settings->setData(CMO::_REF1, _ref->value());
  _settings->setData(CMO::_REF2, _ref2->value());
  _settings->setData(CMO::_COLOR_REF1, _refColor->color().name());
  _settings->setData(CMO::_COLOR_REF2, _ref2Color->color().name());

  saveSettings();

  accept();
}
