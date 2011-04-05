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

#include "MACDDialog.h"
#include "Globals.h"
#include "InputType.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

MACDDialog::MACDDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createMACDPage();
  createSignalPage();
  createHistPage();
}

void MACDDialog::createGeneralPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // input
  InputType it;
  QStringList l;
  l = it.list();
  l.append(_settings->data("INPUT"));
  l.removeDuplicates();
  
  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data("INPUT"), Qt::MatchExactly));
  _input->setEditable(TRUE);
  form->addRow(tr("Input"), _input);

  // fast ma type
  MAType mat;
  l = mat.list();

  _fastMAType = new QComboBox;
  _fastMAType->addItems(l);
  _fastMAType->setCurrentIndex(_fastMAType->findText(_settings->data("MA_TYPE_FAST"), Qt::MatchExactly));
  form->addRow(tr("Fast MA"), _fastMAType);

  // slow ma type
  _slowMAType = new QComboBox;
  _slowMAType->addItems(l);
  _slowMAType->setCurrentIndex(_slowMAType->findText(_settings->data("MA_TYPE_SLOW"), Qt::MatchExactly));
  form->addRow(tr("Slow MA"), _slowMAType);

  // fast period
  _fastPeriod = new QSpinBox;
  _fastPeriod->setRange(1, 100000);
  _fastPeriod->setValue(_settings->getInt("PERIOD_FAST"));
  form->addRow(tr("Fast Period"), _fastPeriod);

  // slow period
  _slowPeriod = new QSpinBox;
  _slowPeriod->setRange(1, 100000);
  _slowPeriod->setValue(_settings->getInt("PERIOD_SLOW"));
  form->addRow(tr("Slow Period"), _slowPeriod);

  _tabs->addTab(w, tr("General"));
}

void MACDDialog::createMACDPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _macdOutput = new QLineEdit(_settings->data("OUTPUT_MACD"));
  form->addRow(tr("Output"), _macdOutput);

  // color
  _macdColor = new ColorButton(this, QColor(_settings->data("COLOR_MACD")));
  _macdColor->setColorButton();
  form->addRow(tr("Color"), _macdColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _macdStyle = new QComboBox;
  _macdStyle->addItems(l);
  _macdStyle->setCurrentIndex(_macdStyle->findText(_settings->data("STYLE_MACD"), Qt::MatchExactly));
  form->addRow(tr("Style"), _macdStyle);

  // z
  _macdZ = new QSpinBox;
  _macdZ->setRange(-1, 99);
  _macdZ->setValue(_settings->getInt("Z_MACD"));
  form->addRow(tr("Plot Order"), _macdZ);

  _tabs->addTab(w, "MACD");
}

void MACDDialog::createSignalPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _sigOutput = new QLineEdit(_settings->data("OUTPUT_SIG"));
  form->addRow(tr("Output"), _sigOutput);

  // ma type
  MAType mat;
  QStringList l = mat.list();

  _signalMAType = new QComboBox;
  _signalMAType->addItems(l);
  _signalMAType->setCurrentIndex(_signalMAType->findText(_settings->data("MA_TYPE_SIG"), Qt::MatchExactly));
  form->addRow(tr("Type"), _signalMAType);

  // period
  _signalPeriod = new QSpinBox;
  _signalPeriod->setRange(2, 100000);
  _signalPeriod->setValue(_settings->getInt("PERIOD_SIG"));
  form->addRow(tr("Period"), _signalPeriod);

  // color
  _signalColor = new ColorButton(this, QColor(_settings->data("COLOR_SIG")));
  _signalColor->setColorButton();
  form->addRow(tr("Color"), _signalColor);

  // plot style
  Curve c;
  l = c.list();

  _signalStyle = new QComboBox;
  _signalStyle->addItems(l);
  _signalStyle->setCurrentIndex(_signalStyle->findText(_settings->data("STYLE_SIG"), Qt::MatchExactly));
  form->addRow(tr("Style"), _signalStyle);

  // z
  _sigZ = new QSpinBox;
  _sigZ->setRange(-1, 99);
  _sigZ->setValue(_settings->getInt("Z_SIG"));
  form->addRow(tr("Plot Order"), _sigZ);

  _tabs->addTab(w, "SIG");
}

void MACDDialog::createHistPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _histOutput = new QLineEdit(_settings->data("OUTPUT_HIST"));
  form->addRow(tr("Output"), _histOutput);

  // color
  _histColor = new ColorButton(this, QColor(_settings->data("COLOR_HIST")));
  _histColor->setColorButton();
  form->addRow(tr("Color"), _histColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _histStyle = new QComboBox;
  _histStyle->addItems(l);
  _histStyle->setCurrentIndex(_histStyle->findText(_settings->data("STYLE_HIST"), Qt::MatchExactly));
  form->addRow(tr("Style"), _histStyle);

  // z
  _histZ = new QSpinBox;
  _histZ->setRange(-1, 99);
  _histZ->setValue(_settings->getInt("Z_HIST"));
  form->addRow(tr("Plot Order"), _histZ);

  _tabs->addTab(w, "HIST");
}

void MACDDialog::save ()
{
  _settings->setData("COLOR_MACD", _macdColor->color().name());
  _settings->setData("COLOR_SIG", _signalColor->color().name());
  _settings->setData("COLOR_HIST", _histColor->color().name());
  _settings->setData("STYLE_MACD", _macdStyle->currentText());
  _settings->setData("STYLE_SIG", _signalStyle->currentText());
  _settings->setData("STYLE_HIST", _histStyle->currentText());
  _settings->setData("PERIOD_FAST", _fastPeriod->value());
  _settings->setData("PERIOD_SLOW", _slowPeriod->value());
  _settings->setData("PERIOD_SIG", _signalPeriod->value());
  _settings->setData("MA_TYPE_FAST", _fastMAType->currentText());
  _settings->setData("MA_TYPE_SLOW", _slowMAType->currentText());
  _settings->setData("MA_TYPE_SIG", _signalMAType->currentText());
  _settings->setData("INPUT", _input->currentText());
  _settings->setData("OUTPUT_HIST", _histOutput->text());
  _settings->setData("OUTPUT_MACD", _macdOutput->text());
  _settings->setData("OUTPUT_SIG", _sigOutput->text());
  _settings->setData("Z_MACD", _macdZ->text());
  _settings->setData("Z_SIG", _sigZ->text());
  _settings->setData("Z_HIST", _histZ->text());
}
