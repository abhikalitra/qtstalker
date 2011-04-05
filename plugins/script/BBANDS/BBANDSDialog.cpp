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

#include "BBANDSDialog.h"
#include "Globals.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

BBANDSDialog::BBANDSDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createUpperPage();
  createMiddlePage();
  createLowerPage();
}

void BBANDSDialog::createGeneralPage ()
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
  QStringList l = it.list();
  l.append(_settings->data("INPUT"));
  l.removeDuplicates();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setEditable(TRUE);
  _input->setCurrentIndex(_input->findText(_settings->data("INPUT"), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings->getInt("PERIOD"));
  form->addRow(tr("Period"), _period);

  // up deviation
  _upDev = new QDoubleSpinBox;
  _upDev->setRange(-100000, 100000);
  _upDev->setValue(_settings->getDouble("DEVIATION_UP"));
  form->addRow(tr("Upper Deviation"), _upDev);

  // low deviation
  _lowDev = new QDoubleSpinBox;
  _lowDev->setRange(-100000, 100000);
  _lowDev->setValue(_settings->getDouble("DEVIATION_DOWN"));
  form->addRow(tr("Lower Deviation"), _lowDev);

  // ma type
  MAType mat;
  l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data("MA_TYPE"), Qt::MatchExactly));
  form->addRow(tr("MA Type"), _maType);

  _tabs->addTab(w, "BBANDS");
}

void BBANDSDialog::createUpperPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _upOutput = new QLineEdit(_settings->data("OUTPUT_UPPER"));
  form->addRow(tr("Output"), _upOutput);

  // color
  _upperColor = new ColorButton(this, QColor(_settings->data("COLOR_UP")));
  _upperColor->setColorButton();
  form->addRow(tr("Color"), _upperColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _upperStyle = new QComboBox;
  _upperStyle->addItems(l);
  _upperStyle->setCurrentIndex(_upperStyle->findText(_settings->data("STYLE_UP"), Qt::MatchExactly));
  form->addRow(tr("Style"), _upperStyle);

  // z
  _zUp = new QSpinBox;
  _zUp->setRange(-1, 99);
  _zUp->setValue(_settings->getInt("Z_UP"));
  form->addRow(tr("Plot Order"), _zUp);

  _tabs->addTab(w, tr("Upper"));
}

void BBANDSDialog::createMiddlePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _midOutput = new QLineEdit(_settings->data("OUTPUT_MID"));
  form->addRow(tr("Output"), _midOutput);

  // color
  _midColor = new ColorButton(this, QColor(_settings->data("COLOR_MID")));
  _midColor->setColorButton();
  form->addRow(tr("Color"), _midColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _midStyle = new QComboBox;
  _midStyle->addItems(l);
  _midStyle->setCurrentIndex(_midStyle->findText(_settings->data("STYLE_MID"), Qt::MatchExactly));
  form->addRow(tr("Style"), _midStyle);

  // z
  _zMid = new QSpinBox;
  _zMid->setRange(-1, 99);
  _zMid->setValue(_settings->getInt("Z_MID"));
  form->addRow(tr("Plot Order"), _zMid);

  _tabs->addTab(w, tr("Middle"));
}

void BBANDSDialog::createLowerPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _downOutput = new QLineEdit(_settings->data("OUTPUT_DOWN"));
  form->addRow(tr("Output"), _downOutput);

  // color
  _lowerColor = new ColorButton(this, QColor(_settings->data("COLOR_DOWN")));
  _lowerColor->setColorButton();
  form->addRow(tr("Color"), _lowerColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _lowerStyle = new QComboBox;
  _lowerStyle->addItems(l);
  _lowerStyle->setCurrentIndex(_lowerStyle->findText(_settings->data("STYLE_DOWN"), Qt::MatchExactly));
  form->addRow(tr("Style"), _lowerStyle);

  // z
  _zDown = new QSpinBox;
  _zDown->setRange(-1, 99);
  _zDown->setValue(_settings->getInt("Z_DOWN"));
  form->addRow(tr("Plot Order"), _zDown);

  _tabs->addTab(w, tr("Lower"));
}

void BBANDSDialog::save ()
{
  _settings->setData("COLOR_UP", _upperColor->color().name());
  _settings->setData("COLOR_MID", _midColor->color().name());
  _settings->setData("COLOR_DOWN", _lowerColor->color().name());
  _settings->setData("STYLE_UP", _upperStyle->currentText());
  _settings->setData("STYLE_MID", _midStyle->currentText());
  _settings->setData("STYLE_DOWN", _lowerStyle->currentText());
  _settings->setData("DEVIATION_UP", _upDev->value());
  _settings->setData("DEVIATION_DOWN", _lowDev->value());
  _settings->setData("INPUT", _input->currentText());
  _settings->setData("PERIOD", _period->value());
  _settings->setData("MA_TYPE", _maType->currentText());
  _settings->setData("Z_UP", _zUp->text());
  _settings->setData("Z_MID", _zMid->text());
  _settings->setData("Z_DOWN", _zDown->text());
  _settings->setData("OUTPUT_UP", _upOutput->text());
  _settings->setData("OUTPUT_MID", _midOutput->text());
  _settings->setData("OUTPUT_DOWN", _downOutput->text());
}
