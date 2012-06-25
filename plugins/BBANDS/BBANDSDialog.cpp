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
#include "Doc.h"
#include "Global.h"
#include "CurveOHLCType.h"
#include "BarType.h"
#include "MAType.h"
#include "CurveLineType.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


BBANDSDialog::BBANDSDialog (QWidget *p) : QDialog (p)
{
  _settings = 0;
  _modified = 0;
  _helpFile = "main.html";
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  // main vbox
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();

  // help button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void BBANDSDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void BBANDSDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void BBANDSDialog::cancel ()
{
  saveSettings();
  reject();
}

void BBANDSDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("bbands_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("bbands_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void BBANDSDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("bbands_dialog_size", size());
  settings.setValue("bbands_dialog_pos", pos());
}

void BBANDSDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // BBANDS page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // style
  QVariant *set = _settings->get(QString("style"));
  
  CurveOHLCType ct;
  _ohlcStyle = new QComboBox;
  _ohlcStyle->addItems(ct.list());
  _ohlcStyle->setCurrentIndex(_ohlcStyle->findText(set->toString()));
  connect(_ohlcStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _ohlcStyle);
  
  // up color
  set = _settings->get(QString("upColor"));
  
  _upColor = new ColorButton(this, QColor(set->toString()));
  connect(_upColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Up Color"), _upColor);
  
  // down color
  set = _settings->get(QString("downColor"));
  
  _downColor = new ColorButton(this, QColor(set->toString()));
  connect(_downColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Down Color"), _downColor);
  
  // neutral color
  set = _settings->get(QString("neutralColor"));
  
  _neutralColor = new ColorButton(this, QColor(set->toString()));
  connect(_neutralColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Neutral Color"), _neutralColor);
  
  _tabs->addTab(w, QString("OHLC"));

  // BBANDS page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  BarType bt;
  QStringList il;
  il << bt.indexToString(BarType::_OPEN);
  il << bt.indexToString(BarType::_HIGH);
  il << bt.indexToString(BarType::_LOW);
  il << bt.indexToString(BarType::_CLOSE);

  // input
  set = _settings->get(QString("input"));

  _input = new QComboBox;
  _input->addItems(il);
  _input->setCurrentIndex(_input->findText(set->toString(), Qt::MatchExactly));
  connect(_input, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Input"), _input);
  
  // type
  set = _settings->get(QString("type"));

  MAType mat;
  _ma = new QComboBox;
  _ma->addItems(mat.list());
  _ma->setCurrentIndex(_ma->findText(set->toString(), Qt::MatchExactly));
  connect(_ma, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("MA"), _ma);

  // period
  set = _settings->get(QString("period"));

  _period = new QSpinBox;
  _period->setRange(1, 9999);
  _period->setValue(set->toInt());
  connect(_period, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _period);
  
  // dev up
  set = _settings->get(QString("devUp"));

  _devUp = new QDoubleSpinBox;
  _devUp->setRange(0.0, 999999.0);
  _devUp->setValue(set->toDouble());
  connect(_devUp, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Deviations Up"), _devUp);
  
  // dev down
  set = _settings->get(QString("devDown"));

  _devDown = new QDoubleSpinBox;
  _devDown->setRange(0.0, 999999.0);
  _devDown->setValue(set->toDouble());
  connect(_devDown, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Deviations Down"), _devDown);
  
  _tabs->addTab(w, QString("BBANDS"));
  
  // Upper page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // style
  set = _settings->get(QString("upStyle"));

  CurveLineType clt;
  _upStyle = new QComboBox;
  _upStyle->addItems(clt.list());
  _upStyle->setCurrentIndex(_upStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_upStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _upStyle);

  // width
  set = _settings->get(QString("upWidth"));

  _upWidth = new QSpinBox;
  _upWidth->setRange(1, 99);
  _upWidth->setValue(set->toInt());
  connect(_upWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _upWidth);
  
  // color
  set = _settings->get(QString("upperColor"));
  
  _upperColor = new ColorButton(this, QColor(set->toString()));
  connect(_upperColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _upperColor);
  
  // show
  set = _settings->get(QString("upShow"));

  _upShow = new QCheckBox;
  _upShow->setChecked(set->toBool());
  connect(_upShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _upShow);

  _tabs->addTab(w, tr("Upper"));

  
  // Mid page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // style
  set = _settings->get(QString("midStyle"));

  _midStyle = new QComboBox;
  _midStyle->addItems(clt.list());
  _midStyle->setCurrentIndex(_midStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_midStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _midStyle);

  // width
  set = _settings->get(QString("midWidth"));

  _midWidth = new QSpinBox;
  _midWidth->setRange(1, 99);
  _midWidth->setValue(set->toInt());
  connect(_midWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _midWidth);
  
  // color
  set = _settings->get(QString("midColor"));
  
  _midColor = new ColorButton(this, QColor(set->toString()));
  connect(_midColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _midColor);
  
  // show
  set = _settings->get(QString("midShow"));

  _midShow = new QCheckBox;
  _midShow->setChecked(set->toBool());
  connect(_midShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _midShow);

  _tabs->addTab(w, tr("Middle"));
  
  // Lower page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // style
  set = _settings->get(QString("downStyle"));

  _downStyle = new QComboBox;
  _downStyle->addItems(clt.list());
  _downStyle->setCurrentIndex(_downStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_downStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _downStyle);

  // width
  set = _settings->get(QString("downWidth"));

  _downWidth = new QSpinBox;
  _downWidth->setRange(1, 99);
  _downWidth->setValue(set->toInt());
  connect(_downWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _downWidth);
  
  // color
  set = _settings->get(QString("lowerColor"));
  
  _lowerColor = new ColorButton(this, QColor(set->toString()));
  connect(_lowerColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _lowerColor);
  
  // show
  set = _settings->get(QString("downShow"));

  _downShow = new QCheckBox;
  _downShow->setChecked(set->toBool());
  connect(_downShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _downShow);

  _tabs->addTab(w, tr("Lower"));
}

void BBANDSDialog::save ()
{
  if (! _modified)
    return;

  // ohlc
  QVariant *set = _settings->get(QString("style"));
  set->setValue(_ohlcStyle->currentText());
  
  set = _settings->get(QString("upColor"));
  set->setValue(_upColor->color().name());

  set = _settings->get(QString("downColor"));
  set->setValue(_downColor->color().name());

  set = _settings->get(QString("neutralColor"));
  set->setValue(_neutralColor->color().name());

  // bbands
  set = _settings->get(QString("input"));
  set->setValue(_input->currentText());
  
  set = _settings->get(QString("period"));
  set->setValue(_period->value());

  set = _settings->get(QString("devUp"));
  set->setValue(_devUp->value());

  set = _settings->get(QString("devDown"));
  set->setValue(_devDown->value());
  
  set = _settings->get(QString("type"));
  set->setValue(_ma->currentText());

  // upper
  set = _settings->get(QString("upStyle"));
  set->setValue(_upStyle->currentText());

  set = _settings->get(QString("upWidth"));
  set->setValue(_upWidth->value());

  set = _settings->get(QString("upperColor"));
  set->setValue(_upperColor->color().name());

  set = _settings->get(QString("upShow"));
  set->setValue(_upShow->isChecked());

  // middle
  set = _settings->get(QString("midStyle"));
  set->setValue(_midStyle->currentText());

  set = _settings->get(QString("midWidth"));
  set->setValue(_midWidth->value());

  set = _settings->get(QString("midColor"));
  set->setValue(_midColor->color().name());

  set = _settings->get(QString("midShow"));
  set->setValue(_midShow->isChecked());

  // lower
  set = _settings->get(QString("downStyle"));
  set->setValue(_downStyle->currentText());

  set = _settings->get(QString("downWidth"));
  set->setValue(_downWidth->value());

  set = _settings->get(QString("lowerColor"));
  set->setValue(_lowerColor->color().name());

  set = _settings->get(QString("downShow"));
  set->setValue(_downShow->isChecked());
}

void BBANDSDialog::modified ()
{
  _modified = 1;
}
