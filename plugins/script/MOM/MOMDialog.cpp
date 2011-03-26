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

#include "MOMDialog.h"
#include "Globals.h"
#include "InputType.h"
#include "MOM.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>

MOMDialog::MOMDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "MOMDialog_window_size";
  _keyPos = "MOMDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "MOM" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createMAPage();

  loadSettings();
}

void MOMDialog::createGeneralPage ()
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
  _input->setCurrentIndex(_input->findText(_settings->data(MOM::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(MOM::_PERIOD));
  form->addRow(tr("Period"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(MOM::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(MOM::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "MOM");
}

void MOMDialog::createMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // type
  MAType mat;
  QStringList l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data(MOM::_MA_TYPE), Qt::MatchExactly));
  form->addRow(tr("Type"), _maType);

  // period
  _maPeriod = new QSpinBox;
  _maPeriod->setRange(1, 100000);
  _maPeriod->setValue(_settings->getInt(MOM::_MA_PERIOD));
  form->addRow(tr("Period"), _maPeriod);

  // color
  _maColor = new ColorButton(this, QColor(_settings->data(MOM::_MA_COLOR)));
  _maColor->setColorButton();
  form->addRow(tr("Color"), _maColor);

  // plot style
  Curve c;
  c.list(l, 1);

  _maStyle = new QComboBox;
  _maStyle->addItems(l);
  _maStyle->setCurrentIndex(_maStyle->findText(_settings->data(MOM::_MA_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _maStyle);

  _tabs->addTab(w, "MA");
}

void MOMDialog::done ()
{
  _settings->setData(MOM::_INPUT, _input->currentText());
  _settings->setData(MOM::_COLOR, _color->color().name());
  _settings->setData(MOM::_STYLE, _style->currentText());
  _settings->setData(MOM::_PERIOD, _period->value());
  _settings->setData(MOM::_MA_TYPE, _maType->currentText());
  _settings->setData(MOM::_MA_PERIOD, _maPeriod->value());
  _settings->setData(MOM::_MA_COLOR, _maColor->color().name());
  _settings->setData(MOM::_MA_STYLE, _maStyle->currentText());

  saveSettings();

  accept();
}
