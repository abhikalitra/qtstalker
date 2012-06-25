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
#include "Doc.h"
#include "Global.h"
#include "MAType.h"
#include "CurveLineType.h"
#include "BarType.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


AROONDialog::AROONDialog (QWidget *p) : QDialog (p)
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

void AROONDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void AROONDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void AROONDialog::cancel ()
{
  saveSettings();
  reject();
}

void AROONDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("aroon_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("aroon_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void AROONDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("aroon_dialog_size", size());
  settings.setValue("aroon_dialog_pos", pos());
}

void AROONDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // General page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // period
  QVariant *set = _settings->get(QString("period"));

  _period = new QSpinBox;
  _period->setRange(1, 9999);
  _period->setValue(set->toInt());
  connect(_period, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _period);
  
  _tabs->addTab(w, tr("General"));
  

  // UP page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // color
  set = _settings->get(QString("upColor"));
  
  _upColor = new ColorButton(this, QColor(set->toString()));
  connect(_upColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _upColor);
  
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

  // show
  set = _settings->get(QString("upShow"));

  _upShow = new QCheckBox;
  _upShow->setChecked(set->toBool());
  connect(_upShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _upShow);
  
  _tabs->addTab(w, tr("Up"));

  // DOWN page
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
  set = _settings->get(QString("downColor"));
  
  _downColor = new ColorButton(this, QColor(set->toString()));
  connect(_downColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _downColor);
  
  // show
  set = _settings->get(QString("downShow"));

  _downShow = new QCheckBox;
  _downShow->setChecked(set->toBool());
  connect(_downShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _downShow);

  _tabs->addTab(w, tr("Down"));
}

void AROONDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("period"));
  set->setValue(_period->value());
  
  set = _settings->get(QString("upColor"));
  set->setValue(_upColor->color().name());

  set = _settings->get(QString("upWidth"));
  set->setValue(_upWidth->value());

  set = _settings->get(QString("upStyle"));
  set->setValue(_upStyle->currentText());

  set = _settings->get(QString("upShow"));
  set->setValue(_upShow->isChecked());
  
  set = _settings->get(QString("downColor"));
  set->setValue(_downColor->color().name());

  set = _settings->get(QString("downWidth"));
  set->setValue(_downWidth->value());

  set = _settings->get(QString("downStyle"));
  set->setValue(_downStyle->currentText());

  set = _settings->get(QString("downShow"));
  set->setValue(_downShow->isChecked());
}

void AROONDialog::modified ()
{
  _modified = 1;
}
