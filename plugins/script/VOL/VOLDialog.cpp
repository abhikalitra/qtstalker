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

#include "VOLDialog.h"
#include "Curve.h"
#include "Globals.h"
#include "MAType.h"
#include "VOL.h"

#include <QtDebug>
#include <QStringList>

VOLDialog::VOLDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "VOLDialog_window_size";
  _keyPos = "VOLDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "VOL" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createMAPage();

  loadSettings();
}

void VOLDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);
  
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // up color
  _upColor = new ColorButton(this, QColor(_settings->data(VOL::_COLOR_UP)));
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data(VOL::_COLOR_DOWN)));
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data(VOL::_COLOR_NEUTRAL)));
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // label
  _label = new QLineEdit(_settings->data(VOL::_LABEL));
  form->addRow(tr("Label"), _label);

  // make room unused
  _message->hide();

  _tabs->addTab(w, tr("General"));
}

void VOLDialog::createMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _maColor = new ColorButton(this, QColor(_settings->data(VOL::_MA_COLOR)));
  _maColor->setColorButton();
  form->addRow(tr("Color"), _maColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _maStyle = new QComboBox;
  _maStyle->addItems(l);
  _maStyle->setCurrentIndex(_maStyle->findText(_settings->data(VOL::_MA_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _maStyle);

  // label
  _maLabel = new QLineEdit(_settings->data(VOL::_MA_LABEL));
  form->addRow(tr("Label"), _maLabel);

  // period
  _maPeriod = new QSpinBox;
  _maPeriod->setRange(2, 100000);
  _maPeriod->setValue(_settings->getInt(VOL::_MA_PERIOD));
  form->addRow(tr("Period"), _maPeriod);

  // ma type
  MAType mat;
  l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data(VOL::_MA_TYPE), Qt::MatchExactly));
  form->addRow(tr("Type"), _maType);

  _tabs->addTab(w, tr("MA"));
}

void VOLDialog::done ()
{
  _settings->setData(VOL::_COLOR_UP, _upColor->color().name());
  _settings->setData(VOL::_COLOR_DOWN, _downColor->color().name());
  _settings->setData(VOL::_COLOR_NEUTRAL, _neutralColor->color().name());
  _settings->setData(VOL::_LABEL, _label->text());
  _settings->setData(VOL::_MA_COLOR, _maColor->color().name());
  _settings->setData(VOL::_MA_STYLE, _maStyle->currentText());
  _settings->setData(VOL::_MA_LABEL, _maLabel->text());
  _settings->setData(VOL::_MA_PERIOD, _maPeriod->value());
  _settings->setData(VOL::_MA_TYPE, _maType->currentText());

  saveSettings();

  accept();
}
