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
#include "Tester.h"
#include "FuturesData.h"
#include "QSMath.h"

Tester::Tester (Config *c, QString n) : QTabDialog (0, 0, FALSE)
{
  config = c;
  ruleName = n;
  
  fieldList.append(tr("Open"));
  fieldList.append(tr("Close"));
  fieldList.append(tr("Average"));

  setCaption ("Back Tester");

  setOkButton(tr("&OK"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(exitDialog()));
  setCancelButton(tr("&Cancel"));  

  createFormulaPage();

  createStopPage();

  createTestPage();

  createReportPage();

  loadRule();
}

Tester::~Tester ()
{
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

  QLabel *label = new QLabel(tr("Delay"), gbox);
    
  delay = new QSpinBox(0, 999999, 1, gbox);
  delay->setValue(0);

  gbox = new QVGroupBox(tr("Date Range"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 0, 1);

  label = new QLabel(tr("Start Date"), gbox);

  startDate = new QDateEdit(QDate::currentDate(), gbox);
  startDate->setOrder(QDateEdit::YMD);
  startDate->setAutoAdvance(TRUE);

  label = new QLabel(tr("End Date"), gbox);

  endDate = new QDateEdit(QDate::currentDate(), gbox);
  endDate->setOrder(QDateEdit::YMD);
  endDate->setAutoAdvance(TRUE);

  gbox = new QVGroupBox(tr("Account"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 1, 0);

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

  gbox = new QVGroupBox(tr("Symbol"), w);
  gbox->setInsideSpacing(2);
  grid->addWidget(gbox, 1, 1);

  symbolButton = new SymbolButton(gbox, config->getData(Config::DataPath), QString());
  connect(symbolButton, SIGNAL(symbolChanged()), this, SLOT(symbolButtonPressed()));

  gbox = new QVGroupBox(tr("Volume"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 2, 0);

  label = new QLabel(tr("Account %"), gbox);

  volumePercent = new QSpinBox(0, 100, 1, gbox);
  
  gbox = new QVGroupBox(tr("Entry/Exit Price"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 2, 1);

  priceField = new QComboBox(gbox);
  priceField->insertStringList(fieldList,-1);

  testButton = new QPushButton(tr("Perform Test"), w);
  connect(testButton, SIGNAL(clicked()), this, SLOT(test()));
  vbox->addWidget(testButton);
  testButton->setAccel(CTRL+Key_T);

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
  header->setLabel(8, tr("Volume"), 50);
  vbox->addWidget(tradeList);
  
  // test summary

  QVGroupBox *gbox = new QVGroupBox(tr("Test Summary"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  vbox->addWidget(gbox);

  QLabel *label = new QLabel(tr("Net Profit"), gbox);
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

  BarDate sd;
  sd.setDate(startDate->date().toString("yyyyMMddmmhhss"));
  if (! sd.getDate().isValid())
    return;

  BarDate ed;
  ed.setDate(endDate->date().toString("yyyyMMddmmhhss"));
  if (! ed.getDate().isValid())
    return;

  db = new ChartDb;
  if (db->openChart(symbolButton->getPath()))
  {
    delete db;
    qDebug("Tester: Cant open db");
    return;
  }

  while (tradeList->numRows())
    tradeList->removeRow(0);

  db->setBarCompression(ChartDb::Daily);
  db->setBarRange(99999999);
  recordList = db->getHistory();

  loadAlerts(0);
  loadAlerts(1);
  loadAlerts(2);
  loadAlerts(3);

  clearAlertCounts();

  for (testLoop = 0; testLoop < (int) recordList->count(); testLoop++)
  {
    currentRecord = testLoop;
    BarDate td = recordList->getDate(currentRecord);
    if (td.getDateValue() >= sd.getDateValue())
      break;
  }

  status = 0;
  for (; testLoop < (int) recordList->count(); testLoop++)
  {
    currentRecord = testLoop;
    BarDate dt = recordList->getDate(currentRecord);
    if (dt.getDateValue() > ed.getDateValue())
      break;

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

  delete recordList;
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
  {
    FuturesData *fd = new FuturesData;
    fd->setSymbol(db->getDetail(ChartDb::FuturesType));
    profit = fd->getRate() * profit;
    delete fd;
  }

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
  
  db = new ChartDb;
  if (db->openChart(symbol))
  {
    delete db;
    qDebug("Tester: Cant open db");
    return;
  }
    
  Bar *bar = db->getFirstBar();
  startDate->setDate(bar->getDate().getDate());
  delete bar;

  bar = db->getLastBar();
  endDate->setDate(bar->getDate().getDate());
  delete bar;

  delete db;
}

void Tester::loadAlerts (int type)
{
  QStringList formulaList;
  QStringList plotList;
  int loop;
  FormulaEdit *edit = 0;
  
  switch(type)
  {
    case 0:
      edit = enterLongEdit;
      enterLongAlerts.clear();
      break;
    case 1:
      edit = exitLongEdit;
      exitLongAlerts.clear();
      break;
    case 2:
      edit = enterShortEdit;
      enterShortAlerts.clear();
      break;
    case 3:
      edit = exitShortEdit;
      exitShortAlerts.clear();
      break;
    default:
      break;
  }
  
  for (loop = 0; loop < edit->getLines(); loop++)
  {
    formulaList.append(edit->getFormula(loop));
    plotList.append(edit->getPlot(loop));
  }
  
  QSMath *t = new QSMath(recordList);
  QString err = t->calculateCustomFormula(formulaList, plotList);
  if (err.length())
  {
    qDebug("Tester::loadAlerts: " + err);
    delete t;
    return;
  }

  PlotLine *line = 0;
  for (loop = plotList.count() - 1; loop > -1; loop--)
  {
    if (plotList[loop].toInt())
    {
      line = t->getCustomLine(loop + 1);
      break;
    }
  }
  
  bool flag = FALSE;
  loop = recordList->count() - line->getSize();
  int lineLoop = 0;
  int delays = delay->value();
  for (; loop < (int) recordList->count(); loop++, lineLoop++)
  {
    if (line->getData(lineLoop) == 1)
    {
      if (! flag)
      {
        if (delays)
	{
	  int delayLoop;
	  bool delayFlag = FALSE;
	  for (delayLoop = 1; delayLoop <= delays; delayLoop++)
	  {
	    if ((lineLoop + delayLoop) < line->getSize())
	    {
	      if (line->getData(lineLoop + delayLoop) != 1)
	      {
	        delayFlag = TRUE;
	        break;
	      }
	    }
	    else
	    {
              delayFlag = TRUE;
	      break;
	    }
	  }
	  
	  if (delayFlag)
	    continue;
	}
      
        switch(type)
        {
          case 0:
            enterLongAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
            break;
          case 1:
            exitLongAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
            break;
          case 2:
            enterShortAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
            break;
          case 3:
            exitShortAlerts.setData(recordList->getDate(loop).getDateString(FALSE), "1");
            break;
          default:
            break;
        }
	
        flag = TRUE;
      }
    }
    else
      flag = FALSE;
  }
  
  if (line)
    delete line;
  delete t;
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

  s = "Volume Percent=";
  s.append(volumePercent->text());
  stream << s << "\n";
  
  s = "Entry Com=";
  s.append(entryCom->text());
  stream << s << "\n";
  
  s = "Exit Com=";
  s.append(exitCom->text());
  stream << s << "\n";
  
  s = "Delay=";
  s.append(delay->text());
  stream << s << "\n";
  
  s = "Price Field=";
  s.append(priceField->currentText());
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
    
    if (! l2[0].compare("Delay"))
    {
      delay->setValue(l2[1].toInt());
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
  if (volumePercent->value() == 0)
  {
    volume = 1;
    return;
  }

  double balance = equity;
  balance = balance * ((double) volumePercent->value() / 100.0);

  if (margin->value())
    volume = (int) (balance / margin->value());
  else
    volume = (int) (balance / getPrice(buyRecord));

  if (volume < 1)
    volume = 1;
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



