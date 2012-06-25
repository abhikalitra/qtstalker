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

#include "SARDialog.h"
#include "Doc.h"
#include "Global.h"
#include "CurveOHLCType.h"
#include "BarType.h"
#include "CurveLineType.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


SARDialog::SARDialog (QWidget *p) : QDialog (p)
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

void SARDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void SARDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void SARDialog::cancel ()
{
  saveSettings();
  reject();
}

void SARDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("sar_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("sar_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void SARDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("sar_dialog_size", size());
  settings.setValue("sar_dialog_pos", pos());
}

void SARDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // SAR page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // style
  QVariant *set = _settings->get(QString("style"));
  
  CurveOHLCType ct;
  _style = new QComboBox;
  _style->addItems(ct.list());
  _style->setCurrentIndex(_style->findText(set->toString()));
  connect(_style, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _style);
  
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

  // SAR page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("sarColor"));
  
  _sarColor = new ColorButton(this, QColor(set->toString()));
  connect(_sarColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _sarColor);

  // init
  set = _settings->get(QString("init"));

  _init = new QDoubleSpinBox;
  _init->setRange(0.0, 0.2);
  _init->setValue(set->toDouble());
  connect(_init, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Initial"), _init);
  
  // max
  set = _settings->get(QString("max"));

  _max = new QDoubleSpinBox;
  _max->setRange(0.0, 0.2);
  _max->setValue(set->toDouble());
  connect(_max, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Maximum Step"), _max);
  
  _tabs->addTab(w, QString("SAR"));
}

void SARDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("style"));
  set->setValue(_style->currentText());

  set = _settings->get(QString("upColor"));
  set->setValue(_upColor->color().name());

  set = _settings->get(QString("downColor"));
  set->setValue(_downColor->color().name());

  set = _settings->get(QString("neutralColor"));
  set->setValue(_neutralColor->color().name());

  set = _settings->get(QString("init"));
  set->setValue(_init->value());

  set = _settings->get(QString("max"));
  set->setValue(_max->value());

  set = _settings->get(QString("sarColor"));
  set->setValue(_sarColor->color().name());
}

void SARDialog::modified ()
{
  _modified = 1;
}
