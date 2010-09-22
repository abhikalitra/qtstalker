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
#include "FunctionPP.h"
#include "PP.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
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

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(PP::R1Color, d);
  QColor c(d);

  _r1Color = new ColorButton(this, c);
  _r1Color->setColorButton();
  grid->addWidget(_r1Color, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(PP::R1Label, d);

  _r1Label = new QLineEdit(d);
  grid->addWidget(_r1Label, row++, col--);

  // check
  _r1Check = new QCheckBox(tr("Show"));
  _r1Check->setChecked(_settings.getInt(PP::R1Show));
  grid->addWidget(_r1Check, row++, col);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("R1"));
}

void PPDialog::createR2Page ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(PP::R2Color, d);
  QColor c(d);

  _r2Color = new ColorButton(this, c);
  _r2Color->setColorButton();
  grid->addWidget(_r2Color, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(PP::R2Label, d);

  _r2Label = new QLineEdit(d);
  grid->addWidget(_r2Label, row++, col--);

  // check
  _r2Check = new QCheckBox(tr("Show"));
  _r2Check->setChecked(_settings.getInt(PP::R2Show));
  grid->addWidget(_r2Check, row++, col);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("R2"));
}

void PPDialog::createR3Page ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(PP::R3Color, d);
  QColor c(d);

  _r3Color = new ColorButton(this, c);
  _r3Color->setColorButton();
  grid->addWidget(_r3Color, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(PP::R3Label, d);

  _r3Label = new QLineEdit(d);
  grid->addWidget(_r3Label, row++, col--);

  // check
  _r3Check = new QCheckBox(tr("Show"));
  _r3Check->setChecked(_settings.getInt(PP::R3Show));
  grid->addWidget(_r3Check, row++, col);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("R3"));
}

void PPDialog::createS1Page ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(PP::S1Color, d);
  QColor c(d);

  _s1Color = new ColorButton(this, c);
  _s1Color->setColorButton();
  grid->addWidget(_s1Color, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(PP::S1Label, d);

  _s1Label = new QLineEdit(d);
  grid->addWidget(_s1Label, row++, col--);

  // check
  _s1Check = new QCheckBox(tr("Show"));
  _s1Check->setChecked(_settings.getInt(PP::S1Show));
  grid->addWidget(_s1Check, row++, col);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("S1"));
}

void PPDialog::createS2Page ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(PP::S2Color, d);
  QColor c(d);

  _s2Color = new ColorButton(this, c);
  _s2Color->setColorButton();
  grid->addWidget(_s2Color, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(PP::S2Label, d);

  _s2Label = new QLineEdit(d);
  grid->addWidget(_s2Label, row++, col--);

  // check
  _s2Check = new QCheckBox(tr("Show"));
  _s2Check->setChecked(_settings.getInt(PP::S2Show));
  grid->addWidget(_s2Check, row++, col);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("S2"));
}

void PPDialog::createS3Page ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(PP::S3Color, d);
  QColor c(d);

  _s3Color = new ColorButton(this, c);
  _s3Color->setColorButton();
  grid->addWidget(_s3Color, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(PP::S3Label, d);

  _s3Label = new QLineEdit(d);
  grid->addWidget(_s3Label, row++, col--);

  // check
  _s3Check = new QCheckBox(tr("Show"));
  _s3Check->setChecked(_settings.getInt(PP::S3Show));
  grid->addWidget(_s3Check, row++, col);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("S3"));
}

void PPDialog::done ()
{
  _settings.setData(PP::R1Color, _r1Color->color().name());
  _settings.setData(PP::R2Color, _r2Color->color().name());
  _settings.setData(PP::R3Color, _r3Color->color().name());
  _settings.setData(PP::S1Color, _s1Color->color().name());
  _settings.setData(PP::S2Color, _s2Color->color().name());
  _settings.setData(PP::S3Color, _s3Color->color().name());
  _settings.setData(PP::R1Label, _r1Label->text());
  _settings.setData(PP::R2Label, _r2Label->text());
  _settings.setData(PP::R3Label, _r3Label->text());
  _settings.setData(PP::S1Label, _s1Label->text());
  _settings.setData(PP::S2Label, _s2Label->text());
  _settings.setData(PP::S3Label, _s3Label->text());
  _settings.setData(PP::R1Show, (int) _r1Check->isChecked());
  _settings.setData(PP::R2Show, (int) _r2Check->isChecked());
  _settings.setData(PP::R3Show, (int) _r3Check->isChecked());
  _settings.setData(PP::S1Show, (int) _s1Check->isChecked());
  _settings.setData(PP::S2Show, (int) _s2Check->isChecked());
  _settings.setData(PP::S3Show, (int) _s3Check->isChecked());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

