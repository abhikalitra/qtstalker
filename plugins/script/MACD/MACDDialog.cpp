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

#include "MACDDialog.h"
#include "Globals.h"
#include "InputType.h"
#include "MACD.h"
#include "MAType.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

MACDDialog::MACDDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "MACDDialog_window_size";
  _keyPos = "MACDDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "MACD" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createMACDPage();
  createSignalPage();
  createHistPage();

  loadSettings();
}

void MACDDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // input
  InputType it;
  QStringList l;
  l = it.list();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data(MACD::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // fast ma type
  MAType mat;
  l = mat.list();

  _fastMAType = new QComboBox;
  _fastMAType->addItems(l);
  _fastMAType->setCurrentIndex(_fastMAType->findText(_settings->data(MACD::_MA_TYPE_FAST), Qt::MatchExactly));
  form->addRow(tr("Fast MA"), _fastMAType);

  // slow ma type
  _slowMAType = new QComboBox;
  _slowMAType->addItems(l);
  _slowMAType->setCurrentIndex(_slowMAType->findText(_settings->data(MACD::_MA_TYPE_SLOW), Qt::MatchExactly));
  form->addRow(tr("Slow MA"), _slowMAType);

  // fast period
  _fastPeriod = new QSpinBox;
  _fastPeriod->setRange(1, 100000);
  _fastPeriod->setValue(_settings->getInt(MACD::_PERIOD_FAST));
  form->addRow(tr("Fast Period"), _fastPeriod);

  // slow period
  _slowPeriod = new QSpinBox;
  _slowPeriod->setRange(1, 100000);
  _slowPeriod->setValue(_settings->getInt(MACD::_PERIOD_SLOW));
  form->addRow(tr("Slow Period"), _slowPeriod);

  // make room unused
  _message->hide();

  _tabs->addTab(w, tr("General"));
}

void MACDDialog::createMACDPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _macdColor = new ColorButton(this, QColor(_settings->data(MACD::_COLOR_MACD)));
  _macdColor->setColorButton();
  form->addRow(tr("Color"), _macdColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _macdStyle = new QComboBox;
  _macdStyle->addItems(l);
  _macdStyle->setCurrentIndex(_macdStyle->findText(_settings->data(MACD::_STYLE_MACD), Qt::MatchExactly));
  form->addRow(tr("Style"), _macdStyle);

  _tabs->addTab(w, "MACD");
}

void MACDDialog::createSignalPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // ma type
  MAType mat;
  QStringList l = mat.list();

  _signalMAType = new QComboBox;
  _signalMAType->addItems(l);
  _signalMAType->setCurrentIndex(_signalMAType->findText(_settings->data(MACD::_MA_TYPE_SIG), Qt::MatchExactly));
  form->addRow(tr("Type"), _signalMAType);

  // period
  _signalPeriod = new QSpinBox;
  _signalPeriod->setRange(2, 100000);
  _signalPeriod->setValue(_settings->getInt(MACD::_PERIOD_SIG));
  form->addRow(tr("Period"), _signalPeriod);

  // color
  _signalColor = new ColorButton(this, QColor(_settings->data(MACD::_COLOR_SIG)));
  _signalColor->setColorButton();
  form->addRow(tr("Color"), _signalColor);

  // plot style
  Curve c;
  c.list(l, 1);

  _signalStyle = new QComboBox;
  _signalStyle->addItems(l);
  _signalStyle->setCurrentIndex(_signalStyle->findText(_settings->data(MACD::_STYLE_SIG), Qt::MatchExactly));
  form->addRow(tr("Style"), _signalStyle);

  _tabs->addTab(w, "SIG");
}

void MACDDialog::createHistPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _histColor = new ColorButton(this, QColor(_settings->data(MACD::_COLOR_HIST)));
  _histColor->setColorButton();
  form->addRow(tr("Color"), _histColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _histStyle = new QComboBox;
  _histStyle->addItems(l);
  _histStyle->setCurrentIndex(_histStyle->findText(_settings->data(MACD::_STYLE_HIST), Qt::MatchExactly));
  form->addRow(tr("Style"), _histStyle);

  _tabs->addTab(w, "HIST");
}

void MACDDialog::done ()
{
  _settings->setData(MACD::_COLOR_MACD, _macdColor->color().name());
  _settings->setData(MACD::_COLOR_SIG, _signalColor->color().name());
  _settings->setData(MACD::_COLOR_HIST, _histColor->color().name());
  _settings->setData(MACD::_STYLE_MACD, _macdStyle->currentText());
  _settings->setData(MACD::_STYLE_SIG, _signalStyle->currentText());
  _settings->setData(MACD::_STYLE_HIST, _histStyle->currentText());
  _settings->setData(MACD::_PERIOD_FAST, _fastPeriod->value());
  _settings->setData(MACD::_PERIOD_SLOW, _slowPeriod->value());
  _settings->setData(MACD::_PERIOD_SIG, _signalPeriod->value());
  _settings->setData(MACD::_MA_TYPE_FAST, _fastMAType->currentText());
  _settings->setData(MACD::_MA_TYPE_SLOW, _slowMAType->currentText());
  _settings->setData(MACD::_MA_TYPE_SIG, _signalMAType->currentText());
  _settings->setData(MACD::_INPUT, _input->currentText());

  saveSettings();

  accept();
}
