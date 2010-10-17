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

#include "PPDialog.h"
#include "PP.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

PPDialog::PPDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createR1Page ();
  createR2Page ();
  createR3Page ();
  createS1Page ();
  createS2Page ();
  createS3Page ();
}

void PPDialog::createR1Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(PP::_R1Color, d);
  QColor c(d);

  _r1Color = new ColorButton(this, c);
  _r1Color->setColorButton();
  form->addRow(tr("Color"), _r1Color);

  // label
  _settings.getData(PP::_R1Label, d);

  _r1Label = new QLineEdit(d);
  form->addRow(tr("Label"), _r1Label);

  // check
  _r1Check = new QCheckBox;
  _r1Check->setChecked(_settings.getInt(PP::_R1Show));
  form->addRow(tr("Show"), _r1Check);

  _tabs->addTab(w, tr("R1"));
}

void PPDialog::createR2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(PP::_R2Color, d);
  QColor c(d);

  _r2Color = new ColorButton(this, c);
  _r2Color->setColorButton();
  form->addRow(tr("Color"), _r2Color);

  // label
  _settings.getData(PP::_R2Label, d);

  _r2Label = new QLineEdit(d);
  form->addRow(tr("Label"), _r2Label);

  // check
  _r2Check = new QCheckBox;
  _r2Check->setChecked(_settings.getInt(PP::_R2Show));
  form->addRow(tr("Show"), _r2Check);

  _tabs->addTab(w, tr("R2"));
}

void PPDialog::createR3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(PP::_R3Color, d);
  QColor c(d);

  _r3Color = new ColorButton(this, c);
  _r3Color->setColorButton();
  form->addRow(tr("Color"), _r3Color);

  // label
  _settings.getData(PP::_R3Label, d);

  _r3Label = new QLineEdit(d);
  form->addRow(tr("Label"), _r3Label);

  // check
  _r3Check = new QCheckBox;
  _r3Check->setChecked(_settings.getInt(PP::_R3Show));
  form->addRow(tr("Show"), _r3Check);

  _tabs->addTab(w, tr("R3"));
}

void PPDialog::createS1Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(PP::_S1Color, d);
  QColor c(d);

  _s1Color = new ColorButton(this, c);
  _s1Color->setColorButton();
  form->addRow(tr("Color"), _s1Color);

  // label
  _settings.getData(PP::_S1Label, d);

  _s1Label = new QLineEdit(d);
  form->addRow(tr("Label"), _s1Label);

  // check
  _s1Check = new QCheckBox;
  _s1Check->setChecked(_settings.getInt(PP::_S1Show));
  form->addRow(tr("Show"), _s1Check);

  _tabs->addTab(w, tr("S1"));
}

void PPDialog::createS2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(PP::_S2Color, d);
  QColor c(d);

  _s2Color = new ColorButton(this, c);
  _s2Color->setColorButton();
  form->addRow(tr("Color"), _s2Color);

  // label
  _settings.getData(PP::_S2Label, d);

  _s2Label = new QLineEdit(d);
  form->addRow(tr("Label"), _s2Label);

  // check
  _s2Check = new QCheckBox;
  _s2Check->setChecked(_settings.getInt(PP::_S2Show));
  form->addRow(tr("Show"), _s2Check);

  _tabs->addTab(w, tr("S2"));
}

void PPDialog::createS3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(PP::_S3Color, d);
  QColor c(d);

  _s3Color = new ColorButton(this, c);
  _s3Color->setColorButton();
  form->addRow(tr("Color"), _s3Color);

  // label
  _settings.getData(PP::_S3Label, d);

  _s3Label = new QLineEdit(d);
  form->addRow(tr("Label"), _s3Label);

  // check
  _s3Check = new QCheckBox;
  _s3Check->setChecked(_settings.getInt(PP::_S3Show));
  form->addRow(tr("Show"), _s3Check);

  _tabs->addTab(w, tr("S3"));
}

void PPDialog::done ()
{
  _settings.setData(PP::_R1Color, _r1Color->color().name());
  _settings.setData(PP::_R2Color, _r2Color->color().name());
  _settings.setData(PP::_R3Color, _r3Color->color().name());
  _settings.setData(PP::_S1Color, _s1Color->color().name());
  _settings.setData(PP::_S2Color, _s2Color->color().name());
  _settings.setData(PP::_S3Color, _s3Color->color().name());
  _settings.setData(PP::_R1Label, _r1Label->text());
  _settings.setData(PP::_R2Label, _r2Label->text());
  _settings.setData(PP::_R3Label, _r3Label->text());
  _settings.setData(PP::_S1Label, _s1Label->text());
  _settings.setData(PP::_S2Label, _s2Label->text());
  _settings.setData(PP::_S3Label, _s3Label->text());
  _settings.setData(PP::_R1Show, (int) _r1Check->isChecked());
  _settings.setData(PP::_R2Show, (int) _r2Check->isChecked());
  _settings.setData(PP::_R3Show, (int) _r3Check->isChecked());
  _settings.setData(PP::_S1Show, (int) _s1Check->isChecked());
  _settings.setData(PP::_S2Show, (int) _s2Check->isChecked());
  _settings.setData(PP::_S3Show, (int) _s3Check->isChecked());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

