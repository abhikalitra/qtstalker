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

#include "STOCHDialog.h"
#include "FunctionSTOCH.h"
#include "FunctionMA.h"
#include "STOCH.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

STOCHDialog::STOCHDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createKPage();
  createDPage();
  createRefPage();
  createRef2Page();
}

void STOCHDialog::createKPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // period
  QLabel *label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _kperiod = new QSpinBox;
  _kperiod->setRange(1, 100000);
  _kperiod->setValue(_settings.getInt(STOCH::FastKPeriod));
  grid->addWidget(_kperiod, row++, col--);

  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(STOCH::FastKColor, d);
  QColor c(d);

  _kcolor = new ColorButton(this, c);
  _kcolor->setColorButton();
  grid->addWidget(_kcolor, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(STOCH::FastKPlot, d);

  _kplotStyle = new QComboBox;
  _kplotStyle->addItems(l);
  _kplotStyle->setCurrentIndex(_kplotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_kplotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(STOCH::FastKLabel, d);

  _klabel = new QLineEdit(d);
  grid->addWidget(_klabel, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "%K");
}

void STOCHDialog::createDPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // ma type
  QLabel *label = new QLabel(tr("MA Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  QStringList l = mau.list();

  QString d;
  _settings.getData(STOCH::FastDMA, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  grid->addWidget(_maType, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _dperiod = new QSpinBox;
  _dperiod->setRange(1, 100000);
  _dperiod->setValue(_settings.getInt(STOCH::FastDPeriod));
  grid->addWidget(_dperiod, row++, col--);

  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(STOCH::FastDColor, d);
  QColor c(d);

  _dcolor = new ColorButton(this, c);
  _dcolor->setColorButton();
  grid->addWidget(_dcolor, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(STOCH::FastDPlot, d);

  _dplotStyle = new QComboBox;
  _dplotStyle->addItems(l);
  _dplotStyle->setCurrentIndex(_dplotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_dplotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(STOCH::FastDLabel, d);

  _dlabel = new QLineEdit(d);
  grid->addWidget(_dlabel, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "%D");
}

void STOCHDialog::createRefPage ()
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
  _settings.getData(STOCH::Ref1Color, d);
  QColor c(d);

  _refColor = new ColorButton(this, c);
  _refColor->setColorButton();
  grid->addWidget(_refColor, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings.getDouble(STOCH::Ref1));
  grid->addWidget(_ref, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 1"));
}

void STOCHDialog::createRef2Page ()
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
  _settings.getData(STOCH::Ref2Color, d);
  QColor c(d);

  _ref2Color = new ColorButton(this, c);
  _ref2Color->setColorButton();
  grid->addWidget(_ref2Color, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings.getDouble(STOCH::Ref2));
  grid->addWidget(_ref2, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 2"));
}

void STOCHDialog::done ()
{
  _settings.setData(STOCH::FastKColor, _kcolor->color().name());
  _settings.setData(STOCH::FastDColor, _dcolor->color().name());
  _settings.setData(STOCH::FastKPlot, _kplotStyle->currentText());
  _settings.setData(STOCH::FastDPlot, _dplotStyle->currentText());
  _settings.setData(STOCH::FastKLabel, _klabel->text());
  _settings.setData(STOCH::FastDLabel, _dlabel->text());
  _settings.setData(STOCH::FastDPeriod, _dperiod->value());
  _settings.setData(STOCH::FastKPeriod, _kperiod->value());
  _settings.setData(STOCH::FastDMA, _maType->currentText());
  _settings.setData(STOCH::Ref1, _ref->value());
  _settings.setData(STOCH::Ref2, _ref2->value());
  _settings.setData(STOCH::Ref1Color, _refColor->color().name());
  _settings.setData(STOCH::Ref2Color, _ref2Color->color().name());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

