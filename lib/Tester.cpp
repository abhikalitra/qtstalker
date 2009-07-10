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
#include <QMessageBox>
#include <QInputDialog>
#include <QProgressDialog>
#include <QtDebug>

#include "Tester.h"
#include "IndicatorPlugin.h"
#include "DataBase.h"




Tester::Tester (QString &n) : QDialog (0, 0)
{
  rule.setName(n);
  recordList = 0;
  customShortStopLine = 0;
  customLongStopLine = 0;
  
  QString s = "Qtstalker Back Tester";
  s.append(": ");
  s.append(n);
  setWindowTitle(s);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(10);
  vbox->setSpacing(10);
  setLayout(vbox);

  tabs = new QTabWidget;
  vbox->addWidget(tabs);
  
  rulePage = new TesterRulePage(this);
  tabs->addTab(rulePage, tr("Rules"));

  stopPage = new TesterStopPage(this);
  tabs->addTab(stopPage, tr("Stops"));

  testPage = new TesterTestPage(this);
  tabs->addTab(testPage, tr("Testing"));

  reportPage = new TesterReport(this);
  tabs->addTab(reportPage, tr("Reports"));

  chartPage = new TesterChartPage(this);
  tabs->addTab(chartPage, tr("Chart"));

  buttonBox = new QDialogButtonBox;
  vbox->addWidget(buttonBox);

  QPushButton *button = buttonBox->addButton(QDialogButtonBox::Ok);
  connect(button, SIGNAL(clicked()), this, SLOT(test()));

  button = buttonBox->addButton(QDialogButtonBox::Save);
  connect(button, SIGNAL(clicked()), this, SLOT(saveRule()));

  button = buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));

  loadRule();
}

Tester::~Tester ()
{
  if (recordList)
    delete recordList;

  qDeleteAll(enterLongSignal);
  qDeleteAll(exitLongSignal);
  qDeleteAll(enterShortSignal);
  qDeleteAll(exitShortSignal);
  qDeleteAll(trades);

  if (customLongStopLine)
    delete customLongStopLine;
  
  if (customShortStopLine)
    delete customShortStopLine;
}

void Tester::saveRule ()
{
  rulePage->getRules(rule);
  stopPage->getStops(rule);
  testPage->getParms(rule);
  reportPage->getParms(rule);

  DataBase db;
  db.setTest(rule);

  emit signalSaved();
}

void Tester::loadRule ()
{
  DataBase db;
  db.getTest(rule);

  rulePage->setRules(rule);
  stopPage->setStops(rule);
  testPage->setParms(rule);
  reportPage->clear();
  reportPage->setParms(rule);
  chartPage->clear();
}

int Tester::getVolume (int i, double d)
{
  double balance = d;
  int volume = 1;
  double volumePercent = rule.getDouble(TesterRule::VolumePercent);

  if (volumePercent == 0)
    return volume;

  balance = balance * ((double) volumePercent / 100.0);

  volume = (int) (double) (balance / getPrice(i));

  return volume;
}

double Tester::getPrice (int i)
{
  double price = 0;

  QString field;
  rule.getData(TesterRule::PriceField, field);
  
  if (field == tr("Open"))
    price = recordList->getOpen(i);
  else
  {
    if (field == tr("Close"))
      price = recordList->getClose(i);
    else
      price = recordList->getLow(i) + ((recordList->getHigh(i) - recordList->getLow(i)) / 2);
  }
  
  return price;
}

void Tester::loadSignals ()
{
  qDeleteAll(enterLongSignal);
  enterLongSignal.clear();

  qDeleteAll(exitLongSignal);
  exitLongSignal.clear();

  qDeleteAll(enterShortSignal);
  enterShortSignal.clear();

  qDeleteAll(exitShortSignal);
  exitShortSignal.clear();

  IndicatorPlugin *ip = new IndicatorPlugin;
  ip->setIndicatorInput(recordList);

  int loop;
  for (loop = 0; loop < 4; loop++)
  {
    Indicator i;
    switch (loop)
    {
      case 0:
        rule.getEnterLong(i);
        break;
      case 1:
        rule.getExitLong(i);
        break;
      case 2:
        rule.getEnterShort(i);
        break;
      case 3:
        rule.getExitShort(i);
        break;
      default:
        break;
    }

    ip->setIndicator(i);

    QList<PlotLine *> plotList;
    ip->calculate(plotList);
    if (! plotList.count())
      continue;
    
    PlotLine *line = plotList.at(0);

    int loop2 = recordList->count() - line->getSize();
    int lineLoop = 0;
    Setting *trade = 0;
    for (; loop2 < (int) recordList->count(); loop2++, lineLoop++)
    {
      if (line->getData(lineLoop) == 1)
      {
        if (! trade)
        {
          trade = new Setting;
          QDateTime dt;
          recordList->getDate(loop2, dt);
          QString key = dt.toString("yyyyMMddhhmmss");
          switch (loop)
          {
            case 0:
              enterLongSignal.insert(key, trade);
              break;
            case 1:
              exitLongSignal.insert(key, trade);
              break;
            case 2:
              enterShortSignal.insert(key, trade);
              break;
            case 3:
              exitShortSignal.insert(key, trade);
              break;
            default:
              break;
          }
        }
      }
      else
      {
        if (trade)
          trade = 0;
      }
    }

    qDeleteAll(plotList);
  }
}

void Tester::test ()
{
  if (! rule.getInt(TesterRule::TradeLong) && ! rule.getInt(TesterRule::TradeShort))
    return;

  equity = rule.getDouble(TesterRule::Account);
  if (equity == 0)
    return;

  QStringList l;
  QString symbol;
  rule.getSymbols(l);
  if (l.count())
    symbol = l[0];

  if (symbol.isEmpty())
    return;


  if (recordList)
    delete recordList;
  recordList = new BarData(symbol);
  recordList->setBarsRequested(rule.getInt(TesterRule::Bars));
  QString s;
  rule.getData(TesterRule::BarLength, s);
  recordList->setBarLength(s);
  DataBase db;
  db.getChart(recordList);

  chartPage->clear();

  loadSignals();

  if (loadCustomLongStop())
    return;
  
  if (loadCustomShortStop())
    return;

  reportPage->clear();
  
  qDeleteAll(trades);
  trades.clear();

  QProgressDialog prog(tr("Testing..."), tr("Cancel"), 1, rule.getInt(TesterRule::Bars), this);
  prog.show();
  
#ifndef MINGW //gje: on windows, disabling/enabling blocks dialog completely
  this->setEnabled(FALSE);
#endif

  currentRecord = 0;
  for (; currentRecord < (int) recordList->count(); currentRecord++)
  {
    prog.setValue(currentRecord);
    emit message(QString());
    if (prog.wasCanceled())
      break;
  
    QDateTime dt;
    recordList->getDate(currentRecord, dt);
    QString key = dt.toString("yyyyMMddhhmmss");

    if (rule.getInt(TesterRule::TradeLong))
    {
      Setting *set = enterLongSignal[key];
      if (set)
        enterTrade(TradeItem::Long);
    }

    if (rule.getInt(TesterRule::TradeShort))
    {
      Setting *set = enterShortSignal[key];
      if (set)
        enterTrade(TradeItem::Short);
    }
  }

  reportPage->createSummary(trades, rule.getDouble(TesterRule::Account));

  chartPage->updateChart(recordList, trades, rule.getDouble(TesterRule::Account));

#ifndef MINGW //gje: on windows, disabling/enabling blocks dialog completely
  this->setEnabled(TRUE);
#endif
}

void Tester::enterTrade (TradeItem::TradePosition flag)
{
  TradeItem *trade = new TradeItem;
  trade->setTradePosition(flag);
  if (flag == TradeItem::Long)
    trade->setEnterSignal(TradeItem::EnterLong);
  else
    trade->setEnterSignal(TradeItem::EnterShort);

  int buyRecord = 0;
  if (currentRecord < recordList->count())
    buyRecord = currentRecord;
  else
    buyRecord = currentRecord;

  QDateTime td;
  recordList->getDate(buyRecord, td);
  trade->setEnterDate(td);

  double enterPrice = getPrice(buyRecord);
  trade->setEnterPrice(enterPrice);

  setTrailingHigh(getPrice(buyRecord));

  if (! trades.count())
  {
    trade->setVolume(getVolume(buyRecord, rule.getDouble(TesterRule::Account)));
    trade->setBalance(rule.getDouble(TesterRule::Account));
  }
  else
  {
    TradeItem *t = trades.at(trades.count() - 1);
    trade->setVolume(getVolume(buyRecord, t->getBalance()));
    trade->setBalance(t->getBalance());
  }
  if (trade->getVolume() == 0)
  {
    delete trade;
    return;
  }

  trade->setCommissionType(rule.getInt(TesterRule::CommissionType));

  trade->setEntryCom(rule.getDouble(TesterRule::EntryCom));

  trade->setExitCom(rule.getDouble(TesterRule::ExitCom));

  int loop = buyRecord;
  for (; loop < (int) recordList->count(); loop++)
  {
    QDateTime dt;
    recordList->getDate(loop, dt);
    QString key = dt.toString("yyyyMMddhhmmss");

    Setting *set = 0;
    if (flag == TradeItem::Long)
      set = exitLongSignal[key];
    else
      set = exitShortSignal[key];
    if (set)
    {
      int sellRecord = 0;
      if (loop < recordList->count())
        sellRecord = loop;
      else
        sellRecord = loop;

      recordList->getDate(sellRecord, td);
      trade->setExitDate(td);
      trade->setExitPrice(getPrice(sellRecord));
      if (flag == TradeItem::Long)
        trade->setExitSignal(TradeItem::ExitLong);
      else
        trade->setExitSignal(TradeItem::ExitShort);
      trades.append(trade);
      currentRecord = loop - 1;
      trade->calculateProfit();
      break;
    }

    bool tflag = FALSE;
    if (flag == TradeItem::Long)
      tflag = maximumLoss(FALSE, enterPrice, recordList->getLow(loop));
    else
      tflag = maximumLoss(TRUE, enterPrice, recordList->getHigh(loop));
    if (tflag)
    {
      int sellRecord = 0;
      if (loop < recordList->count())
        sellRecord = loop;
      else
        sellRecord = loop;

      recordList->getDate(sellRecord, td);
      trade->setExitDate(td);
      trade->setExitPrice(getPrice(sellRecord));
      trade->setExitSignal(TradeItem::MaximumLoss);
      trades.append(trade);
      currentRecord = loop - 1;
      trade->calculateProfit();
      break;
    }

    if (flag == TradeItem::Long)
      tflag = calculateProfit(FALSE, enterPrice, recordList->getHigh(loop));
    else
      tflag = calculateProfit(TRUE, enterPrice, recordList->getLow(loop));
    if (tflag)
    {
      int sellRecord = 0;
      if (loop < recordList->count())
        sellRecord = loop;
      else
        sellRecord = loop;

      recordList->getDate(sellRecord, td);
      trade->setExitDate(td);
      trade->setExitPrice(getPrice(sellRecord));
      trade->setExitSignal(TradeItem::Profit);
      trades.append(trade);
      currentRecord = loop - 1;
      trade->calculateProfit();
      break;
    }

    if (flag == TradeItem::Long)
      tflag = customStop(FALSE, loop);
    else
      tflag = customStop(TRUE, loop);
    if (tflag)
    {
      int sellRecord = 0;
      if (loop < recordList->count())
        sellRecord = loop;
      else
        sellRecord = loop;

      recordList->getDate(sellRecord, td);
      trade->setExitDate(td);
      trade->setExitPrice(getPrice(sellRecord));
      trade->setExitSignal(TradeItem::CUSStop);
      trades.append(trade);
      currentRecord = loop - 1;
      trade->calculateProfit();
      break;
    }
    
    if (flag == TradeItem::Long)
      tflag = calculateTrailing(FALSE, recordList->getLow(loop));
    else
      tflag = calculateTrailing(TRUE, recordList->getHigh(loop));
    if (tflag)
    {
      int sellRecord = 0;
      if (loop < recordList->count())
        sellRecord = loop;
      else
        sellRecord = loop;

      recordList->getDate(sellRecord, td);
      trade->setExitDate(td);
      trade->setExitPrice(getPrice(sellRecord));
      trade->setExitSignal(TradeItem::Trailing);
      trades.append(trade);
      currentRecord = loop - 1;
      trade->calculateProfit();
      break;
    }
  }

  if (trade->getExitSignal() == TradeItem::None)
  {
    recordList->getDate(recordList->count() - 1, td);
    trade->setExitDate(td);
    trade->setExitPrice(getPrice(recordList->count() - 1));
    trade->setExitSignal(TradeItem::EndTest);
    trades.append(trade);
    currentRecord = loop;
    trade->calculateProfit();
  }
}

bool Tester::loadCustomShortStop ()
{
  if (customShortStopLine)
  {
    delete customShortStopLine;
    customShortStopLine = 0;
  }
  
  if (! rule.getInt(TesterRule::CustomShortCheck))
    return FALSE;

  Indicator i;
  rule.getCustomShortStop(i);
  IndicatorPlugin ip;
  ip.setIndicator(i);

  DataBase db;
  db.getChart(recordList);

  ip.setIndicatorInput(recordList);

  QList<PlotLine *> plotList;
  ip.calculate(plotList);
  if (! plotList.count())
  {
    qDebug("TesterStopPage::loadCustomShortStop: no PlotLine returned");
    return TRUE;
  }

  PlotLine *line = plotList.at(0);
  customShortStopLine = new PlotLine;
  customLongStopLine->copy(line);

  qDeleteAll(plotList);

  return FALSE;
}

bool Tester::loadCustomLongStop ()
{
  if (customLongStopLine)
  {
    delete customLongStopLine;
    customLongStopLine = 0;
  }
  
  if (! rule.getInt(TesterRule::CustomLongCheck))
    return FALSE;

  Indicator i;
  rule.getCustomLongStop(i);
  IndicatorPlugin ip;
  ip.setIndicator(i);

  DataBase db;
  db.getChart(recordList);

  ip.setIndicatorInput(recordList);

  QList<PlotLine *> plotList;
  ip.calculate(plotList);
  if (! plotList.count())
  {
    qDebug("Tester::loadCustomShortStop: no PlotLine returned");
    return TRUE;
  }

  PlotLine *line = plotList.at(0);
  customLongStopLine = new PlotLine;
  customLongStopLine->copy(line);
    
  qDeleteAll(plotList);

  return FALSE;
}

bool Tester::maximumLoss (bool flag, double enterPrice, double exitPrice)
{
  if (! rule.getInt(TesterRule::MaxLossCheck))
    return FALSE;

  if (! flag)
  {
    double t = enterPrice * (1 - (rule.getDouble(TesterRule::MaxLoss) / 100));
    if (exitPrice <= t)
      return TRUE;
  }
  else
  {
    double t = enterPrice * (1 + (rule.getDouble(TesterRule::MaxLoss) / 100));
    if (exitPrice >= t)
      return TRUE;
  }

  return FALSE;
}

bool Tester::calculateProfit (bool flag, double enterPrice, double exitPrice)
{
  if (! rule.getInt(TesterRule::ProfitCheck))
    return FALSE;

  if (! flag)
  {
    double t = enterPrice * (1 + (rule.getDouble(TesterRule::Profit) / 100));
    if (exitPrice >= t)
      return TRUE;
  }
  else
  {
    double t = enterPrice * (1 - (rule.getDouble(TesterRule::Profit) / 100));
    if (exitPrice <= t)
      return TRUE;
  }

  return FALSE;
}

bool Tester::calculateTrailing (bool flag, double exitPrice)
{
  if (! rule.getInt(TesterRule::TrailingCheck))
    return FALSE;

  if (! flag)
  {
    if (exitPrice < trailingHigh)
      trailingHigh = exitPrice;

    double t = ((exitPrice - trailingHigh) / trailingHigh) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= rule.getDouble(TesterRule::Trailing))
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
      if (t >= rule.getDouble(TesterRule::Trailing))
        return TRUE;
    }
  }

  return FALSE;
}

bool Tester::customStop (bool flag, int index)
{
  if (! flag)
  {
    if (rule.getInt(TesterRule::CustomLongCheck) && customLongStopLine)
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
    if (rule.getInt(TesterRule::CustomShortCheck) && customShortStopLine)
    {
      int i = customShortStopLine->getSize() - index;
      if (i > -1)
      {
        if (customShortStopLine->getData(i) == 1)
          return TRUE;
      }
    }
  }

  return FALSE;
}

void Tester::setTrailingHigh (double d)
{
  trailingHigh = d;
}

