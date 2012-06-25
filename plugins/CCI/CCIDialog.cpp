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

#include "CCIDialog.h"
#include "Doc.h"
#include "Global.h"
#include "MAType.h"
#include "CurveLineType.h"
#include "BarType.h"

#include <QtDebug>
#include <QVariant>
#include <QLayout>
#include <QFormLayout>


CCIDialog::CCIDialog (QWidget *p) : QDialog (p)
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

void CCIDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void CCIDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void CCIDialog::cancel ()
{
  saveSettings();
  reject();
}

void CCIDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("cci_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("cci_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void CCIDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("cci_dialog_size", size());
  settings.setValue("cci_dialog_pos", pos());
}

void CCIDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // CCI page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // color
  QVariant *set = _settings->get(QString("cciColor"));
  
  _cciColor = new ColorButton(this, QColor(set->toString()));
  connect(_cciColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Up Color"), _cciColor);
  
  // period
  set = _settings->get(QString("cciPeriod"));

  _cciPeriod = new QSpinBox;
  _cciPeriod->setRange(1, 999999);
  _cciPeriod->setValue(set->toInt());
  connect(_cciPeriod, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _cciPeriod);
  
  // style
  set = _settings->get(QString("cciStyle"));

  CurveLineType clt;
  _cciStyle = new QComboBox;
  _cciStyle->addItems(clt.list());
  _cciStyle->setCurrentIndex(_cciStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_cciStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _cciStyle);

  // width
  set = _settings->get(QString("cciWidth"));

  _cciWidth = new QSpinBox;
  _cciWidth->setRange(1, 99);
  _cciWidth->setValue(set->toInt());
  connect(_cciWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _cciWidth);

  // show
  set = _settings->get(QString("cciShow"));

  _cciShow = new QCheckBox;
  _cciShow->setChecked(set->toBool());
  connect(_cciShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _cciShow);

  _tabs->addTab(w, QString("CCI"));

  
  // MA page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // type
  set = _settings->get(QString("maType"));

  MAType mat;
  _maType = new QComboBox;
  _maType->addItems(mat.list());
  _maType->setCurrentIndex(_maType->findText(set->toString(), Qt::MatchExactly));
  connect(_maType, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _maType);
  
  // style
  set = _settings->get(QString("maStyle"));

  _maStyle = new QComboBox;
  _maStyle->addItems(clt.list());
  _maStyle->setCurrentIndex(_maStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_maStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _maStyle);

  // width
  set = _settings->get(QString("maWidth"));

  _maWidth = new QSpinBox;
  _maWidth->setRange(1, 99);
  _maWidth->setValue(set->toInt());
  connect(_maWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _maWidth);
  
  // color
  set = _settings->get(QString("maColor"));
  
  _maColor = new ColorButton(this, QColor(set->toString()));
  connect(_maColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _maColor);
  
  // period
  set = _settings->get(QString("maPeriod"));

  _maPeriod = new QSpinBox;
  _maPeriod->setRange(1, 999999);
  _maPeriod->setValue(set->toInt());
  connect(_maPeriod, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _maPeriod);
  
  // show
  set = _settings->get(QString("maShow"));

  _maShow = new QCheckBox;
  _maShow->setChecked(set->toBool());
  connect(_maShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _maShow);

  _tabs->addTab(w, QString("MA"));
  

  // buy marker page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("buyMarkerColor"));
  
  _ref1Color = new ColorButton(this, QColor(set->toString()));
  connect(_ref1Color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _ref1Color);

  // value
  set = _settings->get(QString("buyMarkerPrice"));

  _ref1 = new QDoubleSpinBox;
  _ref1->setRange(-99999999.0, 99999999.0);
  _ref1->setValue(set->toDouble());
  connect(_ref1, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Value"), _ref1);

  // show
  set = _settings->get(QString("buyMarkerShow"));

  _ref1Show = new QCheckBox;
  _ref1Show->setChecked(set->toBool());
  connect(_ref1Show, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _ref1Show);

  _tabs->addTab(w, tr("Buy Marker"));
  

  // REF2 page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("sellMarkerColor"));
  
  _ref2Color = new ColorButton(this, QColor(set->toString()));
  connect(_ref2Color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _ref2Color);

  // value
  set = _settings->get(QString("sellMarkerPrice"));

  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-99999999.0, 99999999.0);
  _ref2->setValue(set->toDouble());
  connect(_ref2, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Value"), _ref2);

  // show
  set = _settings->get(QString("sellMarkerShow"));

  _ref2Show = new QCheckBox;
  _ref2Show->setChecked(set->toBool());
  connect(_ref2Show, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _ref2Show);

  _tabs->addTab(w, tr("Sell Marker"));
}

void CCIDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("cciColor"));
  set->setValue(_cciColor->color().name());

  set = _settings->get(QString("cciPeriod"));
  set->setValue(_cciPeriod->value());

  set = _settings->get(QString("cciWidth"));
  set->setValue(_cciWidth->value());

  set = _settings->get(QString("cciStyle"));
  set->setValue(_cciStyle->currentText());

  set = _settings->get(QString("cciShow"));
  set->setValue(_cciShow->isChecked());

  set = _settings->get(QString("maColor"));
  set->setValue(_maColor->color().name());

  set = _settings->get(QString("maPeriod"));
  set->setValue(_maPeriod->value());

  set = _settings->get(QString("maWidth"));
  set->setValue(_maWidth->value());

  set = _settings->get(QString("maStyle"));
  set->setValue(_maStyle->currentText());

  set = _settings->get(QString("maType"));
  set->setValue(_maType->currentText());

  set = _settings->get(QString("maShow"));
  set->setValue(_maShow->isChecked());

  set = _settings->get(QString("buyMarkerColor"));
  set->setValue(_ref1Color->color().name());

  set = _settings->get(QString("buyMarkerPrice"));
  set->setValue(_ref1->value());

  set = _settings->get(QString("buyMarkerShow"));
  set->setValue(_ref1Show->isChecked());

  set = _settings->get(QString("sellMarkerColor"));
  set->setValue(_ref2Color->color().name());

  set = _settings->get(QString("sellMarkerPrice"));
  set->setValue(_ref2->value());

  set = _settings->get(QString("sellMarkerShow"));
  set->setValue(_ref2Show->isChecked());
}

void CCIDialog::modified ()
{
  _modified = 1;
}
