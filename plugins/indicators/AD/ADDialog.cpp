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

#include "ADDialog.h"
#include "AD.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

ADDialog::ADDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createADPage();
  createOSCPage();
}

void ADDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  QLabel *label = new QLabel(tr("Method"));
  grid->addWidget(label, row, col++);

  AD f;
  QStringList l = f.list();

  QString d;
  _settings.getData(AD::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  grid->addWidget(_method, row++, col--);

  grid->setRowStretch(row, 1);
  
  _tabs->addTab(w, tr("General"));  
}

void ADDialog::createADPage ()
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
  _settings.getData(AD::_ADColor, d);
  QColor c(d);

  _adColor = new ColorButton(this, c);
  _adColor->setColorButton();
  grid->addWidget(_adColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(AD::_ADPlot, d);

  _adPlotStyle = new QComboBox;
  _adPlotStyle->addItems(l);
  _adPlotStyle->setCurrentIndex(_adPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_adPlotStyle, row++, col--);

  
  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(AD::_ADLabel, d);

  _adLabel = new QLineEdit(d);
  grid->addWidget(_adLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("AD"));
}

void ADDialog::createOSCPage ()
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
  _settings.getData(AD::_OSCColor, d);
  QColor c(d);

  _oscColor = new ColorButton(this, c);
  _oscColor->setColorButton();
  grid->addWidget(_oscColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(AD::_OSCPlot, d);

  _oscPlotStyle = new QComboBox;
  _oscPlotStyle->addItems(l);
  _oscPlotStyle->setCurrentIndex(_oscPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_oscPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(AD::_OSCLabel, d);

  _oscLabel = new QLineEdit(d);
  grid->addWidget(_oscLabel, row++, col--);


  // fast
  label = new QLabel(tr("Fast Period"));
  grid->addWidget(label, row, col++);

  _fast = new QSpinBox;
  _fast->setRange(1, 100000);
  _fast->setValue(_settings.getInt(AD::_FastPeriod));
  grid->addWidget(_fast, row++, col--);
  

  // slow
  label = new QLabel(tr("Slow Period"));
  grid->addWidget(label, row, col++);

  _slow = new QSpinBox;
  _slow->setRange(1, 100000);
  _slow->setValue(_settings.getInt(AD::_SlowPeriod));
  grid->addWidget(_slow, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("OSC"));
}

void ADDialog::done ()
{
  _settings.setData(AD::_Method, _method->currentText());
  _settings.setData(AD::_ADColor, _adColor->color().name());
  _settings.setData(AD::_ADPlot, _adPlotStyle->currentText());
  _settings.setData(AD::_ADLabel, _adLabel->text());
  _settings.setData(AD::_FastPeriod, _fast->value());
  _settings.setData(AD::_SlowPeriod, _slow->value());
  _settings.setData(AD::_OSCColor, _oscColor->color().name());
  _settings.setData(AD::_OSCPlot, _oscPlotStyle->currentText());
  _settings.setData(AD::_OSCLabel, _oscLabel->text());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

