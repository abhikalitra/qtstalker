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

TestSettings::TestSettings ()
{
  priceList << "Open" << "High" << "Low" << "Close" << "AvgPrice" << "MedianPrice" << "TypicalPrice";
  
  BarData bd;
  bd.getBarLengthList(barLengthList);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  setLayout(vbox);

  // script box area
  QGroupBox *scriptCheck = new QGroupBox;
  scriptCheck->setCheckable(FALSE);
  scriptCheck->setTitle(tr("Run Script"));
//  scriptCheck->setChecked(FALSE);
//  connect(scriptCheck, SIGNAL(toggled(bool)), this, SLOT(scriptCheckChanged(bool)));
  vbox->addWidget(scriptCheck);

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

    label = new QLabel(tr("Comment"));
    tgrid->addWidget(label, 2, 0);

    comment = new QTextEdit;
    tgrid->addWidget(comment, 2, 1);


  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);
  int row = 0;
  int col = 0;

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

  vbox->addStretch();
  
//  grid->setRowStretch(row, 1);
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

void TestSettings::getScript (QString &d)
{
  d = script->text();
}

void TestSettings::setScript (QString &d)
{
  script->setText(d);
}

void TestSettings::getShellCommand (QString &d)
{
  d = shellCommand->text();
}

void TestSettings::setShellCommand (QString &d)
{
  shellCommand->setText(d);
}

void TestSettings::getComment (QString &d)
{
  d = comment->toPlainText();
}

void TestSettings::setComment (QString &d)
{
  comment->clear();
  comment->append(d);
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

