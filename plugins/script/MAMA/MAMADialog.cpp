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

#include "MAMADialog.h"
#include "Globals.h"
#include "MAMA.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>

MAMADialog::MAMADialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "MAMADialog_window_size";
  _keyPos = "MAMADialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "MAMA" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createMAMAPage();
  createFAMAPage();
  createBarsPage();

  loadSettings();
}

void MAMADialog::createGeneralPage ()
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
  _input->setCurrentIndex(_input->findText(_settings->data(MAMA::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // fast limit
  _fast = new QDoubleSpinBox;
  _fast->setRange(0, 100000);
  _fast->setValue(_settings->getInt(MAMA::_LIMIT_FAST));
  form->addRow(tr("Fast Limit"), _fast);

  // slow limit
  _slow = new QDoubleSpinBox;
  _slow->setRange(0, 100000);
  _slow->setValue(_settings->getInt(MAMA::_LIMIT_SLOW));
  form->addRow(tr("Slow Period"), _slow);

  _tabs->addTab(w, tr("General"));  
}

void MAMADialog::createMAMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _mamaColor = new ColorButton(this, QColor(_settings->data(MAMA::_COLOR_MAMA)));
  _mamaColor->setColorButton();
  form->addRow(tr("Color"), _mamaColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _mamaStyle = new QComboBox;
  _mamaStyle->addItems(l);
  _mamaStyle->setCurrentIndex(_mamaStyle->findText(_settings->data(MAMA::_STYLE_MAMA), Qt::MatchExactly));
  form->addRow(tr("Style"), _mamaStyle);

  _tabs->addTab(w, "MAMA");
}

void MAMADialog::createFAMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _famaColor = new ColorButton(this, QColor(_settings->data(MAMA::_COLOR_FAMA)));
  _famaColor->setColorButton();
  form->addRow(tr("Color"), _famaColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _famaStyle = new QComboBox;
  _famaStyle->addItems(l);
  _famaStyle->setCurrentIndex(_famaStyle->findText(_settings->data(MAMA::_STYLE_FAMA), Qt::MatchExactly));
  form->addRow(tr("Style"), _famaStyle);

  _tabs->addTab(w, "FAMA");
}

void MAMADialog::createBarsPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // up color
  _upColor = new ColorButton(this, QColor(_settings->data(MAMA::_COLOR_BARS_UP)));
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data(MAMA::_COLOR_BARS_DOWN)));
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data(MAMA::_COLOR_BARS_NEUTRAL)));
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // style
  QStringList l;
  l << "OHLC" << "Candle";

  _barsStyle = new QComboBox;
  _barsStyle->addItems(l);
  _barsStyle->setCurrentIndex(_barsStyle->findText(_settings->data(MAMA::_STYLE_BARS), Qt::MatchExactly));
  form->addRow(tr("Style"), _barsStyle);

  _tabs->addTab(w, "BARS");
}

void MAMADialog::done ()
{
  _settings->setData(MAMA::_COLOR_MAMA, _mamaColor->color().name());
  _settings->setData(MAMA::_COLOR_FAMA, _famaColor->color().name());
  _settings->setData(MAMA::_STYLE_MAMA, _mamaStyle->currentText());
  _settings->setData(MAMA::_STYLE_FAMA, _famaStyle->currentText());
  _settings->setData(MAMA::_LIMIT_FAST, _fast->value());
  _settings->setData(MAMA::_LIMIT_SLOW, _slow->value());
  _settings->setData(MAMA::_INPUT, _input->currentText());
  _settings->setData(MAMA::_COLOR_BARS_UP, _upColor->color().name());
  _settings->setData(MAMA::_COLOR_BARS_DOWN, _downColor->color().name());
  _settings->setData(MAMA::_COLOR_BARS_NEUTRAL, _neutralColor->color().name());
  _settings->setData(MAMA::_STYLE_BARS, _barsStyle->currentText());

  saveSettings();

  accept();
}
