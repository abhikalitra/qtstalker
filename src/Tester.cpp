/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include <qlayout.h>
#include <qvgroupbox.h>
#include <qheader.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfont.h>
#include "Tester.h"

Tester::Tester (Config *c, QString n) : QTabDialog (0, 0, FALSE)
{
  config = c;
  ruleName = n;
  recordList = 0;
  
  fieldList.append(tr("Open"));
  fieldList.append(tr("Close"));
  fieldList.append(tr("Average"));

  setCaption ("Back Tester");

  setDefaultButton(tr("&Test"));
  connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(test()));
  
  setApplyButton(tr("&Apply"));  
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveRule()));
  
  setCancelButton(tr("&Cancel"));  
  
  setOkButton(QString::null);  

  createFormulaPage();

  createStopPage();

  createTestPage();

  createReportPage();

  createChartPage();
  
  loadRule();
}

Tester::~Tester ()
{
  if (recordList)
    delete recordList;
}

void Tester::createFormulaPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  QGridLayout *grid = new QGridLayout(vbox, 2, 2);
  
  QVGroupBox *gbox = new QVGroupBox(tr("Enter Long"), w);
  grid->addWidget(gbox, 0, 0);

  enterLongEdit = new FormulaEdit(gbox);  
  
  gbox = new QVGroupBox(tr("Exit Long"), w);
  grid->addWidget(gbox, 0, 1);

  exitLongEdit = new FormulaEdit(gbox);  

  gbox = new QVGroupBox(tr("Enter Short"), w);
  grid->addWidget(gbox, 1, 0);

  enterShortEdit = new FormulaEdit(gbox);  
  
  gbox = new QVGroupBox(tr("Exit Short"), w);
  grid->addWidget(gbox, 1, 1);

  exitShortEdit = new FormulaEdit(gbox);  
  
  addTab(w, tr("Rules"));
}

void Tester::createStopPage ()
{
  QWidget *w = new QWidget(this);

  QGridLayout *grid = new QGridLayout(w, 2, 2);
  grid->setMargin(5);
  grid->setSpacing(5);

  QVGroupBox *gbox = new QVGroupBox(tr("Maximum Loss"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 0, 0);

  maximumLossCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(maximumLossCheck, SIGNAL(toggled(bool)), this, SLOT(maximumLossToggled(bool)));
  gbox->addSpace(0);

  maximumLossLong = new QCheckBox(tr("Long"), gbox);
  gbox->addSpace(0);

  maximumLossShort = new QCheckBox(tr("Short"), gbox);
  gbox->addSpace(0);

  validator = new QDoubleValidator(0, 999999, 2, w);

  QLabel *label = new QLabel(tr("Loss %"), gbox);

  maximumLossEdit = new QLineEdit("0", gbox);
  maximumLossEdit->setValidator(validator);

  gbox = new QVGroupBox(tr("Profit"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 0, 1);

  profitCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(profitCheck, SIGNAL(toggled(bool)), this, SLOT(profitToggled(bool)));
  gbox->addSpace(0);

  profitLong = new QCheckBox(tr("Long"), gbox);
  gbox->addSpace(0);

  profitShort = new QCheckBox(tr("Short"), gbox);
  gbox->addSpace(0);

  label = new QLabel(tr("Profit %"), gbox);

  profitEdit = new QLineEdit("0", gbox);
  profitEdit->setValidator(validator);

  gbox = new QVGroupBox(tr("Trailing"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 1, 0);

  trailingCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(trailingCheck, SIGNAL(toggled(bool)), this, SLOT(trailingToggled(bool)));
  gbox->addSpace(0);

  trailingLong = new QCheckBox(tr("Long"), gbox);
  gbox->addSpace(0);

  trailingShort = new QCheckBox(tr("Short"), gbox);
  gbox->addSpace(0);

  label = new QLabel(tr("Loss %"), gbox);

  trailingEdit = new QLineEdit("0", gbox);
  trailingEdit->setValidator(validator);

  maximumLossToggled(FALSE);
  profitToggled(FALSE);
  trailingToggled(FALSE);

  addTab(w, tr("Stops"));
}

void Tester::createTestPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  QGridLayout *grid = new QGridLayout(vbox);
  grid->setSpacing(5);

  QVGroupBox *gbox = new QVGroupBox(tr("Trades"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 0, 0);

  tradeLong = new QCheckBox(tr("Long"), gbox);
  gbox->addSpace(0);

  tradeShort = new QCheckBox(tr("Short"), gbox);
  gbox->addSpace(0);

  QLabel *label = new QLabel(tr("Enter Long Delay"), gbox);
    
  enterLongDelay = new QSpinBox(0, 999999, 1, gbox);
  enterLongDelay->setValue(0);

  label = new QLabel(tr("Exit Long Delay"), gbox);
    
  exitLongDelay = new QSpinBox(0, 999999, 1, gbox);
  exitLongDelay->setValue(0);
  
  label = new QLabel(tr("Enter Short Delay"), gbox);
    
  enterShortDelay = new QSpinBox(0, 999999, 1, gbox);
  enterShortDelay->setValue(0);
  
  label = new QLabel(tr("Exit Short Delay"), gbox);
    
  exitShortDelay = new QSpinBox(0, 999999, 1, gbox);
  exitShortDelay->setValue(0);
  
  gbox = new QVGroupBox(tr("Account"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 0, 1);

  label = new QLabel(tr("Entry Commission"), gbox);

  entryCom = new QSpinBox(0, 999999, 1, gbox);
  entryCom->setValue(10);

  label = new QLabel(tr("Exit Commission"), gbox);

  exitCom = new QSpinBox(0, 999999, 1, gbox);
  exitCom->setValue(10);

  label = new QLabel(tr("Account Balance"), gbox);

  account = new QSpinBox(0, 999999, 1, gbox);
  account->setValue(10000);
  
  label = new QLabel(tr("Futures Margin"), gbox);

  margin = new QSpinBox(0, 999999, 1, gbox);
  
  label = new QLabel(tr("Volume %"), gbox);

  volumePercent = new QSpinBox(0, 100, 1, gbox);
  
  gbox = new QVGroupBox(tr("Testing"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 1, 0);
  
  label = new QLabel(tr("Symbol"), gbox);
  
  symbolButton = new SymbolButton(gbox, config->getData(Config::DataPath), QString());
  connect(symbolButton, SIGNAL(symbolChanged()), this, SLOT(symbolButtonPressed()));
  
  label = new QLabel(tr("Bars"), gbox);
  
  bars = new QSpinBox(1, 99999999, 1, gbox);
  bars->setValue(275);

  label = new QLabel(tr("Entry/Exit Price"), gbox);
  
  priceField = new QComboBox(gbox);
  priceField->insertStringList(fieldList,-1);

  addTab(w, tr("Testing"));
}

void Tester::createReportPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  tradeList = new QTable(0, 9, w);
  tradeList->setSelectionMode(QTable::Single);
  tradeList->setSorting(FALSE);
  QHeader *header = tradeList->horizontalHeader();
  header->setLabel(0, tr("Type"), 40);
  header->setLabel(1, tr("Entry"), 70);
  header->setLabel(2, tr("Entry Price"), 70);
  header->setLabel(3, tr("Exit"), 70);
  header->setLabel(4, tr("Exit Price"), 70);
  header->setLabel(5, tr("Signal"), 100);
  header->setLabel(6, tr("Profit"), 90);
  header->setLabel(7, tr("Account"), 90);
  header->setLabel(8, tr("Vol"), 50);
  vbox->addWidget(tradeList);
  
  // test summary

  QVGroupBox *gbox = new QVGroupBox(tr("Test Summary"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  vbox->addWidget(gbox);

  QLabel *label = new QLabel(tr("Account Balance"), gbox);
  summaryBalance = new QLabel(" ", gbox);
  
  label = new QLabel(tr("Net Profit"), gbox);
  summaryNetProfit = new QLabel(" ", gbox);

  label = new QLabel(tr("Net Profit %"), gbox);
  summaryNetPercentage = new QLabel(" ", gbox);

  label = new QLabel(tr("Initial Investment"), gbox);
  summaryInvestment = new QLabel(" ", gbox);

  label = new QLabel(tr("Commissions"), gbox);
  summaryCommission = new QLabel(" ", gbox);

  label = new QLabel(tr("Largest Drawdown"), gbox);
  summaryDrawdown = new QLabel(" ", gbox);

  label = new QLabel(tr("Trades"), gbox);
  summaryTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Long Trades"), gbox);
  summaryLongTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Short Trades"), gbox);
  summaryShortTrades = new QLabel(" ", gbox);
  
  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(5);

  // win summary

  gbox = new QVGroupBox(tr("Winning Trades Summary"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  label = new QLabel(tr("Trades"), gbox);
  summaryWinTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Profit"), gbox);
  summaryTotalWinTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Average"), gbox);
  summaryAverageWin = new QLabel(" ", gbox);

  label = new QLabel(tr("Largest"), gbox);
  summaryLargestWin = new QLabel(" ", gbox);

  label = new QLabel(tr("Long Trades"), gbox);
  summaryWinLongTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Short Trades"), gbox);
  summaryWinShortTrades = new QLabel(" ", gbox);

  // lose summary

  gbox = new QVGroupBox(tr("Losing Trades Summary"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  label = new QLabel(tr("Trades"), gbox);
  summaryLoseTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Profit"), gbox);
  summaryTotalLoseTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Average"), gbox);
  summaryAverageLose = new QLabel(" ", gbox);

  label = new QLabel(tr("Largest"), gbox);
  summaryLargestLose = new QLabel(" ", gbox);

  label = new QLabel(tr("Long Trades"), gbox);
  summaryLoseLongTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Short Trades"), gbox);
  summaryLoseShortTrades = new QLabel(" ", gbox);

  addTab(w, tr("Reports"));
}

void Tester::createChartPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  equityPlot = new Plot (w);
  equityPlot->setGridFlag(FALSE);
  equityPlot->setScaleToScreen(FALSE);
  equityPlot->setPixelspace(5);
  equityPlot->setIndex(0);
  equityPlot->setDateFlag(FALSE);
  equityPlot->setMainFlag(FALSE);
  equityPlot->setInfoFlag(FALSE);
  QObject::connect(this, SIGNAL(signalIndex(int)), equityPlot, SLOT(setIndex(int)));
  QStringList l = QStringList::split(" ", config->getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  equityPlot->setPlotFont(font);
  vbox->addWidget(equityPlot);
  equityPlot->setMinimumHeight(150);
  equityPlot->setMaximumHeight(150);
  
  plot = new Plot (w);
  plot->setGridFlag(FALSE);
  plot->setScaleToScreen(FALSE);
  plot->setPixelspace(5);
  plot->setIndex(0);
  plot->setDateFlag(TRUE);
  plot->setMainFlag(FALSE);
  plot->setInfoFlag(FALSE);
  QObject::connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  plot->setPlotFont(font);
  vbox->addWidget(plot);
  
  slider = new QSlider(w);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderChanged(int)));
  vbox->addWidget(slider);
  
  addTab(w, tr("Chart"));
}

void Tester::slotSliderChanged (int v)
{
  emit signalIndex(v);
  plot->draw();
  equityPlot->draw();
}

void Tester::test ()
{
  if (! tradeLong->isChecked() && ! tradeShort->isChecked())
    return;

  equity = account->text().toDouble();
  if (equity == 0)
    return;

  QString symbol = symbolButton->getSymbol();
  if (! symbol.length())
    return;

  db = new ChartDb;
  if (db->openChart(symbolButton->getPath()))
  {
    delete db;
    qDebug("Tester: Cant open db");
    return;
  }
  
  QString s = db->getDetail(ChartDb::Type);
  if (! s.compare(tr("Futures")))
    fd.setSymbol(db->getDetail(ChartDb::FuturesType));

  while (tradeList->numRows())
    tradeList->removeRow(0);

  db->setBarCompression(ChartDb::Daily);
  db->setBarRange(bars->value());
  if (recordList)
    delete recordList;
  recordList = db->getHistory();

  plot->clear();
  equityPlot->clear();
    
  equityCurve = new PlotLine;
  equityCurve->setColor(QString("green"));

  loadAlerts(0);
  loadAlerts(1);
  loadAlerts(2);
  loadAlerts(3);

  clearAlertCounts();

  status = 0;
  testLoop = 0;
  for (; testLoop < (int) recordList->count(); testLoop++)
  {
    updateEquityCurve();
    
    currentRecord = testLoop;
    BarDate dt = recordList->getDate(currentRecord);
    
    checkAlerts();

    if (status == 0)
    {
      if (enterLongAlerts.count() && tradeLong->isChecked())
      {
        if (enterLongCount)
        {
          enterLong();
          continue;
        }
      }

      if (enterShortAlerts.count() && tradeShort->isChecked())
      {
        if (enterShortCount)
        {
          enterShort();
          continue;
        }
      }
    }

    if (status == 1)
    {
      if (enterShortAlerts.count() && tradeShort->isChecked())
      {
        if (enterShortCount)
        {
          enterShort();
          continue;
        }
      }

      if (exitLongAlerts.count() && tradeLong->isChecked())
      {
        if (exitLongCount)
        {
          exitLong();
          continue;
        }
      }
    }
    
    if (status == -1)
    {
      if (enterLongAlerts.count() && tradeLong->isChecked())
      {
        if (enterLongCount)
        {
          enterLong();
          continue;
        }
      }

      if (exitShortAlerts.count() && tradeShort->isChecked())
      {
        if (exitShortCount)
        {
          exitShort();
          continue;
        }
      }
    }
    
    if (status == 0)
      continue;

    if (maximumLoss())
      continue;

    if (profit())
      continue;

    trailing();
  }

  if (status != 0)
  {
    exitPosition("End of test");
    status = 0;
  }

  createSummary();
  
  updateChart();

  createEquityCurve();
  
  delete db;
}

void Tester::checkAlerts ()
{
  QString s;
  QString key = recordList->getDate(currentRecord).getDateString(FALSE);

  switch (status)
  {
    case 0:
      s = enterLongAlerts.getData(key);
      if (s.length())
        enterLongCount++;
      else
      {
        s = enterShortAlerts.getData(key);
        if (s.length())
          enterShortCount++;
      }
      break;
    case 1:
      s = exitLongAlerts.getData(key);
      if (s.length())
        exitLongCount++;
      else
      {
        s = enterShortAlerts.getData(key);
        if (s.length())
          enterShortCount++;
      }
      break;
    case -1:
      s = exitShortAlerts.getData(key);
      if (s.length())
        exitShortCount++;
      else
      {
        s = enterLongAlerts.getData(key);
        if (s.length())
          enterLongCount++;
      }
      break;
    default:
      break;
  }
}

void Tester::clearAlertCounts ()
{
  enterLongCount = 0;
  exitLongCount = 0;
  enterShortCount = 0;
  exitShortCount = 0;
}

void Tester::enterLong ()
{
  if (status != 0)
    exitPosition("Enter Long");

  status = 1;
  buyRecord = currentRecord;
  trailingHigh = getPrice(buyRecord);
  equity = equity - entryCom->value();
  getVolume();

  clearAlertCounts();
}

void Tester::exitLong ()
{
  exitPosition("Exit Long");
  status = 0;
  clearAlertCounts();
}

void Tester::enterShort ()
{
  if (status != 0)
    exitPosition("Enter Short");

  status = -1;
  buyRecord = currentRecord;
  trailingLow = getPrice(buyRecord);
  equity = equity - entryCom->value();
  getVolume();

  clearAlertCounts();
}

void Tester::exitShort ()
{
  exitPosition("Exit Short");
  status = 0;
  clearAlertCounts();
}

void Tester::exitPosition (QString signal)
{
  double enterPrice = getPrice(buyRecord);
  double exitPrice = getPrice(currentRecord);
  double profit = 0;
  QString type;

  if (status == -1)
  {
    profit = (enterPrice - exitPrice) * volume;
    type = tr("Short");
  }
  else
  {
    profit = (exitPrice - enterPrice) * volume;
    type = tr("Long");
  }

  QString chartType = db->getDetail(ChartDb::Type);
  if (! chartType.compare(tr("Futures")))
    profit = fd.getRate() * profit;

  equity = equity + profit;

  equity = equity - exitCom->value();

  tradeList->setNumRows(tradeList->numRows() + 1);
  tradeList->setText(tradeList->numRows() - 1, 0, type);
  tradeList->setText(tradeList->numRows() - 1, 1, recordList->getDate(buyRecord).getDateString(FALSE));
  tradeList->setText(tradeList->numRows() - 1, 2, QString::number(enterPrice));
  tradeList->setText(tradeList->numRows() - 1, 3, recordList->getDate(currentRecord).getDateString(FALSE));
  tradeList->setText(tradeList->numRows() - 1, 4, QString::number(exitPrice));
  tradeList->setText(tradeList->numRows() - 1, 5, signal);
  tradeList->setText(tradeList->numRows() - 1, 6, QString::number(profit));
  tradeList->setText(tradeList->numRows() - 1, 7, QString::number(equity));
  tradeList->setText(tradeList->numRows() - 1, 8, QString::number(volume));
}

void Tester::updateEquityCurve ()
{
  if (status == 0)
  {
    equityCurve->append(equity);
    return;
  }
  
  double enterPrice = getPrice(buyRecord);
  double exitPrice = getPrice(currentRecord);
  double profit = 0;
  double bal = equity;
  QString type;

  if (status == -1)
  {
    profit = (enterPrice - exitPrice) * volume;
    type = tr("Short");
  }
  else
  {
    profit = (exitPrice - enterPrice) * volume;
    type = tr("Long");
  }

  QString chartType = db->getDetail(ChartDb::Type);
  if (! chartType.compare(tr("Futures")))
    profit = fd.getRate() * profit;

  bal = bal + profit;

  equityCurve->append(bal);
}

bool Tester::maximumLoss ()
{
  if (! maximumLossCheck->isChecked())
    return FALSE;

  double enterPrice = getPrice(buyRecord);
  double exitPrice = getPrice(currentRecord);
    
  if ((status == 1) && (maximumLossLong->isChecked()))
  {
    double t = ((exitPrice - enterPrice) / enterPrice) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= maximumLossEdit->text().toDouble())
      {
        exitPosition("Maximum Loss");
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  if ((status == -1) && (maximumLossShort->isChecked()))
  {
    double t = ((enterPrice - exitPrice) / enterPrice) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= maximumLossEdit->text().toDouble())
      {
        exitPosition("Maximum Loss");
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  return FALSE;
}

bool Tester::profit ()
{
  if (! profitCheck->isChecked())
    return FALSE;

  double enterPrice = getPrice(buyRecord);
  double exitPrice = getPrice(currentRecord);
    
  if ((status == 1) && (profitLong->isChecked()))
  {
    double t = ((exitPrice - enterPrice) / enterPrice) * 100;
    if (t > 0)
    {
      if (t >= profitEdit->text().toDouble())
      {
        exitPosition("Profit");
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  if ((status == -1) && (profitShort->isChecked()))
  {
    double t = ((enterPrice - exitPrice) / enterPrice) * 100;
    if (t > 0)
    {
      if (t >= profitEdit->text().toDouble())
      {
        exitPosition("Profit");
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  return FALSE;
}

bool Tester::trailing ()
{
  if (! trailingCheck->isChecked())
    return FALSE;

  double exitPrice = getPrice(currentRecord);
    
  if ((status == 1) && (trailingLong->isChecked()))
  {
    if (exitPrice > trailingHigh)
      trailingHigh = exitPrice;

    double t = ((exitPrice - trailingHigh) / trailingHigh) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= trailingEdit->text().toDouble())
      {
        exitPosition("Trailing");
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  if ((status == -1) && (trailingShort->isChecked()))
  {
    if (exitPrice < trailingLow)
      trailingLow = exitPrice;

    double t = ((trailingLow - exitPrice) / trailingLow) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= trailingEdit->text().toDouble())
      {
        exitPosition("Trailing");
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  return FALSE;
}

void Tester::maximumLossToggled (bool status)
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

void Tester::profitToggled (bool status)
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

void Tester::trailingToggled (bool status)
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

void Tester::symbolButtonPressed ()
{
  QString symbol = symbolButton->getPath();
}

void Tester::loadAlerts (int type)
{
  int loop;
  FormulaEdit *edit = 0;
  int delays = 0;
  
  switch(type)
  {
    case 0:
      edit = enterLongEdit;
      enterLongAlerts.clear();
      delays = enterLongDelay->value();
      break;
    case 1:
      edit = exitLongEdit;
      exitLongAlerts.clear();
      delays = exitLongDelay->value();
      break;
    case 2:
      edit = enterShortEdit;
      enterShortAlerts.clear();
      delays = enterShortDelay->value();
      break;
    case 3:
      edit = exitShortEdit;
      exitShortAlerts.clear();
      delays = exitShortDelay->value();
      break;
    default:
      break;
  }

  // open the CUS plugin   
  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, "CUS");
  if (! plug)
  {
    config->closePlugin("CUS");
    return;
  }

  QStringList l;
  for (loop = 0; loop < edit->getLines(); loop++)
    l.append(edit->getLine(loop));
  
  // load the CUS plugin and calculate
  plug->setCustomFunction(l.join("|"));
  plug->setIndicatorInput(recordList);
  plug->calculate();
  PlotLine *tline = plug->getIndicatorLine(0);
  if (! tline)
  {
    qDebug("Tester::loadAlerts: no PlotLine returned");
    config->closePlugin("CUS");
    return;
  }
    
  PlotLine *line = new PlotLine;
  line->copy(tline);
  config->closePlugin("CUS");
  
  loop = recordList->count() - line->getSize();
  int lineLoop = 0;
  int delayCount = 0;
  for (; loop < (int) recordList->count(); loop++, lineLoop++)
  {
    if (line->getData(lineLoop) == 1)
    {
      switch(type)
      {
        case 0:
          if (delays)
	  {
	    delayCount++;
	    if (delayCount > delays)
	    {
              enterLongAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
	      delayCount = 0;
	    }
	  }
	  else
            enterLongAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
          break;
        case 1:
	  if (delays)
	  {
	    delayCount++;
	    if (delayCount > delays)
	    {
              exitLongAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
	      delayCount = 0;
            }
	  }
	  else
            exitLongAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
          break;
        case 2:
	  if (delays)
	  {
	    delayCount++;
	    if (delayCount > delays)
	    {
              enterShortAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
	      delayCount = 0;
	    }
	  }
	  else
            enterShortAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
          break;
        case 3:
	  if (delays)
	  {
	    delayCount++;
	    if (delayCount > delays)
	    {
              exitShortAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
	      delayCount = 0;
	    }
	  }
	  else
            exitShortAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
          break;
        default:
          break;
      }
    }
  }
  
  if (line)
    delete line;
}

void Tester::saveEditRule (int type)
{
  FormulaEdit *edit = 0;
  QString s = config->getData(Config::TestPath);
  s.append("/");
  s.append(ruleName);
  
  switch(type)
  {
    case 0:
      edit = enterLongEdit;
      s.append("/el/rule");
      break;
    case 1:
      edit = exitLongEdit;
      s.append("/xl/rule");
      break;
    case 2:
      edit = enterShortEdit;
      s.append("/es/rule");
      break;
    case 3:
      edit = exitShortEdit;
      s.append("/xs/rule");
      break;
    default:
      break;
  }

  QFile f(s);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);

  int loop;
  for (loop = 0; loop < edit->getLines(); loop++)
    stream << edit->getLine(loop) << "\n";
  
  f.close();
}

void Tester::saveRule ()
{
  saveEditRule(0);
  saveEditRule(1);
  saveEditRule(2);
  saveEditRule(3);

  QString s = config->getData(Config::TestPath);
  s.append("/");
  s.append(ruleName);
  s.append("/");
  s.append(ruleName);

  QFile f(s);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);
  
  // save max loss stop
  if (maximumLossCheck->isChecked())
    s = "Maximum Loss Check=True";
  else
    s = "Maximum Loss Check=False";
  stream << s << "\n";

  if (maximumLossLong->isChecked())
    s = "Maximum Loss Long=True";
  else
    s = "Maximum Loss Long=False";
  stream << s << "\n";

  if (maximumLossShort->isChecked())
    s = "Maximum Loss Short=True";
  else
    s = "Maximum Loss Short=False";
  stream << s << "\n";

  s = "Maximum Loss Edit=";
  s.append(maximumLossEdit->text());
  stream << s << "\n";

  // save profit stop
  if (profitCheck->isChecked())
    s = "Profit Check=True";
  else
    s = "Profit Check=False";
  stream << s << "\n";

  if (profitLong->isChecked())
    s = "Profit Long=True";
  else
    s = "Profit Long=False";
  stream << s << "\n";

  if (profitShort->isChecked())
    s = "Profit Short=True";
  else
    s = "Profit Short=False";
  stream << s << "\n";

  s = "Profit Edit=";
  s.append(profitEdit->text());
  stream << s << "\n";

  // save trailing stop
  if (trailingCheck->isChecked())
    s = "Trailing Check=True";
  else
    s = "Trailing Check=False";
  stream << s << "\n";

  if (trailingLong->isChecked())
    s = "Trailing Long=True";
  else
    s = "Trailing Long=False";
  stream << s << "\n";

  if (trailingShort->isChecked())
    s = "Trailing Short=True";
  else
    s = "Trailing Short=False";
  stream << s << "\n";

  s = "Trailing Edit=";
  s.append(trailingEdit->text());
  stream << s << "\n";
  
  if (tradeLong->isChecked())
    s = "TradeLong=True";
  else
    s = "TradeLong=False";
  stream << s << "\n";
  
  if (tradeShort->isChecked())
    s = "TradeShort=True";
  else
    s = "TradeShort=False";
  stream << s << "\n";

  s = "Volume Percent=";
  s.append(volumePercent->text());
  stream << s << "\n";
  
  s = "Entry Com=";
  s.append(entryCom->text());
  stream << s << "\n";
  
  s = "Exit Com=";
  s.append(exitCom->text());
  stream << s << "\n";
  
  s = "EnterLongDelay=";
  s.append(enterLongDelay->text());
  stream << s << "\n";
  
  s = "ExitLongDelay=";
  s.append(exitLongDelay->text());
  stream << s << "\n";
  
  s = "EnterShortDelay=";
  s.append(enterShortDelay->text());
  stream << s << "\n";
  
  s = "ExitShortDelay=";
  s.append(exitShortDelay->text());
  stream << s << "\n";
  
  s = "Price Field=";
  s.append(priceField->currentText());
  stream << s << "\n";
  
  s = "Bars=";
  s.append(bars->text());
  stream << s << "\n";
  
  f.close();
}

void Tester::loadRule ()
{
  loadEditRule(0);
  loadEditRule(1);
  loadEditRule(2);
  loadEditRule(3);

  QString s = config->getData(Config::TestPath);
  s.append("/");
  s.append(ruleName);
  s.append("/");
  s.append(ruleName);

  QFile f(s);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();

    if (! s.length())
      continue;

    QStringList l2 = QStringList::split("=", s, FALSE);
    
    if (! l2[0].compare("Maximum Loss Check"))
    {
      if (! l2[1].compare("True"))
        maximumLossCheck->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Maximum Loss Long"))
    {
      if (! l2[1].compare("True"))
        maximumLossLong->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Maximum Loss Short"))
    {
      if (! l2[1].compare("True"))
        maximumLossShort->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Maximum Loss Edit"))
    {
      maximumLossEdit->setText(l2[1]);
      continue;
    }

    if (! l2[0].compare("Profit Check"))
    {
      if (! l2[1].compare("True"))
        profitCheck->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Profit Long"))
    {
      if (! l2[1].compare("True"))
        profitLong->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Profit Short"))
    {
      if (! l2[1].compare("True"))
        profitShort->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Profit Edit"))
    {
      profitEdit->setText(l2[1]);
      continue;
    }

    if (! l2[0].compare("Trailing Check"))
    {
      if (! l2[1].compare("True"))
        trailingCheck->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Trailing Long"))
    {
      if (! l2[1].compare("True"))
        trailingLong->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Trailing Short"))
    {
      if (! l2[1].compare("True"))
        trailingShort->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Trailing Edit"))
    {
      trailingEdit->setText(l2[1]);
      continue;
    }
    
    if (! l2[0].compare("TradeLong"))
    {
      if (! l2[1].compare("True"))
        tradeLong->setChecked(TRUE);
      continue;
    }
    
    if (! l2[0].compare("TradeShort"))
    {
      if (! l2[1].compare("True"))
        tradeShort->setChecked(TRUE);
      continue;
    }
    
    if (! l2[0].compare("Volume Percent"))
    {
      volumePercent->setValue(l2[1].toInt());
      continue;
    }
  
    if (! l2[0].compare("Entry Com"))
    {
      entryCom->setValue(l2[1].toInt());
      continue;
    }
  
    if (! l2[0].compare("Exit Com"))
    {
      exitCom->setValue(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("EnterLongDelay"))
    {
      enterLongDelay->setValue(l2[1].toInt());
      continue;
    }
  
    if (! l2[0].compare("ExitLongDelay"))
    {
      exitLongDelay->setValue(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("EnterShortDelay"))
    {
      enterShortDelay->setValue(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("ExitShortDelay"))
    {
      exitShortDelay->setValue(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("Bars"))
    {
      bars->setValue(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("Price Field"))
    {
      priceField->setCurrentText(l2[1]);
      continue;
    }
  }
  
  f.close();
}

void Tester::loadEditRule (int type)
{
  FormulaEdit *edit = 0;
  QString s = config->getData(Config::TestPath);
  s.append("/");
  s.append(ruleName);
  
  switch(type)
  {
    case 0:
      edit = enterLongEdit;
      s.append("/el/rule");
      break;
    case 1:
      edit = exitLongEdit;
      s.append("/xl/rule");
      break;
    case 2:
      edit = enterShortEdit;
      s.append("/es/rule");
      break;
    case 3:
      edit = exitShortEdit;
      s.append("/xs/rule");
      break;
    default:
      break;
  }

  QFile f(s);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
  
    edit->setLine(s);
  }  
  
  f.close();
}

void Tester::exitDialog ()
{
  saveRule();
  accept();
}

void Tester::createSummary ()
{
  int shortTrades = 0;
  int longTrades = 0;
  int winLongTrades = 0;
  int loseLongTrades = 0;
  int winShortTrades = 0;
  int loseShortTrades = 0;
  double totalWinLongTrades = 0;
  double totalLoseLongTrades = 0;
  double totalWinShortTrades = 0;
  double totalLoseShortTrades = 0;
  double largestWin = 0;
  double largestLose = 0;
  double accountDrawdown = account->value();
  double commission = (tradeList->numRows() * entryCom->value()) + (tradeList->numRows() * exitCom->value());
  double balance = 0;

  int loop;
  for (loop = 0; loop < tradeList->numRows(); loop++)
  {
    // get long/short trades
    QString s = tradeList->text(loop, 0);
    if (! s.compare(tr("Long")))
    {
      longTrades++;

      s = tradeList->text(loop, 6);
      if (s.contains("-"))
      {
        loseLongTrades++;
	totalLoseLongTrades = totalLoseLongTrades + s.toDouble();

	if (s.toDouble() < largestLose)
	  largestLose = s.toDouble();
      }
      else
      {
        winLongTrades++;
	totalWinLongTrades = totalWinLongTrades + s.toDouble();

	if (s.toDouble() > largestWin)
	  largestWin = s.toDouble();
      }
    }
    else
    {
      shortTrades++;

      s = tradeList->text(loop, 6);
      if (s.contains("-"))
      {
        loseShortTrades++;
      	totalLoseShortTrades = totalLoseShortTrades + s.toDouble();

	if (s.toDouble() < largestLose)
	  largestLose = s.toDouble();
      }
      else
      {
        winShortTrades++;
      	totalWinShortTrades = totalWinShortTrades + s.toDouble();

	if (s.toDouble() > largestWin)
	  largestWin = s.toDouble();
      }
    }

    s = tradeList->text(loop, 7);
    if (s.toDouble() < accountDrawdown)
      accountDrawdown = s.toDouble();

    balance = s.toDouble();
  }

  // main summary
  summaryBalance->setNum(balance);
  summaryNetProfit->setNum(balance - account->value());
  summaryNetPercentage->setNum(((balance - account->value()) / account->value()) * 100);
  summaryInvestment->setNum(account->value());
  summaryCommission->setNum(commission);
  summaryDrawdown->setNum(accountDrawdown - account->value());
  summaryTrades->setNum(longTrades + shortTrades);
  summaryLongTrades->setNum(longTrades);
  summaryShortTrades->setNum(shortTrades);

  // win summary
  summaryWinTrades->setNum(winLongTrades + winShortTrades);
  summaryTotalWinTrades->setNum(totalWinLongTrades + totalWinShortTrades);
  summaryAverageWin->setNum((totalWinLongTrades + totalWinShortTrades) / (winLongTrades + winShortTrades));
  summaryLargestWin->setNum(largestWin);
  summaryWinLongTrades->setNum(winLongTrades);
  summaryWinShortTrades->setNum(winShortTrades);

  // lose summary
  summaryLoseTrades->setNum(loseLongTrades + loseShortTrades);
  summaryTotalLoseTrades->setNum(totalLoseLongTrades + totalLoseShortTrades);
  summaryAverageLose->setNum((totalLoseLongTrades + totalLoseShortTrades) / (loseLongTrades + loseShortTrades));
  summaryLargestLose->setNum(largestLose);
  summaryLoseLongTrades->setNum(loseLongTrades);
  summaryLoseShortTrades->setNum(loseShortTrades);
}

void Tester::getVolume ()
{
  double balance = equity;
  if (volumePercent->value() == 0)
  {
//    volume = 1;
    balance = account->text().toDouble();
    volume = (balance / getPrice(buyRecord));
    return;
  }

  balance = balance * ((double) volumePercent->value() / 100.0);

  if (margin->value())
    volume = (double) (balance / margin->value());
  else
    volume = (double) (balance / getPrice(buyRecord));

//  if (volume < 1)
//    volume = 1;
}

double Tester::getPrice (int i)
{
  double price = 0;
  
  if (! priceField->currentText().compare(tr("Open")))
    price = recordList->getOpen(i);
  else
  {
    if (! priceField->currentText().compare(tr("Close")))
      price = recordList->getClose(i);
    else
      price = recordList->getAverage(i);
  }
  
  return price;
}

void Tester::updateChart ()
{
  plot->setData(recordList);
  
  //set up indicator
  PlotLine *close = recordList->getInput(BarData::Close);
  close->setColor(QString("green"));
  
  Indicator *i = new Indicator;
  i->setName("tester");
  i->addLine(close);
  i->setMainPlot(FALSE);
  plot->addIndicator(i->getName(), i);

  //set up arrows
  int loop;
  int name = 0;
  bool flag1 = FALSE;
  bool flag2 = FALSE;
  double enter = 0;
  double exit = 0;
  for (loop = 0; loop < (int) tradeList->numRows(); loop++)
  {
    if (! tradeList->text(loop, 0).compare(tr("Short")))
    {
      flag1 = TRUE;
      flag2 = FALSE;
      enter = tradeList->text(loop, 2).toDouble() * 1.005;
      exit = tradeList->text(loop, 4).toDouble() * 0.995;
    }
    else
    {
      flag1 = FALSE;
      flag2 = TRUE;
      enter = tradeList->text(loop, 2).toDouble() * 0.995;
      exit = tradeList->text(loop, 4).toDouble() * 1.005;
    }
    
    name++;
    BarDate dt;
    dt.setDate(tradeList->text(loop, 1) + "000000");
    plot->addArrow(flag1, "tester", QString::number(name), dt, enter);
      
    name++;
    dt.setDate(tradeList->text(loop, 3) + "000000");
    plot->addArrow(flag2, "tester", QString::number(name), dt, exit);
  }

  slider->setMaxValue(recordList->count() - 1);
        
  plot->draw();
}

void Tester::createEquityCurve ()
{
  equityPlot->setData(recordList);
  
  //set up indicator
  Indicator *i = new Indicator;
  i->setName("equity");
  i->addLine(equityCurve);
  i->setMainPlot(FALSE);
  equityPlot->addIndicator(i->getName(), i);

  equityPlot->draw();
}

