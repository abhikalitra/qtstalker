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
#include "Doc.h"
#include "Global.h"
#include "CurveLineType.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


ADXDialog::ADXDialog (QWidget *p) : QDialog (p)
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

void ADXDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void ADXDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ADXDialog::cancel ()
{
  saveSettings();
  reject();
}

void ADXDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("adx_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("adx_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void ADXDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("adx_dialog_size", size());
  settings.setValue("adx_dialog_pos", pos());
}

void ADXDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // ADX page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // general page
  
  // period
  QVariant *set = _settings->get(QString("period"));

  _period = new QSpinBox;
  _period->setRange(1, 999999);
  _period->setValue(set->toInt());
  connect(_period, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _period);
  
  _tabs->addTab(w, tr("General"));
  
  
  // MDI page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("mdiColor"));
  
  _mdiColor = new ColorButton(this, QColor(set->toString()));
  connect(_mdiColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _mdiColor);
  
  // style
  set = _settings->get(QString("mdiStyle"));

  CurveLineType clt;
  _mdiStyle = new QComboBox;
  _mdiStyle->addItems(clt.list());
  _mdiStyle->setCurrentIndex(_mdiStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_mdiStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _mdiStyle);

  // width
  set = _settings->get(QString("mdiWidth"));

  _mdiWidth = new QSpinBox;
  _mdiWidth->setRange(1, 99);
  _mdiWidth->setValue(set->toInt());
  connect(_mdiWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _mdiWidth);

  // show
  set = _settings->get(QString("mdiShow"));

  _mdiShow = new QCheckBox;
  _mdiShow->setChecked(set->toBool());
  connect(_mdiShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _mdiShow);
  
  _tabs->addTab(w, QString("MDI"));
  

  // PDI page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("pdiColor"));
  
  _pdiColor = new ColorButton(this, QColor(set->toString()));
  connect(_pdiColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _pdiColor);
  
  // style
  set = _settings->get(QString("pdiStyle"));

  _pdiStyle = new QComboBox;
  _pdiStyle->addItems(clt.list());
  _pdiStyle->setCurrentIndex(_pdiStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_pdiStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _pdiStyle);

  // width
  set = _settings->get(QString("pdiWidth"));

  _pdiWidth = new QSpinBox;
  _pdiWidth->setRange(1, 99);
  _pdiWidth->setValue(set->toInt());
  connect(_pdiWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _pdiWidth);

  // show
  set = _settings->get(QString("pdiShow"));

  _pdiShow = new QCheckBox;
  _pdiShow->setChecked(set->toBool());
  connect(_pdiShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _pdiShow);
  
  _tabs->addTab(w, QString("PDI"));
  

  // ADX page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("adxColor"));
  
  _adxColor = new ColorButton(this, QColor(set->toString()));
  connect(_adxColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _adxColor);
  
  // style
  set = _settings->get(QString("adxStyle"));

  _adxStyle = new QComboBox;
  _adxStyle->addItems(clt.list());
  _adxStyle->setCurrentIndex(_adxStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_adxStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _adxStyle);

  // width
  set = _settings->get(QString("adxWidth"));

  _adxWidth = new QSpinBox;
  _adxWidth->setRange(1, 99);
  _adxWidth->setValue(set->toInt());
  connect(_adxWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _adxWidth);

  // show
  set = _settings->get(QString("adxShow"));

  _adxShow = new QCheckBox;
  _adxShow->setChecked(set->toBool());
  connect(_adxShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _adxShow);
  
  _tabs->addTab(w, QString("ADX"));
  

  // ADXR page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("adxrColor"));
  
  _adxrColor = new ColorButton(this, QColor(set->toString()));
  connect(_adxrColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _adxrColor);
  
  // style
  set = _settings->get(QString("adxrStyle"));

  _adxrStyle = new QComboBox;
  _adxrStyle->addItems(clt.list());
  _adxrStyle->setCurrentIndex(_adxrStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_adxrStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _adxrStyle);

  // width
  set = _settings->get(QString("adxrWidth"));

  _adxrWidth = new QSpinBox;
  _adxrWidth->setRange(1, 99);
  _adxrWidth->setValue(set->toInt());
  connect(_adxrWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _adxrWidth);

  // show
  set = _settings->get(QString("adxrShow"));

  _adxrShow = new QCheckBox;
  _adxrShow->setChecked(set->toBool());
  connect(_adxrShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _adxrShow);
  
  _tabs->addTab(w, QString("ADXR"));
}

void ADXDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("period"));
  set->setValue(_period->value());

  set = _settings->get(QString("mdiColor"));
  set->setValue(_mdiColor->color().name());

  set = _settings->get(QString("mdiStyle"));
  set->setValue(_mdiStyle->currentText());

  set = _settings->get(QString("mdiWidth"));
  set->setValue(_mdiWidth->value());

  set = _settings->get(QString("mdiShow"));
  set->setValue(_mdiShow->isChecked());

  set = _settings->get(QString("pdiColor"));
  set->setValue(_pdiColor->color().name());

  set = _settings->get(QString("pdiStyle"));
  set->setValue(_pdiStyle->currentText());

  set = _settings->get(QString("pdiWidth"));
  set->setValue(_pdiWidth->value());

  set = _settings->get(QString("pdiShow"));
  set->setValue(_pdiShow->isChecked());

  set = _settings->get(QString("adxColor"));
  set->setValue(_adxColor->color().name());

  set = _settings->get(QString("adxStyle"));
  set->setValue(_adxStyle->currentText());

  set = _settings->get(QString("adxWidth"));
  set->setValue(_adxWidth->value());

  set = _settings->get(QString("adxShow"));
  set->setValue(_adxShow->isChecked());

  set = _settings->get(QString("adxrColor"));
  set->setValue(_adxrColor->color().name());

  set = _settings->get(QString("adxrStyle"));
  set->setValue(_adxrStyle->currentText());

  set = _settings->get(QString("adxrWidth"));
  set->setValue(_adxrWidth->value());

  set = _settings->get(QString("adxrShow"));
  set->setValue(_adxrShow->isChecked());
}

void ADXDialog::modified ()
{
  _modified = 1;
}
