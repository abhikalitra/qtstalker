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
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QProgressDialog>
#include <QFileInfo>
#include <QtDebug>
#include "Tester.h"
#include "IndicatorPlugin.h"


Tester::Tester (QString n) : QDialog (0, 0)
{
  ruleName = n;
  recordList = 0;
//  enterLongSignal.setAutoDelete(TRUE);
//  exitLongSignal.setAutoDelete(TRUE);
//  enterShortSignal.setAutoDelete(TRUE);
//  exitShortSignal.setAutoDelete(TRUE);
//  trades.setAutoDelete(TRUE);
  
  QString s = "Qtstalker Back Tester";
  s.append(": ");
  s.append(ruleName);
  setWindowTitle(s);

  tabs = new QTabWidget;
  
  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Save | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(buttonPressed(QAbstractButton *)));
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(tabs);
  vbox->addWidget(buttonBox);
  setLayout(vbox);

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

  loadRule();
}

Tester::Tester () : QDialog (0, 0)
{
  recordList = 0;
}

Tester::~Tester ()
{
  if (recordList)
    delete recordList;
}

void Tester::buttonPressed (QAbstractButton *button)
{
  switch (buttonBox->buttonRole(button))
  {
    case QDialogButtonBox::AcceptRole:
      test();
      break;
    case QDialogButtonBox::ApplyRole:
      saveRule();
      break;
    case QDialogButtonBox::RejectRole:
      reject();
      break;
    default:
      break;
  }
}

void Tester::saveRule ()
{
  rulePage->saveEditRule(TesterRulePage::EnterLong, ruleName);
  rulePage->saveEditRule(TesterRulePage::ExitLong, ruleName);
  rulePage->saveEditRule(TesterRulePage::EnterShort, ruleName);
  rulePage->saveEditRule(TesterRulePage::ExitShort, ruleName);

  stopPage->saveCustomStopRule(ruleName);

  QString s;
  s.append("/" + ruleName + "/rule");
  QFile f(s);
  if (! f.open(QIODevice::WriteOnly))
    return;
  QTextStream stream(&f);

  QStringList l;
  reportPage->getSummary(l);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    stream << "Trade=" << l[loop] << "\n";
  
  stream << "Maximum Loss Check=" << QString::number(stopPage->getMaximumLossCheck()) << "\n";
  stream << "Maximum Loss Long=" << QString::number(stopPage->getMaximumLossLong()) << "\n";
  stream << "Maximum Loss Short=" << QString::number(stopPage->getMaximumLossShort()) << "\n";
  stream << "Maximum Loss Edit=" << stopPage->getMaximumLossEdit() << "\n";
  stream << "Profit Check=" << QString::number(stopPage->getProfitCheck()) << "\n";
  stream << "Profit Long=" << QString::number(stopPage->getProfitLong()) << "\n";
  stream << "Profit Short=" << QString::number(stopPage->getProfitShort()) << "\n";
  stream << "Profit Edit=" << stopPage->getProfitEdit() << "\n";
  stream << "Trailing Check=" << QString::number(stopPage->getTrailingCheck()) << "\n";
  stream << "Trailing Long=" << QString::number(stopPage->getTrailingLong()) << "\n";
  stream << "Trailing Short=" << QString::number(stopPage->getTrailingShort()) << "\n";
  stream << "Trailing Edit=" << stopPage->getTrailingEdit() << "\n";
  stream << "TradeLong=" << QString::number(testPage->getTradeLong()) << "\n";
  stream << "TradeShort=" << QString::number(testPage->getTradeShort()) << "\n";
  stream << "Volume Percent=" << QString::number(testPage->getVolumePercent()) << "\n";
  stream << "Entry Com=" << QString::number(testPage->getEntryCom()) << "\n";
  stream << "Exit Com=" << QString::number(testPage->getExitCom()) << "\n";
  stream << "TradeDelay=" << QString::number(testPage->getTradeDelay()) << "\n";
  stream << "Price Field=" << testPage->getPriceField() << "\n";
  stream << "Bars=" << QString::number(testPage->getBars()) << "\n";
  stream << "Symbol=" << testPage->getSymbolPath() << "\n";
  stream << "Account=" << QString::number(testPage->getAccount()) << "\n";
  stream << "Compression=" << testPage->getBarLength() << "\n";
  stream << "CommissionType=" << QString::number(testPage->getCommissionType()) << "\n";
  stream << "FuturesMargin=" << QString::number(testPage->getMargin()) << "\n";
  
  f.close();
}

void Tester::loadRule ()
{
  reportPage->clear();

  rulePage->loadEditRule(TesterRulePage::EnterLong, ruleName);
  rulePage->loadEditRule(TesterRulePage::ExitLong, ruleName);
  rulePage->loadEditRule(TesterRulePage::EnterShort, ruleName);
  rulePage->loadEditRule(TesterRulePage::ExitShort, ruleName);

  stopPage->loadCustomStopRule(ruleName);

  QString s;
  s.append("/" + ruleName + "/rule");
  QFile f(s);
  if (! f.open(QIODevice::ReadOnly))
    return;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    QByteArray ba;
    ba.append(s);
    ba = ba.trimmed();
    s = ba;
    if (! s.length())
      continue;

    QStringList l2 = s.split("=");
    
    if (! l2[0].compare("Maximum Loss Check"))
    {
      stopPage->setMaximumLossCheck(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Maximum Loss Long"))
    {
      stopPage->setMaximumLossLong(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Maximum Loss Short"))
    {
      stopPage->setMaximumLossShort(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Maximum Loss Edit"))
    {
      stopPage->setMaximumLossEdit(l2[1]);
      continue;
    }

    if (! l2[0].compare("Profit Check"))
    {
      stopPage->setProfitCheck(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Profit Long"))
    {
      stopPage->setProfitLong(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Profit Short"))
    {
      stopPage->setProfitShort(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Profit Edit"))
    {
      stopPage->setProfitEdit(l2[1]);
      continue;
    }

    if (! l2[0].compare("Trailing Check"))
    {
      stopPage->setTrailingCheck(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Trailing Long"))
    {
      stopPage->setTrailingLong(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Trailing Short"))
    {
      stopPage->setTrailingShort(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Trailing Edit"))
    {
      stopPage->setTrailingEdit(l2[1]);
      continue;
    }
    
    if (! l2[0].compare("TradeLong"))
    {
      testPage->setTradeLong(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("TradeShort"))
    {
      testPage->setTradeShort(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("Volume Percent"))
    {
      testPage->setVolumePercent(l2[1].toInt());
      continue;
    }
  
    if (! l2[0].compare("Entry Com"))
    {
      testPage->setEntryCom(l2[1].toInt());
      continue;
    }
  
    if (! l2[0].compare("Exit Com"))
    {
      testPage->setExitCom(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("TradeDelay"))
    {
      testPage->setTradeDelay(l2[1].toInt());
      continue;
    }
  
    if (! l2[0].compare("Bars"))
    {
      testPage->setBars(l2[1].toInt());
      continue;
    }
    
    if (! l2[0].compare("Price Field"))
    {
      testPage->setPriceField(l2[1]);
      continue;
    }
  
    if (! l2[0].compare("Symbol"))
    {
      testPage->setSymbol(l2[1]);
      continue;
    }
    
    if (! l2[0].compare("Account"))
    {
      testPage->setAccount(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("Trade"))
    {
      TradeItem *trade = new TradeItem;
      reportPage->addTrade(l2[1], trade);
      trades.append(trade);
      continue;
    }

    if (! l2[0].compare("Compression"))
    {
      testPage->setBarLength(l2[1]);
      continue;
    }

    if (! l2[0].compare("CommissionType"))
    {
      testPage->setCommissionType(l2[1].toInt());
      continue;
    }

    if (! l2[0].compare("FuturesMargin"))
      testPage->setMargin(l2[1].toInt());
  }
  f.close();

  s = testPage->getSymbolPath();
//  if (db.open(s, index))
//  {
//    db.close();
//    return;
//  }

  QFileInfo fi(s);
  QString fn = fi.fileName();

//  index->getIndexItem(fn, item);
//  item.getType(chartType);
//  if (! chartType.compare(tr("Futures")))
//    item.getFuturesType(futuresType);

  int loop;
  for (loop = 0; loop < (int) trades.count(); loop++)
  {
    TradeItem *trade = trades.at(loop);
    trade->setCommissionType(testPage->getCommissionType());
    trade->setEntryCom(testPage->getEntryCom());
    trade->setExitCom(testPage->getExitCom());

    if (! chartType.compare(tr("Futures")))
    {
      trade->setStockFlag(FALSE);
      trade->setFuturesType(futuresType);
    }

    if (! loop)
      trade->setBalance(testPage->getAccount());
    else
    {
      TradeItem *ttrade = trades.at(loop - 1);
      trade->setBalance(ttrade->getBalance());
    }

    trade->calculateProfit();
  }

  chartPage->clear();

  reportPage->createSummary(trades, testPage->getAccount());
}

void Tester::exitDialog ()
{
  saveRule();
  accept();
}

int Tester::getVolume (int i, double d)
{
  double balance = d;
  int volume = 1;
  if (testPage->getVolumePercent() == 0)
    return volume;

  balance = balance * ((double) testPage->getVolumePercent() / 100.0);

  if (testPage->getMargin())
    volume = (int) (double) (balance / testPage->getMargin());
  else
    volume = (int) (double) (balance / getPrice(i));

  return volume;
}

double Tester::getPrice (int i)
{
  double price = 0;
  
  if (! testPage->getPriceField().compare(tr("Open")))
    price = recordList->getOpen(i);
  else
  {
    if (! testPage->getPriceField().compare(tr("Close")))
      price = recordList->getClose(i);
    else
      price = recordList->getLow(i) + ((recordList->getHigh(i) - recordList->getLow(i)) / 2);
  }
  
  return price;
}

QString Tester::newTest ()
{
  bool ok;
  QString s = QInputDialog::getText(this,
				    tr("New Backtest Rule"),
  				    tr("Enter new backtest rule name."),
  				    QLineEdit::Normal,
				    tr("NewRule"),
				    &ok);

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

  s.append("/" + selection);
  QDir dir(s);
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return selection;
  }

  if (! dir.mkdir(s))
  {
    qDebug() << "TestPage::newTest:can't create dir " << s;
    return selection;
  }
  
  if (! dir.mkdir(s + "/el"))
  {
    qDebug("TestPage::newTest:can't create el dir");
    return selection;
  }
  
  if (! dir.mkdir(s + "/xl"))
  {
    qDebug("TestPage::newTest:can't create xl dir");
    return selection;
  }
  
  if (! dir.mkdir(s + "/es"))
  {
    qDebug("TestPage::newTest:can't create es dir");
    return selection;
  }
  
  if (! dir.mkdir(s + "/xs"))
  {
    qDebug("TestPage::newTest:can't create xs dir");
    return selection;
  }
  
  return selection;
}

void Tester::loadSignals ()
{
  enterLongSignal.clear();
  exitLongSignal.clear();
  enterShortSignal.clear();
  exitShortSignal.clear();

  // open the CUS plugin
  QString plugin("CUS");
  IndicatorPlugin *plug = new IndicatorPlugin;

  int loop;
  for (loop = 0; loop < 4; loop++)
  {
    QStringList l;
    QString s;
    switch (loop)
    {
      case 0:
        s = rulePage->getEditRule(TesterRulePage::EnterLong);
        break;
      case 1:
        s = rulePage->getEditRule(TesterRulePage::ExitLong);
        break;
      case 2:
        s = rulePage->getEditRule(TesterRulePage::EnterShort);
        break;
      case 3:
        s = rulePage->getEditRule(TesterRulePage::ExitShort);
        break;
      default:
        break;
    }
    l = s.split("\n");
    if (! l.count())
      continue;

//    plug->setCustomFunction(l);
  
    // load the CUS plugin and calculate
    plug->setIndicatorInput(recordList);
//    plug->calculate();
//    PlotLine *line = plug->getLine(0);
    PlotLine *line = 0;
//    if (! line)
//    {
//      qDebug("Tester::loadSignals: no PlotLine returned");
//      continue;
//    }

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
  }
}

void Tester::test ()
{
  if (! testPage->getTradeLong() && ! testPage->getTradeShort())
    return;

  equity = (double) testPage->getAccount();
  if (equity == 0)
    return;

  QString symbol = testPage->getSymbol();
  if (! symbol.length())
    return;

  QString path = testPage->getSymbolPath();
  QDir dir;
  if (! dir.exists(path))
    return;
  
//  if (db.open(path, index))
//  {
//    db.close();
//    return;
//  }

  QFileInfo fi(path);
  QString fn = fi.fileName();

//  item.getType(chartType);
//  if (! chartType.compare(tr("Futures")))
//    item.getFuturesType(futuresType);
  
//  db.setBarLength((BarData::BarLength) testPage->getBarLengthIndex());
//  db.setBarRange(testPage->getBars());
  if (recordList)
    delete recordList;
  recordList = new BarData(path);
  QDateTime dt = QDateTime::currentDateTime();
//  db.getHistory(recordList, dt);

  chartPage->clear();

  loadSignals();

  if (stopPage->loadCustomLongStop(recordList))
    return;
  
  if (stopPage->loadCustomShortStop(recordList))
    return;

  reportPage->clear();
  trades.clear();

  QProgressDialog prog(tr("Testing..."),
                       tr("Cancel"),
                       1,
		       testPage->getBars(),
		       this);
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

    if (testPage->getTradeLong())
    {
      Setting *set = enterLongSignal[key];
      if (set)
        enterTrade(TradeItem::Long);
    }

    if (testPage->getTradeShort())
    {
      Setting *set = enterShortSignal[key];
      if (set)
        enterTrade(TradeItem::Short);
    }
  }

  reportPage->createSummary(trades, testPage->getAccount());

  chartPage->updateChart(recordList, trades, testPage->getAccount());

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
  if (currentRecord + testPage->getTradeDelay() < recordList->count())
    buyRecord = currentRecord + testPage->getTradeDelay();
  else
    buyRecord = currentRecord;

  QDateTime td;
  recordList->getDate(buyRecord, td);
  trade->setEnterDate(td);

  double enterPrice = getPrice(buyRecord);
  trade->setEnterPrice(enterPrice);

  stopPage->setTrailingHigh(getPrice(buyRecord));

  if (! trades.count())
  {
    trade->setVolume(getVolume(buyRecord, testPage->getAccount()));
    trade->setBalance(testPage->getAccount());
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

  trade->setCommissionType(testPage->getCommissionType());

  trade->setEntryCom(testPage->getEntryCom());

  trade->setExitCom(testPage->getExitCom());

  if (! chartType.compare(tr("Futures")))
  {
    trade->setStockFlag(FALSE);
    trade->setFuturesType(futuresType);
    trade->setMargin(testPage->getMargin());
  }

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
      if (loop + testPage->getTradeDelay() < recordList->count())
        sellRecord = loop + testPage->getTradeDelay();
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
      tflag = stopPage->maximumLoss(FALSE, enterPrice, recordList->getLow(loop));
    else
      tflag = stopPage->maximumLoss(TRUE, enterPrice, recordList->getHigh(loop));
    if (tflag)
    {
      int sellRecord = 0;
      if (loop + testPage->getTradeDelay() < recordList->count())
        sellRecord = loop + testPage->getTradeDelay();
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
      tflag = stopPage->profit(FALSE, enterPrice, recordList->getHigh(loop));
    else
      tflag = stopPage->profit(TRUE, enterPrice, recordList->getLow(loop));
    if (tflag)
    {
      int sellRecord = 0;
      if (loop + testPage->getTradeDelay() < recordList->count())
        sellRecord = loop + testPage->getTradeDelay();
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
      tflag = stopPage->customStop(FALSE, loop);
    else
      tflag = stopPage->customStop(TRUE, loop);
    if (tflag)
    {
      int sellRecord = 0;
      if (loop + testPage->getTradeDelay() < recordList->count())
        sellRecord = loop + testPage->getTradeDelay();
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
      tflag = stopPage->trailing(FALSE, recordList->getLow(loop));
    else
      tflag = stopPage->trailing(TRUE, recordList->getHigh(loop));
    if (tflag)
    {
      int sellRecord = 0;
      if (loop + testPage->getTradeDelay() < recordList->count())
        sellRecord = loop + testPage->getTradeDelay();
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

