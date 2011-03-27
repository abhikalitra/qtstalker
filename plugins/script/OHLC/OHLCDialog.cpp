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

#include "OHLCDialog.h"
#include "Globals.h"
#include "OHLC.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>

OHLCDialog::OHLCDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "OHLCDialog_window_size";
  _keyPos = "OHLCDialog_window_position";

  _styleList << "OHLC" << "Candle";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "OHLC" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));
  
  createGeneralPage();
  createMA1Page ();
  createMA2Page ();
  createMA3Page ();

  loadSettings();
}

void OHLCDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // style
  _style = new QComboBox;
  _style->addItems(_styleList);
  _style->setCurrentIndex(_style->findText(_settings->data(OHLC::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);
  
  // up color
  _upColor = new ColorButton(this, QColor(_settings->data(OHLC::_COLOR_UP)));
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data(OHLC::_COLOR_DOWN)));
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data(OHLC::_COLOR_NEUTRAL)));
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "OHLC");
}

void OHLCDialog::createMA1Page ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

  _ma1Check = new QGroupBox;
  _ma1Check->setTitle(tr("Show"));
  _ma1Check->setCheckable(TRUE);
  _ma1Check->setChecked(_settings->getInt(OHLC::_MA1_CHECK));
  vbox->addWidget(_ma1Check);

  QFormLayout *form = new QFormLayout;
  _ma1Check->setLayout(form);

  // input
  InputType it;
  QStringList l = it.list();

  _ma1Input = new QComboBox;
  _ma1Input->addItems(l);
  _ma1Input->setCurrentIndex(_ma1Input->findText(_settings->data(OHLC::_MA1_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _ma1Input);

  // type
  MAType mat;
  l = mat.list();

  _ma1Type = new QComboBox;
  _ma1Type->addItems(l);
  _ma1Type->setCurrentIndex(_ma1Type->findText(_settings->data(OHLC::_MA1_TYPE), Qt::MatchExactly));
  form->addRow(tr("Type"), _ma1Type);

  // period
  _ma1Period = new QSpinBox;
  _ma1Period->setRange(2, 100000);
  _ma1Period->setValue(_settings->getInt(OHLC::_MA1_PERIOD));
  form->addRow(tr("Period"), _ma1Period);

  // color
  _ma1Color = new ColorButton(this, QColor(_settings->data(OHLC::_MA1_COLOR)));
  _ma1Color->setColorButton();
  form->addRow(tr("Color"), _ma1Color);

  // plot style
  Curve c;
  c.list(l, 1);

  _ma1Style = new QComboBox;
  _ma1Style->addItems(l);
  _ma1Style->setCurrentIndex(_ma1Style->findText(_settings->data(OHLC::_MA1_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _ma1Style);

  _tabs->addTab(w, "MA1");
}

void OHLCDialog::createMA2Page ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

  _ma2Check = new QGroupBox;
  _ma2Check->setTitle(tr("Show"));
  _ma2Check->setCheckable(TRUE);
  _ma2Check->setChecked(_settings->getInt(OHLC::_MA2_CHECK));
  vbox->addWidget(_ma2Check);

  QFormLayout *form = new QFormLayout;
  _ma2Check->setLayout(form);

  // input
  InputType it;
  QStringList l = it.list();

  _ma2Input = new QComboBox;
  _ma2Input->addItems(l);
  _ma2Input->setCurrentIndex(_ma2Input->findText(_settings->data(OHLC::_MA2_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _ma2Input);

  // type
  MAType mat;
  l = mat.list();

  _ma2Type = new QComboBox;
  _ma2Type->addItems(l);
  _ma2Type->setCurrentIndex(_ma2Type->findText(_settings->data(OHLC::_MA2_TYPE), Qt::MatchExactly));
  form->addRow(tr("Type"), _ma2Type);

  // period
  _ma2Period = new QSpinBox;
  _ma2Period->setRange(2, 100000);
  _ma2Period->setValue(_settings->getInt(OHLC::_MA2_PERIOD));
  form->addRow(tr("Period"), _ma2Period);

  // color
  _ma2Color = new ColorButton(this, QColor(_settings->data(OHLC::_MA2_COLOR)));
  _ma2Color->setColorButton();
  form->addRow(tr("Color"), _ma2Color);

  // plot style
  Curve c;
  c.list(l, 1);

  _ma2Style = new QComboBox;
  _ma2Style->addItems(l);
  _ma2Style->setCurrentIndex(_ma2Style->findText(_settings->data(OHLC::_MA2_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _ma2Style);

  _tabs->addTab(w, "MA2");
}

void OHLCDialog::createMA3Page ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

  _ma3Check = new QGroupBox;
  _ma3Check->setTitle(tr("Show"));
  _ma3Check->setCheckable(TRUE);
  _ma3Check->setChecked(_settings->getInt(OHLC::_MA3_CHECK));
  vbox->addWidget(_ma3Check);

  QFormLayout *form = new QFormLayout;
  _ma3Check->setLayout(form);

  // input
  InputType it;
  QStringList l = it.list();

  _ma3Input = new QComboBox;
  _ma3Input->addItems(l);
  _ma3Input->setCurrentIndex(_ma3Input->findText(_settings->data(OHLC::_MA3_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _ma3Input);

  // type
  MAType mat;
  l = mat.list();

  _ma3Type = new QComboBox;
  _ma3Type->addItems(l);
  _ma3Type->setCurrentIndex(_ma3Type->findText(_settings->data(OHLC::_MA3_TYPE), Qt::MatchExactly));
  form->addRow(tr("Type"), _ma3Type);

  // period
  _ma3Period = new QSpinBox;
  _ma3Period->setRange(2, 100000);
  _ma3Period->setValue(_settings->getInt(OHLC::_MA3_PERIOD));
  form->addRow(tr("Period"), _ma3Period);

  // color
  _ma3Color = new ColorButton(this, QColor(_settings->data(OHLC::_MA3_COLOR)));
  _ma3Color->setColorButton();
  form->addRow(tr("Color"), _ma3Color);

  // plot style
  Curve c;
  c.list(l, 1);

  _ma3Style = new QComboBox;
  _ma3Style->addItems(l);
  _ma3Style->setCurrentIndex(_ma3Style->findText(_settings->data(OHLC::_MA3_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _ma3Style);

  _tabs->addTab(w, "MA3");
}

void OHLCDialog::done ()
{
  _settings->setData(OHLC::_COLOR_UP, _upColor->color().name());
  _settings->setData(OHLC::_COLOR_DOWN, _downColor->color().name());
  _settings->setData(OHLC::_COLOR_NEUTRAL, _neutralColor->color().name());
  _settings->setData(OHLC::_STYLE, _style->currentText());
  _settings->setData(OHLC::_MA1_TYPE, _ma1Type->currentText());
  _settings->setData(OHLC::_MA1_PERIOD, _ma1Period->value());
  _settings->setData(OHLC::_MA1_COLOR, _ma1Color->color().name());
  _settings->setData(OHLC::_MA1_STYLE, _ma1Style->currentText());
  _settings->setData(OHLC::_MA1_INPUT, _ma1Input->currentText());
  _settings->setData(OHLC::_MA1_CHECK, _ma1Check->isChecked());
  _settings->setData(OHLC::_MA2_TYPE, _ma2Type->currentText());
  _settings->setData(OHLC::_MA2_PERIOD, _ma2Period->value());
  _settings->setData(OHLC::_MA2_COLOR, _ma2Color->color().name());
  _settings->setData(OHLC::_MA2_STYLE, _ma2Style->currentText());
  _settings->setData(OHLC::_MA2_INPUT, _ma2Input->currentText());
  _settings->setData(OHLC::_MA2_CHECK, _ma2Check->isChecked());
  _settings->setData(OHLC::_MA3_TYPE, _ma3Type->currentText());
  _settings->setData(OHLC::_MA3_PERIOD, _ma3Period->value());
  _settings->setData(OHLC::_MA3_COLOR, _ma3Color->color().name());
  _settings->setData(OHLC::_MA3_STYLE, _ma3Style->currentText());
  _settings->setData(OHLC::_MA3_INPUT, _ma3Input->currentText());
  _settings->setData(OHLC::_MA3_CHECK, _ma3Check->isChecked());

  saveSettings();
  
  accept();
}
