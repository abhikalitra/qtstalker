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

#include "ADXDialog.h"
#include "ADX.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>

ADXDialog::ADXDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "ADXDialog_window_size";
  _keyPos = "ADXDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "ADX" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createMDIPage();
  createPDIPage();
  createADXPage();
  createADXRPage();

  loadSettings();
}

void ADXDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);
  
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings->getInt(ADX::_PERIOD));
  form->addRow(tr("Period"), _period);
  
  // make room unused
  _message->hide();
  
  _tabs->addTab(w, tr("General"));  
}

void ADXDialog::createMDIPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _mdiColor = new ColorButton(this, QColor(_settings->data(ADX::_MDI_COLOR)));
  _mdiColor->setColorButton();
  form->addRow(tr("Color"), _mdiColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _mdiStyle = new QComboBox;
  _mdiStyle->addItems(l);
  _mdiStyle->setCurrentIndex(_mdiStyle->findText(_settings->data(ADX::_MDI_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _mdiStyle);

  _tabs->addTab(w, "-DI");
}

void ADXDialog::createPDIPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _pdiColor = new ColorButton(this, QColor(_settings->data(ADX::_PDI_COLOR)));
  _pdiColor->setColorButton();
  form->addRow(tr("Color"), _pdiColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _pdiStyle = new QComboBox;
  _pdiStyle->addItems(l);
  _pdiStyle->setCurrentIndex(_pdiStyle->findText(_settings->data(ADX::_PDI_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _pdiStyle);

  _tabs->addTab(w, "+DI");
}

void ADXDialog::createADXPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _adxColor = new ColorButton(this, QColor(_settings->data(ADX::_ADX_COLOR)));
  _adxColor->setColorButton();
  form->addRow(tr("Color"), _adxColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _adxStyle = new QComboBox;
  _adxStyle->addItems(l);
  _adxStyle->setCurrentIndex(_adxStyle->findText(_settings->data(ADX::_ADX_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _adxStyle);

  _tabs->addTab(w, "ADX");
}

void ADXDialog::createADXRPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _adxrColor = new ColorButton(this, QColor(_settings->data(ADX::_ADXR_COLOR)));
  _adxrColor->setColorButton();
  form->addRow(tr("Color"), _adxrColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _adxrStyle = new QComboBox;
  _adxrStyle->addItems(l);
  _adxrStyle->setCurrentIndex(_adxrStyle->findText(_settings->data(ADX::_ADXR_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _adxrStyle);

  _tabs->addTab(w, "ADXR");
}

void ADXDialog::done ()
{
  _settings->setData(ADX::_ADX_COLOR, _adxColor->color().name());
  _settings->setData(ADX::_ADXR_COLOR, _adxrColor->color().name());
  _settings->setData(ADX::_PDI_COLOR, _pdiColor->color().name());
  _settings->setData(ADX::_MDI_COLOR, _mdiColor->color().name());
  _settings->setData(ADX::_ADX_STYLE, _adxStyle->currentText());
  _settings->setData(ADX::_ADXR_STYLE, _adxrStyle->currentText());
  _settings->setData(ADX::_PDI_STYLE, _pdiStyle->currentText());
  _settings->setData(ADX::_MDI_STYLE, _mdiStyle->currentText());
  _settings->setData(ADX::_PERIOD, (int) _period->value());

  saveSettings();
  
  accept();
}
