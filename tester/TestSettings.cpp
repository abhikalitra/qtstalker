/*
 *  TestSettings
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QtDebug>
#include <QIcon>
#include <QSqlQuery>
#include <QLabel>

#include "TestSettings.h"
#include "BarData.h"
#include "TestConfig.h"
#include "PluginFactory.h"

TestSettings::TestSettings ()
{
  priceList << "Open" << "High" << "Low" << "Close" << "AvgPrice" << "MedianPrice" << "TypicalPrice";
  
  TestConfig config;
  config.getBaseData(TestConfig::IndicatorPluginList, indicatorList);

  BarData bd;
  bd.getBarLengthList(barLengthList);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(5);
  hbox->setSpacing(10);
  setLayout(hbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(5);
  hbox->addLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  vbox->addLayout(grid);
  int row = 0;
  int col = 0;

  // script box area
  scriptCheck = new QGroupBox;
  scriptCheck->setCheckable(TRUE);
  scriptCheck->setTitle(tr("Run Script"));
  scriptCheck->setChecked(FALSE);
  connect(scriptCheck, SIGNAL(toggled(bool)), this, SLOT(scriptCheckChanged(bool)));
  grid->addWidget(scriptCheck, row++, col);

    QGridLayout *tgrid = new QGridLayout;
    tgrid->setSpacing(5);
    tgrid->setColumnStretch(1, 1);
    scriptCheck->setLayout(tgrid);

    QLabel *label = new QLabel(tr("Shell Command"));
    tgrid->addWidget(label, 0, 0);

    shellCommand = new QLineEdit;
    tgrid->addWidget(shellCommand, 0, 1);

    label = new QLabel(tr("Script File"));
    tgrid->addWidget(label, 1, 0);

    script = new QLineEdit;
    tgrid->addWidget(script, 1, 1);

    scriptButton = new QToolButton;
    scriptButton->setText(QString("..."));
    tgrid->addWidget(scriptButton, 1, 2);
    connect(scriptButton, SIGNAL(pressed()), this, SLOT(scriptButtonPressed()));


  // long box area
  longCheck = new QGroupBox;
  longCheck->setCheckable(TRUE);
  longCheck->setTitle(tr("Long"));
  longCheck->setChecked(FALSE);
  grid->addWidget(longCheck, row++, col);

    tgrid = new QGridLayout;
    tgrid->setSpacing(5);
    tgrid->setColumnStretch(1, 1);
    longCheck->setLayout(tgrid);

    // enter long line
    label = new QLabel(tr("Enter Long"));
    tgrid->addWidget(label, 0, 0);

    enterLongCombo = new QComboBox;
    enterLongCombo->addItems(indicatorList);
    connect(enterLongCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(enterLongComboChanged()));
    tgrid->addWidget(enterLongCombo, 0, 1);

    enterLongButton = new QToolButton;
    tgrid->addWidget(enterLongButton, 0, 2);
    enterLongButton->setText(QString("..."));
    connect(enterLongButton, SIGNAL(pressed()), this, SLOT(enterLongButtonPressed()));

    // exit long line
    label = new QLabel(tr("Exit Long"));
    tgrid->addWidget(label, 1, 0);

    exitLongCombo = new QComboBox;
    exitLongCombo->addItems(indicatorList);
    connect(exitLongCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exitLongComboChanged()));
    tgrid->addWidget(exitLongCombo, 1, 1);

    exitLongButton = new QToolButton;
    tgrid->addWidget(exitLongButton, 1, 2);
    exitLongButton->setText(QString("..."));
    connect(exitLongButton, SIGNAL(pressed()), this, SLOT(exitLongButtonPressed()));

  // short box area
  shortCheck = new QGroupBox;
  shortCheck->setCheckable(TRUE);
  shortCheck->setTitle(tr("Short"));
  shortCheck->setChecked(FALSE);
  grid->addWidget(shortCheck, row++, col);

    tgrid = new QGridLayout;
    tgrid->setSpacing(5);
    tgrid->setColumnStretch(1, 1);
    shortCheck->setLayout(tgrid);

    // enter short line
    label = new QLabel(tr("Enter Short"));
    tgrid->addWidget(label, 0, 0);

    enterShortCombo = new QComboBox;
    enterShortCombo->addItems(indicatorList);
    connect(enterShortCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(enterShortComboChanged()));
    tgrid->addWidget(enterShortCombo, 0, 1);

    enterShortButton = new QToolButton;
    tgrid->addWidget(enterShortButton, 0, 2);
    enterShortButton->setText(QString("..."));
    connect(enterShortButton, SIGNAL(pressed()), this, SLOT(enterShortButtonPressed()));

    // exit short line
    label = new QLabel(tr("Exit Short"));
    tgrid->addWidget(label, 1, 0);

    exitShortCombo = new QComboBox;
    exitShortCombo->addItems(indicatorList);
    connect(exitShortCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exitShortComboChanged()));
    tgrid->addWidget(exitShortCombo, 1, 1);

    exitShortButton = new QToolButton;
    tgrid->addWidget(exitShortButton, 1, 2);
    exitShortButton->setText(QString("..."));
    connect(exitShortButton, SIGNAL(pressed()), this, SLOT(exitShortButtonPressed()));

  vbox->addStretch(1);


  
  // right side of page
  grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  hbox->addLayout(grid);
  row = 0;
  col = 0;

  label = new QLabel(tr("Symbol"));
  grid->addWidget(label, row, col++);

  symbolButton = new SymbolButton(this);
  grid->addWidget(symbolButton, row++, col--);
  connect(symbolButton, SIGNAL(pressed()), symbolButton, SLOT(symbolDialog()));
  
  // bar length parm
  label = new QLabel(tr("Bar Length"));
  grid->addWidget(label, row, col++);

  barLength = new QComboBox;
  barLength->addItems(barLengthList);
  barLength->setCurrentIndex(6);
  grid->addWidget(barLength, row++, col--);

  // bars
  label = new QLabel(tr("Bars"));
  grid->addWidget(label, row, col++);

  bars = new QSpinBox;
  bars->setMinimum(1);
  bars->setMaximum(999999);
  bars->setValue(275);
  grid->addWidget(bars, row++, col--);

  // trade delay
  label = new QLabel(tr("Trade Delay"));
  grid->addWidget(label, row, col++);

  delay = new QSpinBox;
  delay->setMinimum(0);
  delay->setMaximum(100);
  delay->setValue(1);
  grid->addWidget(delay, row++, col--);

  // enter field parms
  label = new QLabel(tr("Enter Price"));
  grid->addWidget(label, row, col++);

  enterField = new QComboBox;
  enterField->addItems(priceList);
  grid->addWidget(enterField, row++, col--);

  // exit field parms
  label = new QLabel(tr("Exit Price"));
  grid->addWidget(label, row, col++);

  exitField = new QComboBox;
  exitField->addItems(priceList);
  grid->addWidget(exitField, row++, col--);

  // entry comm
  label = new QLabel(tr("Entry Commission"));
  grid->addWidget(label, row, col++);

  entryComm = new QDoubleSpinBox;
  entryComm->setMinimum(0);
  entryComm->setMaximum(999999);
  entryComm->setValue(10);
  grid->addWidget(entryComm, row++, col--);

  // exit comm
  label = new QLabel(tr("Exit Commission"));
  grid->addWidget(label, row, col++);

  exitComm = new QDoubleSpinBox;
  exitComm->setMinimum(0);
  exitComm->setMaximum(999999);
  exitComm->setValue(10);
  grid->addWidget(exitComm, row++, col--);

  // account
  label = new QLabel(tr("Account"));
  grid->addWidget(label, row, col++);

  account = new QDoubleSpinBox;
  account->setMinimum(0);
  account->setMaximum(99999999);
  account->setValue(10000);
  grid->addWidget(account, row++, col--);

  // volume percentage
  label = new QLabel(tr("Volume Percentage"));
  grid->addWidget(label, row, col++);

  volumePercentage = new QDoubleSpinBox;
  volumePercentage->setMinimum(0);
  volumePercentage->setMaximum(100);
  volumePercentage->setValue(20);
  grid->addWidget(volumePercentage, row++, col--);

  // trailing stop parms
  trailingCheck = new QCheckBox(tr("Trailing Stop"));
  grid->addWidget(trailingCheck, row, col++);

  trailing = new QDoubleSpinBox;
  trailing->setMinimum(0);
  trailing->setMaximum(100);
  trailing->setValue(7);
  grid->addWidget(trailing, row++, col--);

  grid->setRowStretch(row, 1);
}

TestSettings::~TestSettings ()
{
  enterLongIndicator.clear();
  exitLongIndicator.clear();
  enterShortIndicator.clear();
  exitShortIndicator.clear();
}

void TestSettings::scriptButtonPressed ()
{
  QString dir;
  TestConfig config;
  config.getData(TestConfig::ScriptDirectory, dir);
  if (dir.isEmpty())
    dir = QDir::homePath();

  QString s = QFileDialog::getOpenFileName(this,
					   QString(tr("Select Test Script")),
					   dir,
					   QString(),
					   0,
					   0);
  if (s.isEmpty())
    return;

  config.setData(TestConfig::ScriptDirectory, s);

  script->setText(s);
}

void TestSettings::scriptCheckChanged (bool d)
{
  if (! d)
  {
    longCheck->setEnabled(TRUE);
    shortCheck->setEnabled(TRUE);
  }
  else
  {
    longCheck->setEnabled(FALSE);
    shortCheck->setEnabled(FALSE);
  }
}

void TestSettings::enterLongComboChanged ()
{
  enterLongIndicator.clear();
  
  QString s = enterLongCombo->currentText();
  if (s.isEmpty())
    return;
  
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;
  
  plug->getSettings(enterLongIndicator);
  enterLongIndicator.setIndicator(s);
}

void TestSettings::exitLongComboChanged ()
{
  exitLongIndicator.clear();

  QString s = exitLongCombo->currentText();
  if (s.isEmpty())
    return;

  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;
  
  plug->getSettings(exitLongIndicator);
  exitLongIndicator.setIndicator(s);
}

void TestSettings::enterShortComboChanged ()
{
  enterShortIndicator.clear();

  QString s = enterShortCombo->currentText();
  if (s.isEmpty())
    return;
  
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;
  
  plug->getSettings(enterShortIndicator);
  enterShortIndicator.setIndicator(s);
}

void TestSettings::exitShortComboChanged ()
{
  exitShortIndicator.clear();

  QString s = exitShortCombo->currentText();
  if (s.isEmpty())
    return;
  
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;
  
  plug->getSettings(exitShortIndicator);
  exitShortIndicator.setIndicator(s);
}

void TestSettings::enterLongButtonPressed ()
{
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  QString s;
  enterLongIndicator.getIndicator(s);
  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;

  plug->setSettings(enterLongIndicator);
  plug->dialog(1);
  plug->getSettings(enterLongIndicator);
}

void TestSettings::exitLongButtonPressed ()
{
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  QString s;
  exitLongIndicator.getIndicator(s);
  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;

  plug->setSettings(exitLongIndicator);
  plug->dialog(1);
  plug->getSettings(exitLongIndicator);
}

void TestSettings::enterShortButtonPressed ()
{
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  QString s;
  enterShortIndicator.getIndicator(s);
  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;

  plug->setSettings(enterShortIndicator);
  plug->dialog(1);
  plug->getSettings(enterShortIndicator);
}

void TestSettings::exitShortButtonPressed ()
{
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  QString s;
  exitShortIndicator.getIndicator(s);
  PluginFactory fac;
  IndicatorPlugin *plug = fac.getIndicator(path, s);
  if (! plug)
    return;

  plug->setSettings(exitShortIndicator);
  plug->dialog(1);
  plug->getSettings(exitShortIndicator);
}

void TestSettings::getScript (QString &d)
{
  d = script->text();
}

void TestSettings::setScript (QString &d)
{
  script->setText(d);
}

bool TestSettings::getScriptCheck ()
{
  return scriptCheck->isChecked();
}

void TestSettings::setScriptCheck (bool d)
{
  scriptCheck->setChecked(d);
}

void TestSettings::getShellCommand (QString &d)
{
  d = shellCommand->text();
}

void TestSettings::setShellCommand (QString &d)
{
  shellCommand->setText(d);
}

bool TestSettings::getLongCheck ()
{
  return longCheck->isChecked();
}

void TestSettings::setLongCheck (bool d)
{
  longCheck->setChecked(d);
}

bool TestSettings::getShortCheck ()
{
  return shortCheck->isChecked();
}

void TestSettings::setShortCheck (bool d)
{
  shortCheck->setChecked(d);
}

void TestSettings::getSymbol (QString &d)
{
  QStringList l;
  symbolButton->getSymbols(l);
  d = l.join(",");
}

void TestSettings::setSymbol (QString &d)
{
  QStringList l;
  l = d.split(",");
  symbolButton->setSymbols(l);
}

int TestSettings::getEnterField ()
{
  return enterField->currentIndex();
}

void TestSettings::setEnterField (int d)
{
  enterField->setCurrentIndex(d);
}

int TestSettings::getExitField ()
{
  return exitField->currentIndex();
}

void TestSettings::setExitField (int d)
{
  exitField->setCurrentIndex(d);
}

int TestSettings::getBars ()
{
  return bars->value();
}

void TestSettings::setBars (int d)
{
  bars->setValue(d);
}

int TestSettings::getBarLength ()
{
  return barLength->currentIndex();
}

void TestSettings::setBarLength (int d)
{
  barLength->setCurrentIndex(d);
}

double TestSettings::getEntryComm ()
{
  return entryComm->value();
}

void TestSettings::setEntryComm (double d)
{
  entryComm->setValue(d);
}

double TestSettings::getExitComm ()
{
  return exitComm->value();
}

void TestSettings::setExitComm (double d)
{
  exitComm->setValue(d);
}

int TestSettings::getDelay ()
{
  return delay->value();
}

void TestSettings::setDelay (int d)
{
  delay->setValue(d);
}

double TestSettings::getAccount ()
{
  return account->value();
}

void TestSettings::setAccount (double d)
{
  account->setValue(d);
}

double TestSettings::getVolumePercentage ()
{
  return volumePercentage->value();
}

void TestSettings::setVolumePercentage (double d)
{
  volumePercentage->setValue(d);
}

double TestSettings::getTrailing ()
{
  return trailing->value();
}

void TestSettings::setTrailing (double d)
{
  trailing->setValue(d);
}

bool TestSettings::getTrailingCheck ()
{
  return trailingCheck->isChecked();
}

void TestSettings::setTrailingCheck (bool d)
{
  trailingCheck->setChecked(d);
}

void TestSettings::getEnterLongCombo (QString &d)
{
  d = enterLongCombo->currentText();
}

void TestSettings::setEnterLongCombo (QString &d)
{
  enterLongCombo->setCurrentIndex(indicatorList.indexOf(d));
}

void TestSettings::getExitLongCombo (QString &d)
{
  d = exitLongCombo->currentText();
}

void TestSettings::setExitLongCombo (QString &d)
{
  exitLongCombo->setCurrentIndex(indicatorList.indexOf(d));
}

void TestSettings::getEnterShortCombo (QString &d)
{
  d = enterShortCombo->currentText();
}

void TestSettings::setEnterShortCombo (QString &d)
{
  enterShortCombo->setCurrentIndex(indicatorList.indexOf(d));
}

void TestSettings::getExitShortCombo (QString &d)
{
  d = exitShortCombo->currentText();
}

void TestSettings::setExitShortCombo (QString &d)
{
  exitShortCombo->setCurrentIndex(indicatorList.indexOf(d));
}

void TestSettings::getEnterLongIndicator (Indicator &d)
{
  d = enterLongIndicator;
}

void TestSettings::getExitLongIndicator (Indicator &d)
{
  d = exitLongIndicator;
}

void TestSettings::getEnterShortIndicator (Indicator &d)
{
  d = enterShortIndicator;
}

void TestSettings::getExitShortIndicator (Indicator &d)
{
  d = exitShortIndicator;
}

void TestSettings::getEnterLongSettings (QString &d)
{
  d = " ";
  Setting set;
  enterLongIndicator.getSettings(set);
  set.getString(d);
}

void TestSettings::setEnterLongSettings (QString &d)
{
  Setting set;
  set.parse(d);
  enterLongIndicator.setSettings(set);
}

void TestSettings::getExitLongSettings (QString &d)
{
  d = " ";
  Setting set;
  exitLongIndicator.getSettings(set);
  set.getString(d);
}

void TestSettings::setExitLongSettings (QString &d)
{
  Setting set;
  set.parse(d);
  exitLongIndicator.setSettings(set);
}

void TestSettings::getEnterShortSettings (QString &d)
{
  d = " ";
  Setting set;
  enterShortIndicator.getSettings(set);
  set.getString(d);
}

void TestSettings::setEnterShortSettings (QString &d)
{
  Setting set;
  set.parse(d);
  enterShortIndicator.setSettings(set);
}

void TestSettings::getExitShortSettings (QString &d)
{
  d = " ";
  Setting set;
  exitShortIndicator.getSettings(set);
  set.getString(d);
}

void TestSettings::setExitShortSettings (QString &d)
{
  Setting set;
  set.parse(d);
  exitShortIndicator.setSettings(set);
}




