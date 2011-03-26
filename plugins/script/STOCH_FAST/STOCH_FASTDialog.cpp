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

#include "STOCH_FASTDialog.h"
#include "Globals.h"
#include "MAType.h"
#include "STOCH_FAST.h"

#include <QtDebug>
#include <QStringList>

STOCH_FASTDialog::STOCH_FASTDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "STOCH_FASTDialog_window_size";
  _keyPos = "STOCH_FASTDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "STOCH_FAST" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createKPage();
  createDPage();
  createRefPage();
  createRef2Page();

  loadSettings();
}

void STOCH_FASTDialog::createKPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // period
  _kperiod = new QSpinBox;
  _kperiod->setRange(1, 100000);
  _kperiod->setValue(_settings->getInt(STOCH_FAST::_PERIOD_FASTK));
  form->addRow(tr("Period"), _kperiod);

  // color
  _kcolor = new ColorButton(this, QColor(_settings->data(STOCH_FAST::_COLOR_K)));
  _kcolor->setColorButton();
  form->addRow(tr("Color"), _kcolor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _kstyle = new QComboBox;
  _kstyle->addItems(l);
  _kstyle->setCurrentIndex(_kstyle->findText(_settings->data(STOCH_FAST::_STYLE_K), Qt::MatchExactly));
  form->addRow(tr("Style"), _kstyle);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "%K");
}

void STOCH_FASTDialog::createDPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // type
  MAType mat;
  QStringList l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data(STOCH_FAST::_MA_TYPE_FASTD), Qt::MatchExactly));
  form->addRow(tr("Type"), _maType);

  // period
  _dperiod = new QSpinBox;
  _dperiod->setRange(1, 100000);
  _dperiod->setValue(_settings->getInt(STOCH_FAST::_PERIOD_FASTD));
  form->addRow(tr("Period"), _dperiod);

  // color
  _dcolor = new ColorButton(this, QColor(_settings->data(STOCH_FAST::_COLOR_D)));
  _dcolor->setColorButton();
  form->addRow(tr("Color"), _dcolor);

  // plot style
  Curve c;
  c.list(l, 1);

  _dstyle = new QComboBox;
  _dstyle->addItems(l);
  _dstyle->setCurrentIndex(_dstyle->findText(_settings->data(STOCH_FAST::_STYLE_D), Qt::MatchExactly));
  form->addRow(tr("Style"), _dstyle);

  _tabs->addTab(w, "%D");
}

void STOCH_FASTDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data(STOCH_FAST::_COLOR_REF1)));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings->getDouble(STOCH_FAST::_REF1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void STOCH_FASTDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data(STOCH_FAST::_COLOR_REF2)));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings->getDouble(STOCH_FAST::_REF2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void STOCH_FASTDialog::done ()
{
  _settings->setData(STOCH_FAST::_COLOR_K, _kcolor->color().name());
  _settings->setData(STOCH_FAST::_COLOR_D, _dcolor->color().name());
  _settings->setData(STOCH_FAST::_STYLE_K, _kstyle->currentText());
  _settings->setData(STOCH_FAST::_STYLE_D, _dstyle->currentText());
  _settings->setData(STOCH_FAST::_PERIOD_FASTD, _dperiod->value());
  _settings->setData(STOCH_FAST::_PERIOD_FASTK, _kperiod->value());
  _settings->setData(STOCH_FAST::_MA_TYPE_FASTD, _maType->currentText());
  _settings->setData(STOCH_FAST::_REF1, _ref->value());
  _settings->setData(STOCH_FAST::_REF2, _ref2->value());
  _settings->setData(STOCH_FAST::_COLOR_REF1, _refColor->color().name());
  _settings->setData(STOCH_FAST::_COLOR_REF2, _ref2Color->color().name());

  saveSettings();

  accept();
}
