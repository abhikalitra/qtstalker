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
#include "FunctionSAR.h"
#include "FunctionMA.h"
#include "SAR.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

SARDialog::SARDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void SARDialog::createGeneralPage ()
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
  _settings.getData(SAR::Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(SAR::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  // init
  label = new QLabel(tr("Initial"));
  grid->addWidget(label, row, col++);

  _init = new QDoubleSpinBox;
  _init->setRange(0, 0.2);
  _init->setValue(_settings.getDouble(SAR::Init));
  grid->addWidget(_init, row++, col--);

  // max
  label = new QLabel(tr("Max"));
  grid->addWidget(label, row, col++);

  _max = new QDoubleSpinBox;
  _max->setRange(0, 0.2);
  _max->setValue(_settings.getDouble(SAR::Max));
  grid->addWidget(_max, row++, col--);
 
  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("SAR"));
}

void SARDialog::done ()
{
  _settings.setData(SAR::Color, _color->color().name());
  _settings.setData(SAR::Label, _label->text());
  _settings.setData(SAR::Init, _init->value());
  _settings.setData(SAR::Max, _max->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

