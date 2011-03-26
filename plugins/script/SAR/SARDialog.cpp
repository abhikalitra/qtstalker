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

#include "SARDialog.h"
#include "Globals.h"
#include "SAR.h"

#include <QtDebug>
#include <QStringList>

SARDialog::SARDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "SARDialog_window_size";
  _keyPos = "SARDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "SAR" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createBarsPage();

  loadSettings();
}

void SARDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _color = new ColorButton(this, QColor(_settings->data(SAR::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // initial
  _init = new QDoubleSpinBox;
  _init->setRange(0, 0.2);
  _init->setValue(_settings->getDouble(SAR::_STEP_INITIAL));
  form->addRow(tr("Initial Step"), _init);

  // max
  _max = new QDoubleSpinBox;
  _max->setRange(0, 0.2);
  _max->setValue(_settings->getDouble(SAR::_STEP_MAX));
  form->addRow(tr("Maximum Step"), _max);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "SAR");
}

void SARDialog::createBarsPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // up color
  _upColor = new ColorButton(this, QColor(_settings->data(SAR::_COLOR_BARS_UP)));
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data(SAR::_COLOR_BARS_DOWN)));
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data(SAR::_COLOR_BARS_NEUTRAL)));
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // style
  QStringList l;
  l << "OHLC" << "Candle";

  _barsStyle = new QComboBox;
  _barsStyle->addItems(l);
  _barsStyle->setCurrentIndex(_barsStyle->findText(_settings->data(SAR::_STYLE_BARS), Qt::MatchExactly));
  form->addRow(tr("Style"), _barsStyle);

  _tabs->addTab(w, "BARS");
}

void SARDialog::done ()
{
  _settings->setData(SAR::_COLOR, _color->color().name());
  _settings->setData(SAR::_STEP_INITIAL, _init->value());
  _settings->setData(SAR::_STEP_MAX, _max->value());
  _settings->setData(SAR::_COLOR_BARS_UP, _upColor->color().name());
  _settings->setData(SAR::_COLOR_BARS_DOWN, _downColor->color().name());
  _settings->setData(SAR::_COLOR_BARS_NEUTRAL, _neutralColor->color().name());
  _settings->setData(SAR::_STYLE_BARS, _barsStyle->currentText());

  saveSettings();

  accept();
}
