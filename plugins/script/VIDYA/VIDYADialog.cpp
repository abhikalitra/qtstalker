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

#include "VIDYADialog.h"
#include "Globals.h"
#include "VIDYA.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>

VIDYADialog::VIDYADialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "VIDYADialog_window_size";
  _keyPos = "VIDYADialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "VIDYA" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createBarsPage();

  loadSettings();
}

void VIDYADialog::createGeneralPage ()
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
  _input->setCurrentIndex(_input->findText(_settings->data(VIDYA::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(VIDYA::_PERIOD));
  form->addRow(tr("Period"), _period);

  // vperiod
  _vperiod = new QSpinBox;
  _vperiod->setRange(1, 100000);
  _vperiod->setValue(_settings->getInt(VIDYA::_VPERIOD));
  form->addRow(tr("Volume Period"), _vperiod);

  // color
  _color = new ColorButton(this, QColor(_settings->data(VIDYA::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(VIDYA::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "VIDYA");
}

void VIDYADialog::createBarsPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // up color
  _upColor = new ColorButton(this, QColor(_settings->data(VIDYA::_COLOR_BARS_UP)));
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data(VIDYA::_COLOR_BARS_DOWN)));
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data(VIDYA::_COLOR_BARS_NEUTRAL)));
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // style
  QStringList l;
  l << "OHLC" << "Candle";

  _barsStyle = new QComboBox;
  _barsStyle->addItems(l);
  _barsStyle->setCurrentIndex(_barsStyle->findText(_settings->data(VIDYA::_STYLE_BARS), Qt::MatchExactly));
  form->addRow(tr("Style"), _barsStyle);

  _tabs->addTab(w, "BARS");
}

void VIDYADialog::done ()
{
  _settings->setData(VIDYA::_INPUT, _input->currentText());
  _settings->setData(VIDYA::_COLOR, _color->color().name());
  _settings->setData(VIDYA::_STYLE, _style->currentText());
  _settings->setData(VIDYA::_PERIOD, _period->value());
  _settings->setData(VIDYA::_VPERIOD, _vperiod->value());
  _settings->setData(VIDYA::_COLOR_BARS_UP, _upColor->color().name());
  _settings->setData(VIDYA::_COLOR_BARS_DOWN, _downColor->color().name());
  _settings->setData(VIDYA::_COLOR_BARS_NEUTRAL, _neutralColor->color().name());
  _settings->setData(VIDYA::_STYLE_BARS, _barsStyle->currentText());

  saveSettings();

  accept();
}
