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

#include "BBANDSDialog.h"
#include "BBANDS.h"
#include "Globals.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>

BBANDSDialog::BBANDSDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "BBANDSDialog_window_size";
  _keyPos = "BBANDSDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "BBANDS" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createBarsPage();
  createUpperPage();
  createMiddlePage();
  createLowerPage();

  loadSettings();
}

void BBANDSDialog::createGeneralPage ()
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
  _input->setCurrentIndex(_input->findText(_settings->data(BBANDS::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings->getInt(BBANDS::_PERIOD));
  form->addRow(tr("Period"), _period);

  // up deviation
  _upDev = new QDoubleSpinBox;
  _upDev->setRange(-100000, 100000);
  _upDev->setValue(_settings->getDouble(BBANDS::_DEVIATION_UP));
  form->addRow(tr("Upper Deviation"), _upDev);

  // low deviation
  _lowDev = new QDoubleSpinBox;
  _lowDev->setRange(-100000, 100000);
  _lowDev->setValue(_settings->getDouble(BBANDS::_DEVIATION_DOWN));
  form->addRow(tr("Lower Deviation"), _lowDev);

  // ma type
  MAType mat;
  l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data(BBANDS::_MA_TYPE), Qt::MatchExactly));
  form->addRow(tr("MA Type"), _maType);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "BBANDS");
}

void BBANDSDialog::createUpperPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _upperColor = new ColorButton(this, QColor(_settings->data(BBANDS::_COLOR_UP)));
  _upperColor->setColorButton();
  form->addRow(tr("Color"), _upperColor);

  // plot style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _upperStyle = new QComboBox;
  _upperStyle->addItems(l);
  _upperStyle->setCurrentIndex(_upperStyle->findText(_settings->data(BBANDS::_STYLE_UP), Qt::MatchExactly));
  form->addRow(tr("Style"), _upperStyle);

  _tabs->addTab(w, tr("Upper"));
}

void BBANDSDialog::createMiddlePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _midColor = new ColorButton(this, QColor(_settings->data(BBANDS::_COLOR_MID)));
  _midColor->setColorButton();
  form->addRow(tr("Color"), _midColor);

  // plot style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _midStyle = new QComboBox;
  _midStyle->addItems(l);
  _midStyle->setCurrentIndex(_midStyle->findText(_settings->data(BBANDS::_STYLE_MID), Qt::MatchExactly));
  form->addRow(tr("Style"), _midStyle);

  _tabs->addTab(w, tr("Middle"));
}

void BBANDSDialog::createLowerPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _lowerColor = new ColorButton(this, QColor(_settings->data(BBANDS::_COLOR_DOWN)));
  _lowerColor->setColorButton();
  form->addRow(tr("Color"), _lowerColor);

  // plot style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _lowerStyle = new QComboBox;
  _lowerStyle->addItems(l);
  _lowerStyle->setCurrentIndex(_lowerStyle->findText(_settings->data(BBANDS::_STYLE_DOWN), Qt::MatchExactly));
  form->addRow(tr("Style"), _lowerStyle);

  _tabs->addTab(w, tr("Lower"));
}

void BBANDSDialog::createBarsPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // up color
  _upColor = new ColorButton(this, QColor(_settings->data(BBANDS::_COLOR_BARS_UP)));
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data(BBANDS::_COLOR_BARS_DOWN)));
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data(BBANDS::_COLOR_BARS_NEUTRAL)));
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // style
  QStringList l;
  l << "OHLC" << "Candle";
  
  _barsStyle = new QComboBox;
  _barsStyle->addItems(l);
  _barsStyle->setCurrentIndex(_barsStyle->findText(_settings->data(BBANDS::_STYLE_BARS), Qt::MatchExactly));
  form->addRow(tr("Style"), _barsStyle);

  _tabs->addTab(w, "BARS");
}

void BBANDSDialog::done ()
{
  _settings->setData(BBANDS::_COLOR_UP, _upperColor->color().name());
  _settings->setData(BBANDS::_COLOR_MID, _midColor->color().name());
  _settings->setData(BBANDS::_COLOR_DOWN, _lowerColor->color().name());
  _settings->setData(BBANDS::_COLOR_BARS_UP, _upColor->color().name());
  _settings->setData(BBANDS::_COLOR_BARS_DOWN, _downColor->color().name());
  _settings->setData(BBANDS::_COLOR_BARS_NEUTRAL, _neutralColor->color().name());
  _settings->setData(BBANDS::_STYLE_UP, _upperStyle->currentText());
  _settings->setData(BBANDS::_STYLE_MID, _midStyle->currentText());
  _settings->setData(BBANDS::_STYLE_DOWN, _lowerStyle->currentText());
  _settings->setData(BBANDS::_STYLE_BARS, _barsStyle->currentText());
  _settings->setData(BBANDS::_DEVIATION_UP, _upDev->value());
  _settings->setData(BBANDS::_DEVIATION_DOWN, _lowDev->value());
  _settings->setData(BBANDS::_INPUT, _input->currentText());
  _settings->setData(BBANDS::_PERIOD, _period->value());
  _settings->setData(BBANDS::_MA_TYPE, _maType->currentText());

  saveSettings();

  accept();
}
