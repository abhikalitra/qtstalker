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

#include "AROONDialog.h"
#include "AROON.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>

AROONDialog::AROONDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "AROONDialog_window_size";
  _keyPos = "AROONDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "AROON" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createAROONUpPage();
  createAROONDownPage();

  loadSettings();
}

void AROONDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings->getInt(AROON::_PERIOD));
  form->addRow(tr("Period"), _period);

  // make room unused
  _message->hide();
  
  _tabs->addTab(w, tr("General"));  
}

void AROONDialog::createAROONUpPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _upColor = new ColorButton(this, QColor(_settings->data(AROON::_COLOR_UP)));
  _upColor->setColorButton();
  form->addRow(tr("Color"), _upColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _upStyle = new QComboBox;
  _upStyle->addItems(l);
  _upStyle->setCurrentIndex(_upStyle->findText(_settings->data(AROON::_STYLE_UP), Qt::MatchExactly));
  form->addRow(tr("Style"), _upStyle);

  _tabs->addTab(w, tr("Up"));
}

void AROONDialog::createAROONDownPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _downColor = new ColorButton(this, QColor(_settings->data(AROON::_COLOR_DOWN)));
  _downColor->setColorButton();
  form->addRow(tr("Color"), _downColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _downStyle = new QComboBox;
  _downStyle->addItems(l);
  _downStyle->setCurrentIndex(_downStyle->findText(_settings->data(AROON::_STYLE_DOWN), Qt::MatchExactly));
  form->addRow(tr("Style"), _downStyle);

  _tabs->addTab(w, tr("Down"));
}

void AROONDialog::done ()
{
  _settings->setData(AROON::_COLOR_DOWN, _downColor->color().name());
  _settings->setData(AROON::_COLOR_UP, _upColor->color().name());
  _settings->setData(AROON::_STYLE_DOWN, _downStyle->currentText());
  _settings->setData(AROON::_STYLE_UP, _upStyle->currentText());
  _settings->setData(AROON::_PERIOD, _period->value());

  saveSettings();
  
  accept();
}

