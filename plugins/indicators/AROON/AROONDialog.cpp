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
#include "FunctionAROON.h"
#include "AROON.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

AROONDialog::AROONDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createAROONPage();
  createOSCPage();
}

void AROONDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // method
  QLabel *label = new QLabel(tr("Method"));
  grid->addWidget(label, row, col++);

  FunctionAROON f;
  QStringList l = f.list();

  QString d;
  _settings.getData(AROON::Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  grid->addWidget(_method, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(AROON::Period));
  grid->addWidget(_period, row++, col--);

  grid->setRowStretch(row, 1);
  
  _tabs->addTab(w, tr("General"));  
}

void AROONDialog::createAROONPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // up color
  QLabel *label = new QLabel(tr("Up Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(AROON::UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  grid->addWidget(_upColor, row++, col--);


  // down color
  label = new QLabel(tr("Down Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(AROON::DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  grid->addWidget(_downColor, row++, col--);


  // up plot style
  label = new QLabel(tr("Up Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(AROON::UpPlot, d);

  _upPlotStyle = new QComboBox;
  _upPlotStyle->addItems(l);
  _upPlotStyle->setCurrentIndex(_upPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_upPlotStyle, row++, col--);

  
  // down plot style
  label = new QLabel(tr("Down Plot"));
  grid->addWidget(label, row, col++);

  _settings.getData(AROON::DownPlot, d);

  _downPlotStyle = new QComboBox;
  _downPlotStyle->addItems(l);
  _downPlotStyle->setCurrentIndex(_downPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_downPlotStyle, row++, col--);


  // up label
  label = new QLabel(tr("Up Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(AROON::UpLabel, d);

  _upLabel = new QLineEdit(d);
  grid->addWidget(_upLabel, row++, col--);


  // down label
  label = new QLabel(tr("Down Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(AROON::DownLabel, d);

  _downLabel = new QLineEdit(d);
  grid->addWidget(_downLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "AROON");
}

void AROONDialog::createOSCPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // osc color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(AROON::OSCColor, d);
  QColor c(d);

  _oscColor = new ColorButton(this, c);
  _oscColor->setColorButton();
  grid->addWidget(_oscColor, row++, col--);


  // up plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(AROON::OSCPlot, d);

  _oscPlotStyle = new QComboBox;
  _oscPlotStyle->addItems(l);
  _oscPlotStyle->setCurrentIndex(_oscPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_oscPlotStyle, row++, col--);


  // up label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(AROON::OSCLabel, d);

  _oscLabel = new QLineEdit(d);
  grid->addWidget(_oscLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "OSC");
}

void AROONDialog::done ()
{
  _settings.setData(AROON::Method, _method->currentText());
  _settings.setData(AROON::DownColor, _downColor->color().name());
  _settings.setData(AROON::UpColor, _upColor->color().name());
  _settings.setData(AROON::DownPlot, _downPlotStyle->currentText());
  _settings.setData(AROON::UpPlot, _upPlotStyle->currentText());
  _settings.setData(AROON::DownLabel, _downLabel->text());
  _settings.setData(AROON::UpLabel, _upLabel->text());
  _settings.setData(AROON::OSCColor, _oscColor->color().name());
  _settings.setData(AROON::OSCPlot, _oscPlotStyle->currentText());
  _settings.setData(AROON::OSCLabel, _oscLabel->text());
  _settings.setData(AROON::Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

