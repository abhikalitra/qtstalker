/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qdir.h>
#include <qprogressdialog.h>
#include "Tester.h"
#include "DbPlugin.h"
#include "IndicatorPlugin.h"
#include "HelpWindow.h"

Tester::Tester (QString n) : QTabDialog (0, 0, FALSE)
{
  ruleName = n;
  recordList = 0;
  customShortStopLine = 0;
  customLongStopLine = 0;
  enterLongAlerts = new Setting;
  exitLongAlerts = new Setting;
  enterShortAlerts = new Setting;
  exitShortAlerts = new Setting;
  
  fieldList.append(tr("Open"));
  fieldList.append(tr("Close"));

  QString s = "Qtstalker Back Tester";
  s.append(": ");
  s.append(ruleName);
  setCaption (s);

  setDefaultButton(tr("&Test"));
  connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(test()));
  
  setApplyButton(tr("&Apply"));  
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveRule()));
  
  setCancelButton(tr("&Cancel"));  
  
  setOkButton(QString::null);
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(slotHelp()));

  createFormulaPage();

  createStopPage();

  createTestPage();

  createReportPage();

  createChartPage();
  
  loadRule();
}

Tester::Tester () : QTabDialog (0, 0, FALSE)
{
  recordList = 0;
  customLongStopLine = 0;
  customShortStopLine = 0;
  enterLongAlerts = 0;
  exitLongAlerts = 0;
  enterShortAlerts = 0;
  exitShortAlerts = 0;
}

Tester::~Tester ()
{
  if (recordList)
    delete recordList;
    
  if (customLongStopLine)
    delete customLongStopLine;
  
  if (customShortStopLine)
    delete customShortStopLine;
  
  if (enterLongAlerts)
    delete enterLongAlerts;
    
  if (exitLongAlerts)
    delete exitLongAlerts;
    
  if (enterShortAlerts)
    delete enterShortAlerts;
    
  if (exitShortAlerts)
    delete exitShortAlerts;
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

  enterLongEdit = new FormulaEdit(gbox, FormulaEdit::Logic);  
  
  gbox = new QVGroupBox(tr("Exit Long"), w);
  grid->addWidget(gbox, 0, 1);

  exitLongEdit = new FormulaEdit(gbox, FormulaEdit::Logic);  

  gbox = new QVGroupBox(tr("Enter Short"), w);
  grid->addWidget(gbox, 1, 0);

  enterShortEdit = new FormulaEdit(gbox, FormulaEdit::Logic);  
  
  gbox = new QVGroupBox(tr("Exit Short"), w);
  grid->addWidget(gbox, 1, 1);

  exitShortEdit = new FormulaEdit(gbox, FormulaEdit::Logic);  
  
  addTab(w, tr("Rules"));
}

void Tester::createStopPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(10);
  
  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(5);

  QVGroupBox *gbox = new QVGroupBox(tr("Maximum Loss"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

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
  hbox->addWidget(gbox);

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
  hbox->addWidget(gbox);

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
  
  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setSpacing(5);
  
  gbox = new QVGroupBox(tr("Custom Long Stop"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(1);
  grid->addWidget(gbox, 0, 0);
  
  customLongStopCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(customLongStopCheck, SIGNAL(toggled(bool)), this, SLOT(customLongStopToggled(bool)));

  customLongStopEdit = new FormulaEdit(gbox, FormulaEdit::Logic);  

  gbox = new QVGroupBox(tr("Custom Short Stop"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(1);
  grid->addWidget(gbox, 0, 1);
  
  customShortStopCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(customShortStopCheck, SIGNAL(toggled(bool)), this, SLOT(customShortStopToggled(bool)));

  customShortStopEdit = new FormulaEdit(gbox, FormulaEdit::Logic);  
  
  maximumLossToggled(FALSE);
  profitToggled(FALSE);
  trailingToggled(FALSE);
  customLongStopToggled(FALSE);
  customShortStopToggled(FALSE);

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

  QString s = config.getData(Config::DataPath);
  QString s2;
  symbolButton = new SymbolButton(gbox, s, s2);
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
  header->setLabel(1, tr("Entry"), 80);
  header->setLabel(2, tr("Entry Price"), 80);
  header->setLabel(3, tr("Exit"), 80);
  header->setLabel(4, tr("Exit Price"), 80);
  header->setLabel(5, tr("Signal"), -1);
  header->setLabel(6, tr("Profit"), 80);
  header->setLabel(7, tr("Account"), -1);
  header->setLabel(8, tr("Vol"), 60);
  vbox->addWidget(tradeList);

  int loop;
  for (loop = 0; loop < 9; loop++)
    tradeList->setColumnReadOnly(loop, TRUE);
    
  // test summary
  
  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(5);

  QVGroupBox *gbox = new QVGroupBox(tr("Test Summary"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  QLabel *label = new QLabel(tr("Account Balance "), gbox);
  summaryBalance = new QLabel(" ", gbox);
  
  label = new QLabel(tr("Net Profit "), gbox);
  summaryNetProfit = new QLabel(" ", gbox);

  label = new QLabel(tr("Net Profit % "), gbox);
  summaryNetPercentage = new QLabel(" ", gbox);

  label = new QLabel(tr("Initial Investment "), gbox);
  summaryInvestment = new QLabel(" ", gbox);

  label = new QLabel(tr("Commissions "), gbox);
  summaryCommission = new QLabel(" ", gbox);

  label = new QLabel(tr("Largest Drawdown "), gbox);
  summaryDrawdown = new QLabel(" ", gbox);

  label = new QLabel(tr("Trades "), gbox);
  summaryTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Long Trades "), gbox);
  summaryLongTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Short Trades "), gbox);
  summaryShortTrades = new QLabel(" ", gbox);
  
  // win summary

  gbox = new QVGroupBox(tr("Win Summary"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  label = new QLabel(tr("Trades "), gbox);
  summaryWinTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Profit "), gbox);
  summaryTotalWinTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Average "), gbox);
  summaryAverageWin = new QLabel(" ", gbox);

  label = new QLabel(tr("Largest "), gbox);
  summaryLargestWin = new QLabel(" ", gbox);

  label = new QLabel(tr("Long Trades "), gbox);
  summaryWinLongTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Short Trades "), gbox);
  summaryWinShortTrades = new QLabel(" ", gbox);

  // lose summary

  gbox = new QVGroupBox(tr("Loss Summary"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  label = new QLabel(tr("Trades "), gbox);
  summaryLoseTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Profit "), gbox);
  summaryTotalLoseTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Average "), gbox);
  summaryAverageLose = new QLabel(" ", gbox);

  label = new QLabel(tr("Largest "), gbox);
  summaryLargestLose = new QLabel(" ", gbox);

  label = new QLabel(tr("Long Trades "), gbox);
  summaryLoseLongTrades = new QLabel(" ", gbox);

  label = new QLabel(tr("Short Trades "), gbox);
  summaryLoseShortTrades = new QLabel(" ", gbox);

  addTab(w, tr("Reports"));
}

void Tester::createChartPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  split = new QSplitter(w);
  split->setOrientation(Vertical);
  vbox->addWidget(split);

  bool logFlag = config.getData(Config::LogScale).toInt();
  bool scaleToScreenFlag = config.getData(Config::ScaleToScreen).toInt();
  
  equityPlot = new Plot (split);
  equityPlot->setGridFlag(TRUE);
  equityPlot->setScaleToScreen(scaleToScreenFlag);
//  equityPlot->setLogScale(logFlag);
  equityPlot->setPixelspace(5);
  equityPlot->setIndex(0);
  equityPlot->setDateFlag(TRUE);
  equityPlot->setMainFlag(FALSE);
  equityPlot->setInfoFlag(FALSE);
  equityPlot->setCrosshairsFlag (FALSE); // turn off crosshairs
  QObject::connect(this, SIGNAL(signalIndex(int)), equityPlot, SLOT(setIndex(int)));
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  equityPlot->setPlotFont(font);
  
  plot = new Plot (split);
  plot->setGridFlag(TRUE);
  plot->setScaleToScreen(scaleToScreenFlag);
  plot->setLogScale(logFlag);
  plot->setPixelspace(5);
  plot->setIndex(0);
  plot->setDateFlag(TRUE);
  plot->setMainFlag(FALSE);
  plot->setInfoFlag(FALSE);
  plot->setCrosshairsFlag (FALSE); // turn off crosshairs
  QObject::connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  plot->setPlotFont(font);

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
    
  if (checkFormula(0))
    return;
    
  if (checkFormula(1))
    return;
  
  if (checkFormula(2))
    return;

  if (checkFormula(3))
    return;

  if (checkFormula(4))
    return;

  if (checkFormula(5))
    return;

  QString s = symbolButton->getPath();
  QString plugin = config.parseDbPlugin(s);
  DbPlugin *db = config.getDbPlugin(plugin);
  if (! db)
  {
    config.closePlugin(plugin);
    return;
  }

  s = symbolButton->getPath();
  QDir dir;
  if (! dir.exists(s))
  {
    config.closePlugin(plugin);
    return;
  }
  
  if (db->openChart(s))
  {
    config.closePlugin(plugin);
    return;
  }
  
  db->getHeaderField(DbPlugin::Type, chartType);
  if (! chartType.compare(tr("Futures")))
  {
    s = "FuturesType";
    QString s2;
    db->getData(s, s2);
    fd.setSymbol(s2);
  }

  tradeList->setNumRows(0);

  db->setBarCompression(BarData::DailyBar);
  db->setBarRange(bars->value());
  if (recordList)
    delete recordList;
  recordList = new BarData;
  db->getHistory(recordList);

  plot->clear();
  equityPlot->clear();
    
  equityCurve = new PlotLine;
  s = "green";
  equityCurve->setColor(s);

  if (loadAlerts(0))
  {
    config.closePlugin(plugin);
    return;
  }
  
  if (loadAlerts(1))
  {
    config.closePlugin(plugin);
    return;
  }
  
  if (loadAlerts(2))
  {
    config.closePlugin(plugin);
    return;
  }
  
  if (loadAlerts(3))
  {
    config.closePlugin(plugin);
    return;
  }

  clearAlertCounts();
  
  if (loadCustomLongStop())
  {
    config.closePlugin(plugin);
    return;
  }
  
  if (loadCustomShortStop())
  {
    config.closePlugin(plugin);
    return;
  }

  QProgressDialog prog(tr("Testing..."),
                       tr("Cancel"),
		       bars->value(),
		       this,
		       "progress",
		       TRUE);
  prog.show();
  
  this->setEnabled(FALSE);
  
  status = 0;
  testLoop = 0;
  for (; testLoop < (int) recordList->count(); testLoop++)
  {
    prog.setProgress(testLoop);
    emit message(QString());
    if (prog.wasCancelled())
      break;
  
    updateEquityCurve();
    
    currentRecord = testLoop;
    BarDate dt = recordList->getDate(currentRecord);
    
    checkAlerts();

    if (status == 0)
    {
      if (enterLongAlerts->count() && tradeLong->isChecked())
      {
        if (enterLongCount)
        {
          enterLong();
          continue;
        }
      }

      if (enterShortAlerts->count() && tradeShort->isChecked())
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
      if (enterShortAlerts->count() && tradeShort->isChecked())
      {
        if (enterShortCount)
        {
          enterShort();
          continue;
        }
      }

      if (exitLongAlerts->count() && tradeLong->isChecked())
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
      if (enterLongAlerts->count() && tradeLong->isChecked())
      {
        if (enterLongCount)
        {
          enterLong();
          continue;
        }
      }

      if (exitShortAlerts->count() && tradeShort->isChecked())
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

    if (customStop())
      continue;
    
    trailing();
  }

  if (status != 0)
  {
    QString t = tr("End of test");
    exitPosition(t);
    status = 0;
  }

  createSummary();
  
  updateChart();

  createEquityCurve();
  
  config.closePlugin(plugin);
  
  this->setEnabled(TRUE);
}

void Tester::checkAlerts ()
{
  QString s;
  QString key;
  recordList->getDate(currentRecord).getDateString(FALSE, key);

  switch (status)
  {
    case 0:
      s = enterLongAlerts->getData(key);
      if (s.length())
        enterLongCount++;
      else
      {
        s = enterShortAlerts->getData(key);
        if (s.length())
          enterShortCount++;
      }
      break;
    case 1:
      s = exitLongAlerts->getData(key);
      if (s.length())
        exitLongCount++;
      else
      {
        s = enterShortAlerts->getData(key);
        if (s.length())
          enterShortCount++;
      }
      break;
    case -1:
      s = exitShortAlerts->getData(key);
      if (s.length())
        exitShortCount++;
      else
      {
        s = enterLongAlerts->getData(key);
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
  {
    QString t = tr("Enter Long");
    exitPosition(t);
  }

  status = 1;
  buyRecord = currentRecord;
  trailingHigh = getPrice(buyRecord);
  equity = equity - entryCom->value();
  getVolume();

  clearAlertCounts();
}

void Tester::exitLong ()
{
  QString t = tr("Exit Long");
  exitPosition(t);
  status = 0;
  clearAlertCounts();
}

void Tester::enterShort ()
{
  if (status != 0)
  {
    QString t = tr("Enter Short");
    exitPosition(t);
  }

  status = -1;
  buyRecord = currentRecord;
  trailingLow = getPrice(buyRecord);
  equity = equity - entryCom->value();
  getVolume();

  clearAlertCounts();
}

void Tester::exitShort ()
{
  QString t = tr("Exit Short");
  exitPosition(t);
  status = 0;
  clearAlertCounts();
}

void Tester::exitPosition (QString &signal)
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

  if (! chartType.compare(tr("Futures")))
    profit = fd.getRate() * profit;

  equity = equity + profit;

  equity = equity - exitCom->value();

  QString s;
  tradeList->setNumRows(tradeList->numRows() + 1);
  tradeList->setText(tradeList->numRows() - 1, 0, type);
  recordList->getDate(buyRecord).getDateString(FALSE, s);
  tradeList->setText(tradeList->numRows() - 1, 1, s);
  tradeList->setText(tradeList->numRows() - 1, 2, QString::number(enterPrice));
  recordList->getDate(currentRecord).getDateString(FALSE, s);
  tradeList->setText(tradeList->numRows() - 1, 3, s);
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
    double t = enterPrice * (1 - (maximumLossEdit->text().toDouble() / 100));
    if (exitPrice <= t)
    {
      QString t = tr("Maximum Loss");
      exitPosition(t);
      status = 0;
      clearAlertCounts();
      return TRUE;
    }
  }

  if ((status == -1) && (maximumLossShort->isChecked()))
  {
    double t = enterPrice * (1 + (maximumLossEdit->text().toDouble() / 100));
    if (exitPrice >= t)
    {
      QString t = tr("Maximum Loss");
      exitPosition(t);
      status = 0;
      clearAlertCounts();
      return TRUE;
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
    double t = enterPrice * (1 + (profitEdit->text().toDouble() / 100));
    if (exitPrice >= t)
    {
      QString t = tr("Profit");
      exitPosition(t);
      status = 0;
      clearAlertCounts();
      return TRUE;
    }
  }

  if ((status == -1) && (profitShort->isChecked()))
  {
    double t = enterPrice * (1 - (profitEdit->text().toDouble() / 100));
    if (exitPrice <= t)
    {
      QString t = tr("Profit");
      exitPosition(t);
      status = 0;
      clearAlertCounts();
      return TRUE;
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
        QString t = tr("Trailing");
        exitPosition(t);
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
        QString t = tr("Trailing");
        exitPosition(t);
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  return FALSE;
}

bool Tester::customStop ()
{
  if (status == 1 && customLongStopCheck->isChecked() && customLongStopEdit->getLines() &&
      customLongStopLine)
  {
    int i = customLongStopLine->getSize() - currentRecord;
    if (i > -1)
    {
      if (customLongStopLine->getData(i) == 1)
      {
        QString t = tr("Custom Long Stop");
        exitPosition(t);
        status = 0;
        clearAlertCounts();
        return TRUE;
      }
    }
  }

  if (status == -1 && customShortStopCheck->isChecked() && customShortStopEdit->getLines() &&
      customShortStopLine)
  {
    int i = customShortStopLine->getSize() - currentRecord;
    if (i > -1)
    {
      if (customShortStopLine->getData(i) == 1)
      {
        QString t = tr("Custom Short Stop");
        exitPosition(t);
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

void Tester::customShortStopToggled (bool status)
{
  if (status)
    customShortStopEdit->setEnabled(TRUE);
  else
    customShortStopEdit->setEnabled(FALSE);
}

void Tester::customLongStopToggled (bool status)
{
  if (status)
    customLongStopEdit->setEnabled(TRUE);
  else
    customLongStopEdit->setEnabled(FALSE);
}

void Tester::symbolButtonPressed ()
{
  QString symbol = symbolButton->getPath();
}

bool Tester::loadAlerts (int type)
{
  int loop;
  FormulaEdit *edit = 0;
  int delays = 0;
  QString s;
  Setting *alerts = 0;
  
  switch(type)
  {
    case 0:
      edit = enterLongEdit;
      enterLongAlerts->clear();
      delays = enterLongDelay->value();
      s = tr("Enter long ");
      alerts = enterLongAlerts;
      break;
    case 1:
      edit = exitLongEdit;
      exitLongAlerts->clear();
      delays = exitLongDelay->value();
      s = tr("Exit long ");
      alerts = exitLongAlerts;
      break;
    case 2:
      edit = enterShortEdit;
      enterShortAlerts->clear();
      delays = enterShortDelay->value();
      s = tr("Enter short ");
      alerts = enterShortAlerts;
      break;
    case 3:
      edit = exitShortEdit;
      exitShortAlerts->clear();
      delays = exitShortDelay->value();
      s = tr("Exit short ");
      alerts = exitShortAlerts;
      break;
    default:
      break;
  }
  
  if (! edit->getLines())
    return FALSE;
  
  bool cflag = FALSE;
  for (loop = 0; loop < edit->getLines(); loop++)
  {
    Setting set;
    QString t = edit->getLine(loop);
    set.parse(t);
    if (! set.getData("plugin").compare("COMP"))
    {
      if (set.getData("plot").toInt())
      {
        cflag = TRUE;
	break;
      }
    }
  }
  
  if (! cflag)
  {
    s.append(" no COMP step or COMP step not checked.");
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     s);
    return TRUE;
  }
  
  // open the CUS plugin
  QString plugin("CUS");
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return TRUE;
  }

  for (loop = 0; loop < edit->getLines(); loop++)
  {
    QString t(edit->getLine(loop));
    plug->setCustomFunction(t);
  }
  
  // load the CUS plugin and calculate
  plug->setIndicatorInput(recordList);
  plug->calculate();
  Indicator *i = plug->getIndicator();
  PlotLine *line = i->getLine(0);
  if (! line)
  {
    qDebug("Tester::loadAlerts: no PlotLine returned");
    config.closePlugin(plugin);
    return TRUE;
  }
    
  loop = recordList->count() - line->getSize();
  int lineLoop = 0;
  for (; loop < (int) recordList->count(); loop++, lineLoop++)
  {
    if (line->getData(lineLoop) == 1)
    {
      if (delays)
      {
        if ((lineLoop - delays) > -1)
	{
          int loop2;
	  bool df = FALSE;
          for (loop2 = delays; loop2 > -1; loop2--)
	  {
            if (line->getData(lineLoop - loop2) != 1)
	    {
              df = TRUE;
	      break;
	    }
	  }
	
	  if (! df)
	  {
	    QString t;
	    recordList->getDate(loop).getDateString(FALSE, t);
            alerts->setData(t, "1");
	  }
	}
      }
      else
      {
        QString t;
	recordList->getDate(loop).getDateString(FALSE, t);
        alerts->setData(t, "1");
      }
    }
  }
  
  config.closePlugin(plugin);
  
  return FALSE;
}

void Tester::saveEditRule (int type)
{
  FormulaEdit *edit = 0;
  QString s = config.getData(Config::TestPath);
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
  saveCustomStopRule();

  QString s = config.getData(Config::TestPath);
  s.append("/");
  s.append(ruleName);
  s.append("/rule");

  QFile f(s);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);
  
  int loop;
  for (loop = 0; loop < (int) tradeList->numRows(); loop++)
  {
    s = "Trade=";
    QStringList l;
    int loop2;
    for (loop2 = 0; loop2 < 9; loop2++)
      l.append(tradeList->text(loop, loop2));
    stream << s << l.join(",") << "\n";
  }
  
  stream << "Maximum Loss Check=" << QString::number(maximumLossCheck->isChecked()) << "\n";
  stream << "Maximum Loss Long=" << QString::number(maximumLossLong->isChecked()) << "\n";
  stream << "Maximum Loss Short=" << QString::number(maximumLossShort->isChecked()) << "\n";
  stream << "Maximum Loss Edit=" << maximumLossEdit->text() << "\n";
  stream << "Profit Check=" << QString::number(profitCheck->isChecked()) << "\n";
  stream << "Profit Long=" << QString::number(profitLong->isChecked()) << "\n";
  stream << "Profit Short=" << QString::number(profitShort->isChecked()) << "\n";
  stream << "Profit Edit=" << profitEdit->text() << "\n";
  stream << "Trailing Check=" << QString::number(trailingCheck->isChecked()) << "\n";
  stream << "Trailing Long=" << QString::number(trailingLong->isChecked()) << "\n";
  stream << "Trailing Short=" << QString::number(trailingShort->isChecked()) << "\n";
  stream << "Trailing Edit=" << trailingEdit->text() << "\n";
  stream << "TradeLong=" << QString::number(tradeLong->isChecked()) << "\n";
  stream << "TradeShort=" << QString::number(tradeShort->isChecked()) << "\n";
  stream << "Volume Percent=" << volumePercent->text() << "\n";
  stream << "Entry Com=" << entryCom->text() << "\n";
  stream << "Exit Com=" << exitCom->text() << "\n";
  stream << "EnterLongDelay=" << enterLongDelay->text() << "\n";
  stream << "ExitLongDelay=" << exitLongDelay->text() << "\n";
  stream << "EnterShortDelay=" << enterShortDelay->text() << "\n";
  stream << "ExitShortDelay=" << exitShortDelay->text() << "\n";
  stream << "Price Field=" << priceField->currentText() << "\n";
  stream << "Bars=" << bars->text() << "\n";
  stream << "Symbol=" << symbolButton->getPath() << "\n";
  stream << "Account=" << account->text() << "\n";
  
  f.close();
}

void Tester::loadRule ()
{
  loadEditRule(0);
  loadEditRule(1);
  loadEditRule(2);
  loadEditRule(3);
  loadCustomStopRule();

  QString s = config.getData(Config::TestPath);
  s.append("/");
  s.append(ruleName);
  s.append("/rule");
  
  while (tradeList->numRows())
    tradeList->removeRow(0);

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
      maximumLossCheck->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Maximum Loss Long"))
    {
      maximumLossLong->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Maximum Loss Short"))
    {
      maximumLossShort->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Maximum Loss Edit"))
    {
      maximumLossEdit->setText(l2[1]);
      continue;
    }

    if (! l2[0].compare("Profit Check"))
    {
      profitCheck->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Profit Long"))
    {
      profitLong->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Profit Short"))
    {
      profitShort->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Profit Edit"))
    {
      profitEdit->setText(l2[1]);
      continue;
    }

    if (! l2[0].compare("Trailing Check"))
    {
      trailingCheck->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Trailing Long"))
    {
      trailingLong->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Trailing Short"))
    {
      trailingShort->setChecked(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Trailing Edit"))
    {
      trailingEdit->setText(l2[1]);
      continue;
    }
    
    if (! l2[0].compare("TradeLong"))
    {
      tradeLong->setChecked(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("TradeShort"))
    {
      tradeShort->setChecked(l2[1].toInt());
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
  
    if (! l2[0].compare("Symbol"))
    {
      symbolButton->setSymbol(l2[1]);
      continue;
    }
    
    if (! l2[0].compare("Account"))
    {
      account->setValue(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("Trade"))
    {
      QStringList l3 = QStringList::split(",", l2[1], FALSE);
      int loop;
      tradeList->setNumRows(tradeList->numRows() + 1);
      for (loop = 0; loop < (int) l3.count(); loop++)
        tradeList->setText(tradeList->numRows() - 1, loop, l3[loop]);
      continue;
    }
  }
  
  createSummary();
  
  f.close();
}

void Tester::loadEditRule (int type)
{
  FormulaEdit *edit = 0;
  QString s = config.getData(Config::TestPath);
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
    volume = (int) (balance / getPrice(buyRecord));
    return;
  }

  balance = balance * ((double) volumePercent->value() / 100.0);

  if (margin->value())
    volume = (int) (double) (balance / margin->value());
  else
    volume = (int) (double) (balance / getPrice(buyRecord));

//  if (volume < 1)
//    volume = 1;
}

double Tester::getPrice (int i)
{
  double price = 0;
  
  if (! priceField->currentText().compare(tr("Open")))
    price = recordList->getOpen(i);
  else
    price = recordList->getClose(i);
  
  return price;
}

void Tester::updateChart ()
{
  plot->setData(recordList);
  
  //set up indicator
  PlotLine *close = recordList->getInput(BarData::Close);
  QString str("green");
  close->setColor(str);
  
  Indicator *i = new Indicator;
  str = "tester";
  i->setName(str);
  i->addLine(close);
  str = i->getName();
  plot->addIndicator(str, i);

  //set up arrows
  int loop;
  int name = 0;
  double enter = 0;
  double exit = 0;
  Setting set;
  for (loop = 0; loop < (int) tradeList->numRows(); loop++)
  {
    enter = tradeList->text(loop, 2).toDouble();
    exit = tradeList->text(loop, 4).toDouble();
    
    if (! tradeList->text(loop, 0).compare(tr("Short")))
    {
      name++;
      BarDate dt;
      QString t = tradeList->text(loop, 3) + "000000";
      dt.setDate(t);
      dt.getDateTimeString(FALSE, t);
      set.setData("Date", t);
      set.setData("Value", QString::number(enter));
      set.setData("Color", "red");
      set.setData("Plot", "tester");
      set.setData("Name", QString::number(name));
      set.setData("Plugin", "SellArrow");
      plot->addChartObject(set);
      
      name++;
      t = tradeList->text(loop, 1) + "000000";
      dt.setDate(t);
      dt.getDateTimeString(FALSE, t);
      set.setData("Date", t);
      set.setData("Value", QString::number(exit));
      set.setData("Color", "green");
      set.setData("Plot", "tester");
      set.setData("Name", QString::number(name));
      set.setData("Plugin", "BuyArrow");
      plot->addChartObject(set);
    }
    else
    {
      name++;
      BarDate dt;
      QString t = tradeList->text(loop, 1) + "000000";
      dt.setDate(t);
      dt.getDateTimeString(FALSE, t);
      set.setData("Date", t);
      set.setData("Value", QString::number(enter));
      set.setData("Color", "green");
      set.setData("Plot", "tester");
      set.setData("Name", QString::number(name));
      set.setData("Plugin", "BuyArrow");
      plot->addChartObject(set);
      
      name++;
      t = tradeList->text(loop, 3) + "000000";
      dt.setDate(t);
      dt.getDateTimeString(FALSE, t);
      set.setData("Date", t);
      set.setData("Value", QString::number(exit));
      set.setData("Color", "red");
      set.setData("Plot", "tester");
      set.setData("Name", QString::number(name));
      set.setData("Plugin", "SellArrow");
      plot->addChartObject(set);
    }
  }

  slider->setMaxValue(recordList->count() - 1);
        
  plot->draw();
}

void Tester::createEquityCurve ()
{
  equityPlot->setData(recordList);
  
  //set up indicator
  Indicator *i = new Indicator;
  QString str = "equity";
  i->setName(str);
  i->addLine(equityCurve);
  str = i->getName();
  equityPlot->addIndicator(str, i);

  equityPlot->draw();
}

bool Tester::loadCustomShortStop ()
{
  if (customShortStopLine)
  {
    delete customShortStopLine;
    customShortStopLine = 0;
  }
  
  if (! customShortStopEdit->getLines() || ! customShortStopCheck->isChecked())
    return FALSE;
    
  bool cflag = FALSE;
  int loop;
  for (loop = 0; loop < customShortStopEdit->getLines(); loop++)
  {
    Setting set;
    QString t = customShortStopEdit->getLine(loop); 
    set.parse(t);
    if (! set.getData("plugin").compare("COMP"))
    {
      if (set.getData("plot").toInt())
      {
        cflag = TRUE;
	break;
      }
    }
  }
  
  if (! cflag)
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("No COMP step or COMP step not checked in Custom Short Stop."));
    return TRUE;
  }

  QString plugin("CUS");  
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return TRUE;
  }

  for (loop = 0; loop < customShortStopEdit->getLines(); loop++)
  {
    QString t(customShortStopEdit->getLine(loop));
    plug->setCustomFunction(t);
  }
  
  // load the CUS plugin and calculate
  plug->setIndicatorInput(recordList);
  plug->calculate();
  Indicator *i = plug->getIndicator();
  PlotLine *line = i->getLine(0);
  if (! line)
  {
    qDebug("Tester::loadCustomShortStop: no PlotLine returned");
    config.closePlugin(plugin);
    return TRUE;
  }
    
  customShortStopLine = new PlotLine;
  customLongStopLine->copy(line);
    
  config.closePlugin(plugin);
  
  return FALSE;
}

bool Tester::loadCustomLongStop ()
{
  if (customLongStopLine)
  {
    delete customLongStopLine;
    customLongStopLine = 0;
  }
  
  if (! customLongStopEdit->getLines() || ! customLongStopCheck->isChecked())
    return FALSE;
    
  bool cflag = FALSE;
  int loop;
  for (loop = 0; loop < customLongStopEdit->getLines(); loop++)
  {
    Setting set;
    QString t = customLongStopEdit->getLine(loop); 
    set.parse(t);
    if (! set.getData("plugin").compare("COMP"))
    {
      if (set.getData("plot").toInt())
      {
        cflag = TRUE;
	break;
      }
    }
  }
  
  if (! cflag)
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("No COMP step or COMP step not checked in Custom Long Stop."));
    return TRUE;
  }
  
  QString plugin("CUS");  
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return TRUE;
  }

  for (loop = 0; loop < customLongStopEdit->getLines(); loop++)
  {
    QString t(customLongStopEdit->getLine(loop));
    plug->setCustomFunction(t);
  }
  
  // load the CUS plugin and calculate
  plug->setIndicatorInput(recordList);
  plug->calculate();
  Indicator *i = plug->getIndicator();
  PlotLine *line = i->getLine(0);
  if (! line)
  {
    qDebug("Tester::loadCustomShortStop: no PlotLine returned");
    config.closePlugin(plugin);
    return TRUE;
  }
    
  customLongStopLine = new PlotLine;
  customLongStopLine->copy(line);
    
  config.closePlugin(plugin);
  
  return FALSE;
}

void Tester::loadCustomStopRule ()
{
  QString s = config.getData(Config::TestPath) + "/" + ruleName + "/customLongStop";
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
  
    customLongStopEdit->setLine(s);
  }  
  
  f.close();
  
  s = config.getData(Config::TestPath) + "/" + ruleName + "/customShortStop";
  f.setName(s);
  if (! f.open(IO_ReadOnly))
    return;

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
  
    customShortStopEdit->setLine(s);
  }  
  
  f.close();
}

void Tester::saveCustomStopRule ()
{
  QString s = config.getData(Config::TestPath) + "/" + ruleName + "/customShortStop";
  QFile f(s);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);

  int loop;
  for (loop = 0; loop < customShortStopEdit->getLines(); loop++)
    stream << customShortStopEdit->getLine(loop) << "\n";
  
  f.close();
  
  s = config.getData(Config::TestPath) + "/" + ruleName + "/customLongStop";
  f.setName(s);
  if (! f.open(IO_WriteOnly))
    return;

  for (loop = 0; loop < customLongStopEdit->getLines(); loop++)
    stream << customLongStopEdit->getLine(loop) << "\n";
  
  f.close();
}

QString Tester::newTest ()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Backtest Rule"),
  				    tr("Enter new backtest rule name."),
  				    QLineEdit::Normal,
				    tr("NewRule"),
				    &ok,
				    this);

  if ((! ok) || (s.isNull()))
    return s;

  int loop;
  QString selection;
  for (loop = 0; loop < (int) s.length(); loop++)
  {
    QChar c = s.at(loop);
    if (c.isLetterOrNumber())
      selection.append(c);
  }
  
  s = config.getData(Config::TestPath) + "/" + selection;
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return selection;
  }

  if (! dir.mkdir(s, TRUE))
  {
    qDebug("TestPage::newTest:can't create dir %s", s.latin1());
    return selection;
  }
  
  if (! dir.mkdir(s + "/el", TRUE))
  {
    qDebug("TestPage::newTest:can't create el dir");
    return selection;
  }
  
  if (! dir.mkdir(s + "/xl", TRUE))
  {
    qDebug("TestPage::newTest:can't create xl dir");
    return selection;
  }
  
  if (! dir.mkdir(s + "/es", TRUE))
  {
    qDebug("TestPage::newTest:can't create es dir");
    return selection;
  }
  
  if (! dir.mkdir(s + "/xs", TRUE))
  {
    qDebug("TestPage::newTest:can't create xs dir");
    return selection;
  }
  
  return selection;
}

void Tester::slotHelp ()
{
  HelpWindow *hw = 0;
  QString str;
  QString s = tabLabel(currentPage());
  
  while (s.length())
  {
    if (! s.compare("Rules"))
    {
      str = "backtesterrules.html";
      hw = new HelpWindow(this, str);
      break;
    }
    
    if (! s.compare("Stops"))
    {
      str = "backtesterstops.html";
      hw = new HelpWindow(this, str);
      break;
    }
  
    if (! s.compare("Testing"))
    {
      str = "backtestertesting.html";
      hw = new HelpWindow(this, str);
      break;
    }
  
    if (! s.compare("Reports"))
    {
      str = "backtesterreports.html";
      hw = new HelpWindow(this, str);
      break;
    }
    
    if (! s.compare("Chart"))
    {
      str = "backtesterchart.html";
      hw = new HelpWindow(this, str);
      break;
    }
    
    break;
  }
  
  if (hw)
    hw->show();
}

void Tester::slotScaleToScreen (bool d)
{
  plot->setScaleToScreen(d);
  equityPlot->setScaleToScreen(d);
  equityPlot->draw();
  plot->draw();
}

void Tester::slotLogScaling (bool d)
{
  plot->setLogScale(d);
//  equityPlot->setLogScale(d);
//  equityPlot->draw();
  plot->draw();
}

bool Tester::checkFormula (int d)
{
  bool ok = FALSE;
  QString s;
  
  switch(d)
  {
    case 0:
      if (enterLongEdit->getLines())
      {
        ok = enterLongEdit->checkError();
        s.append(tr("Enter Long: "));
      }
      break;
    case 1:
      if (exitLongEdit->getLines())
      {
        ok = exitLongEdit->checkError();
        s.append(tr("Exit Long: "));
      }
      break;
    case 2:
      if (enterShortEdit->getLines())
      {
        ok = enterShortEdit->checkError();
        s.append(tr("Enter Short: "));
      }
      break;
    case 3:
      if (exitShortEdit->getLines())
      {
        ok = exitShortEdit->checkError();
        s.append(tr("Exit Short: "));
      }
      break;
    case 4:
      if (customLongStopEdit->getLines())
      {
        ok = customLongStopEdit->checkError();
        s.append(tr("Custom Long Stop: "));
      }
      break;
    case 5:
      if (customShortStopEdit->getLines())
      {
        ok = customShortStopEdit->checkError();
        s.append(tr("Custom Short Stop: "));
      }
      break;
    default:
      break;
  }
  
  if (ok)
  {
    s.append(tr("Must have one COMP step checked."));
    QMessageBox::information(this, tr("Qtstalker: Error"), s);
  }
  
  return ok;
}

