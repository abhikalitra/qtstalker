/*
 *  Qtstalker stock charter
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

#include <QLayout>
#include <QGroupBox>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include "TesterStopPage.h"
#include "IndicatorPlugin.h"


TesterStopPage::TesterStopPage (QWidget *p) : QWidget (p)
{
  customShortStopLine = 0;
  customLongStopLine = 0;

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(10);

  tabs = new QTabWidget(this);
  vbox->addWidget(tabs);

  createMaxLossPage();
  createProfitPage();
  createTrailingPage();
  createCustomPage();
}

TesterStopPage::~TesterStopPage ()
{
  if (customLongStopLine)
    delete customLongStopLine;
  
  if (customShortStopLine)
    delete customShortStopLine;
}

void TesterStopPage::createMaxLossPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
    
  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(5);
  vbox->addLayout(grid);
  
  maximumLossCheck = new QCheckBox(tr("Enabled"), w);
  connect(maximumLossCheck, SIGNAL(toggled(bool)), this, SLOT(maximumLossToggled(bool)));
  grid->addWidget(maximumLossCheck, 0, 0);

  maximumLossLong = new QCheckBox(tr("Long"), w);
  grid->addWidget(maximumLossLong, 1, 0);

  maximumLossShort = new QCheckBox(tr("Short"), w);
  grid->addWidget(maximumLossShort, 2, 0);

  validator = new QDoubleValidator(0, 999999, 2, w);

  QLabel *label = new QLabel(tr("Loss %"), w);
  grid->addWidget(label, 3, 0);

  maximumLossEdit = new QLineEdit("0", w);
  maximumLossEdit->setValidator(validator);
  grid->addWidget(maximumLossEdit, 3, 1);

  vbox->insertStretch(-1, 0);

  tabs->addTab(w, tr("Maximum Loss"));

  maximumLossToggled(FALSE);
}

void TesterStopPage::createProfitPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
    
  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(5);
  vbox->addLayout(grid);

  profitCheck = new QCheckBox(tr("Enabled"), w);
  connect(profitCheck, SIGNAL(toggled(bool)), this, SLOT(profitToggled(bool)));
  grid->addWidget(profitCheck, 0, 0);

  profitLong = new QCheckBox(tr("Long"), w);
  grid->addWidget(profitLong, 1, 0);

  profitShort = new QCheckBox(tr("Short"), w);
  grid->addWidget(profitShort, 2, 0);

  QLabel *label = new QLabel(tr("Profit %"), w);
  grid->addWidget(label, 3, 0);

  profitEdit = new QLineEdit("0", w);
  profitEdit->setValidator(validator);
  grid->addWidget(profitEdit, 3, 1);

  vbox->insertStretch(-1, 0);

  tabs->addTab(w, tr("Profit"));

  profitToggled(FALSE);
}

void TesterStopPage::createTrailingPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
    
  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(5);
  vbox->addLayout(grid);

  trailingCheck = new QCheckBox(tr("Enabled"), w);
  connect(trailingCheck, SIGNAL(toggled(bool)), this, SLOT(trailingToggled(bool)));
  grid->addWidget(trailingCheck, 0, 0);

  trailingLong = new QCheckBox(tr("Long"), w);
  grid->addWidget(trailingLong, 1, 0);

  trailingShort = new QCheckBox(tr("Short"), w);
  grid->addWidget(trailingShort, 2, 0);

  QLabel *label = new QLabel(tr("Loss %"), w);
  grid->addWidget(label, 3, 0);

  trailingEdit = new QLineEdit("0", w);
  trailingEdit->setValidator(validator);
  grid->addWidget(trailingEdit, 3, 1);

  vbox->insertStretch(-1, 0);

  tabs->addTab(w, tr("Trailing"));

  trailingToggled(FALSE);
}

void TesterStopPage::createCustomPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
    
  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(5);
  vbox->addLayout(grid);

  customLongStopCheck = new QCheckBox(tr("Custom Long"), w);
  connect(customLongStopCheck, SIGNAL(toggled(bool)), this, SLOT(customLongStopToggled(bool)));
  grid->addWidget(customLongStopCheck, 0, 0);

  QString s;
  customLongStopEdit = new FormulaEdit(w, s);  
  grid->addWidget(customLongStopEdit, 1, 0);

  customShortStopCheck = new QCheckBox(tr("Custom Short"), w);
  connect(customShortStopCheck, SIGNAL(toggled(bool)), this, SLOT(customShortStopToggled(bool)));
  grid->addWidget(customShortStopCheck, 2, 0);

  customShortStopEdit = new FormulaEdit(w, s);  
  grid->addWidget(customShortStopEdit, 3, 0);

  vbox->insertStretch(-1, 0);

  tabs->addTab(w, tr("Custom"));

  customLongStopToggled(FALSE);
  customShortStopToggled(FALSE);
}

void TesterStopPage::maximumLossToggled (bool status)
{
  if (status)
  {
    maximumLossLong->setEnabled(TRUE);
    maximumLossShort->setEnabled(TRUE);
    maximumLossEdit->setEnabled(TRUE);
  }
  else
  {
    maximumLossLong->setEnabled(FALSE);
    maximumLossShort->setEnabled(FALSE);
    maximumLossEdit->setEnabled(FALSE);
  }
}

void TesterStopPage::profitToggled (bool status)
{
  if (status)
  {
    profitLong->setEnabled(TRUE);
    profitShort->setEnabled(TRUE);
    profitEdit->setEnabled(TRUE);
  }
  else
  {
    profitLong->setEnabled(FALSE);
    profitShort->setEnabled(FALSE);
    profitEdit->setEnabled(FALSE);
  }
}

void TesterStopPage::trailingToggled (bool status)
{
  if (status)
  {
    trailingLong->setEnabled(TRUE);
    trailingShort->setEnabled(TRUE);
    trailingEdit->setEnabled(TRUE);
  }
  else
  {
    trailingLong->setEnabled(FALSE);
    trailingShort->setEnabled(FALSE);
    trailingEdit->setEnabled(FALSE);
  }
}

void TesterStopPage::customShortStopToggled (bool status)
{
  if (status)
    customShortStopEdit->setEnabled(TRUE);
  else
    customShortStopEdit->setEnabled(FALSE);
}

void TesterStopPage::customLongStopToggled (bool status)
{
  if (status)
    customLongStopEdit->setEnabled(TRUE);
  else
    customLongStopEdit->setEnabled(FALSE);
}

bool TesterStopPage::loadCustomShortStop (BarData *recordList)
{
  if (customShortStopLine)
  {
    delete customShortStopLine;
    customShortStopLine = 0;
  }
  
//  if (! customShortStopEdit->getLines() || ! customShortStopCheck->isChecked())
//    return FALSE;

  QString s;
//  customShortStopEdit->getText(s);
  QStringList l = s.split("\n");

  QString plugin("CUS");
  IndicatorPlugin plug;
//  plug->setCustomFunction(l);
  
  // load the CUS plugin and calculate
  plug.setIndicatorInput(recordList);
  QList<PlotLine *> lines;
  plug.calculate(lines);
  PlotLine *line = lines.at(0);
  if (! line)
  {
    qDebug("TesterStopPage::loadCustomShortStop: no PlotLine returned");
    return TRUE;
  }
    
  customShortStopLine = new PlotLine;
  customLongStopLine->copy(line);
    
  return FALSE;
}

bool TesterStopPage::loadCustomLongStop (BarData *recordList)
{
  if (customLongStopLine)
  {
    delete customLongStopLine;
    customLongStopLine = 0;
  }
  
//  if (! customLongStopEdit->getLines() || ! customLongStopCheck->isChecked())
//    return FALSE;

  QString s;
//  customLongStopEdit->getText(s);
  QStringList l = s.split("\n");
  
  QString plugin("CUS");
  IndicatorPlugin plug;
//  plug->setCustomFunction(l);
  
  // load the CUS plugin and calculate
  plug.setIndicatorInput(recordList);
  QList<PlotLine *> lines;
  plug.calculate(lines);
  PlotLine *line = lines.at(0);
  if (! line)
  {
    qDebug("Tester::loadCustomShortStop: no PlotLine returned");
    return TRUE;
  }
    
  customLongStopLine = new PlotLine;
  customLongStopLine->copy(line);
    
  return FALSE;
}

void TesterStopPage::loadCustomStopRule (QString &ruleName)
{
  QString s;
//  config->getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customLongStop");
  QFile f(s);
  if (! f.open(QIODevice::ReadOnly))
    return;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.trimmed();
    if (! s.length())
      continue;
  
//    customLongStopEdit->setLine(s);
  }  
  
  f.close();
  
//  config->getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customShortStop");
  f.setFileName(s);
  if (! f.open(QIODevice::ReadOnly))
    return;

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.trimmed();
    if (! s.length())
      continue;
  
//    customShortStopEdit->setLine(s);
  }  
  
  f.close();
}

void TesterStopPage::saveCustomStopRule (QString &ruleName)
{
  QString s;
//  config->getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customShortStop");
  QFile f(s);
  if (! f.open(QIODevice::WriteOnly))
    return;
  QTextStream stream(&f);

//  customShortStopEdit->getText(s);
  stream << s << "\n";
  
  f.close();
  
//  config->getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customLongStop");
  f.setFileName(s);
  if (! f.open(QIODevice::WriteOnly))
    return;

//  customLongStopEdit->getText(s);
  stream << s << "\n";
  
  f.close();
}

bool TesterStopPage::maximumLoss (bool flag, double enterPrice, double exitPrice)
{
  if (! maximumLossCheck->isChecked())
    return FALSE;

  if (! flag)
  {
    double t = enterPrice * (1 - (maximumLossEdit->text().toDouble() / 100));
    if (exitPrice <= t)
      return TRUE;
  }
  else
  {
    double t = enterPrice * (1 + (maximumLossEdit->text().toDouble() / 100));
    if (exitPrice >= t)
      return TRUE;
  }

  return FALSE;
}

bool TesterStopPage::profit (bool flag, double enterPrice, double exitPrice)
{
  if (! profitCheck->isChecked())
    return FALSE;

  if (! flag)
  {
    double t = enterPrice * (1 + (profitEdit->text().toDouble() / 100));
    if (exitPrice >= t)
      return TRUE;
  }
  else
  {
    double t = enterPrice * (1 - (profitEdit->text().toDouble() / 100));
    if (exitPrice <= t)
      return TRUE;
  }

  return FALSE;
}

bool TesterStopPage::trailing (bool flag, double exitPrice)
{
  if (! trailingCheck->isChecked())
    return FALSE;

  if (! flag)
  {
    if (exitPrice < trailingHigh)
      trailingHigh = exitPrice;

    double t = ((exitPrice - trailingHigh) / trailingHigh) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= trailingEdit->text().toDouble())
        return TRUE;
    }
  }
  else
  {
    if (exitPrice < trailingLow)
      trailingLow = exitPrice;

    double t = ((trailingLow - exitPrice) / trailingLow) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= trailingEdit->text().toDouble())
        return TRUE;
    }
  }

  return FALSE;
}

bool TesterStopPage::customStop (bool flag, int index)
{
/*
  if (! flag)
  {
    if (customLongStopCheck->isChecked() && customLongStopEdit->getLines() && customLongStopLine)
    {
      int i = customLongStopLine->getSize() - index;
      if (i > -1)
      {
        if (customLongStopLine->getData(i) == 1)
          return TRUE;
      }
    }
  }
  else
  {
    if (customShortStopCheck->isChecked() && customShortStopEdit->getLines() && customShortStopLine)
    {
      int i = customShortStopLine->getSize() - index;
      if (i > -1)
      {
        if (customShortStopLine->getData(i) == 1)
          return TRUE;
      }
    }
  }
*/
  return FALSE;
}

bool TesterStopPage::getMaximumLossCheck ()
{
  return maximumLossCheck->isChecked();
}

void TesterStopPage::setMaximumLossCheck (bool d)
{
  maximumLossCheck->setChecked(d);
}

bool TesterStopPage::getMaximumLossLong ()
{
  return maximumLossLong->isChecked();
}

void TesterStopPage::setMaximumLossLong (bool d)
{
  maximumLossLong->setChecked(d);
}

bool TesterStopPage::getMaximumLossShort ()
{
  return maximumLossShort->isChecked();
}

void TesterStopPage::setMaximumLossShort (bool d)
{
  maximumLossShort->setChecked(d);
}

QString TesterStopPage::getMaximumLossEdit ()
{
  return maximumLossEdit->text();
}

void TesterStopPage::setMaximumLossEdit (QString d)
{
  maximumLossEdit->setText(d);
}

bool TesterStopPage::getProfitCheck ()
{
  return profitCheck->isChecked();
}

void TesterStopPage::setProfitCheck (bool d)
{
  profitCheck->setChecked(d);
}

bool TesterStopPage::getProfitLong ()
{
  return profitLong->isChecked();
}

void TesterStopPage::setProfitLong (bool d)
{
  profitLong->setChecked(d);
}

bool TesterStopPage::getProfitShort ()
{
  return profitShort->isChecked();
}

void TesterStopPage::setProfitShort (bool d)
{
  profitShort->setChecked(d);
}

QString TesterStopPage::getProfitEdit ()
{
  return profitEdit->text();
}

void TesterStopPage::setProfitEdit (QString d)
{
  profitEdit->setText(d);
}

//

bool TesterStopPage::getTrailingCheck ()
{
  return trailingCheck->isChecked();
}

void TesterStopPage::setTrailingCheck (bool d)
{
  trailingCheck->setChecked(d);
}

bool TesterStopPage::getTrailingLong ()
{
  return trailingLong->isChecked();
}

void TesterStopPage::setTrailingLong (bool d)
{
  trailingLong->setChecked(d);
}

bool TesterStopPage::getTrailingShort ()
{
  return trailingShort->isChecked();
}

void TesterStopPage::setTrailingShort (bool d)
{
  trailingShort->setChecked(d);
}

QString TesterStopPage::getTrailingEdit ()
{
  return trailingEdit->text();
}

void TesterStopPage::setTrailingEdit (QString d)
{
  trailingEdit->setText(d);
}

void TesterStopPage::setTrailingHigh (double d)
{
  trailingHigh = d;
}


