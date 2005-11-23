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
  enterLongSignal.setAutoDelete(TRUE);
  exitLongSignal.setAutoDelete(TRUE);
  enterShortSignal.setAutoDelete(TRUE);
  exitShortSignal.setAutoDelete(TRUE);
  trades.setAutoDelete(TRUE);
  
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
}

Tester::~Tester ()
{
  if (recordList)
    delete recordList;
    
  if (customLongStopLine)
    delete customLongStopLine;
  
  if (customShortStopLine)
    delete customShortStopLine;
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

  QLabel *label = new QLabel(tr("Trade Delay"), gbox);
    
  tradeDelay = new QSpinBox(1, 999999, 1, gbox);
  tradeDelay->setValue(1);

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
  
  label = new QLabel(tr("Compression"), gbox);
  
  BarData bd;
  compression = new QComboBox(gbox);
  bd.getBarCompressionList(compressionList);
  compression->insertStringList(compressionList, -1);
  compression->setCurrentItem(6);

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

//  bool logFlag = config.getData(Config::LogScale).toInt();
  bool scaleToScreenFlag = config.getData(Config::ScaleToScreen).toInt();
  
  equityPlot = new Plot (split);
  equityPlot->setGridFlag(TRUE);
  equityPlot->setScaleToScreen(scaleToScreenFlag);
//  equityPlot->setLogScale(logFlag);
  equityPlot->setPixelspace(5);
  equityPlot->setIndex(0);
  equityPlot->setDateFlag(TRUE);
  equityPlot->setInfoFlag(FALSE);
  equityPlot->setCrosshairsFlag (FALSE); // turn off crosshairs
  QObject::connect(this, SIGNAL(signalIndex(int)), equityPlot, SLOT(setIndex(int)));
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  equityPlot->setPlotFont(font);
  
/*
  plot = new Plot (split);
  plot->setGridFlag(TRUE);
  plot->setScaleToScreen(scaleToScreenFlag);
  plot->setLogScale(logFlag);
  plot->setPixelspace(5);
  plot->setIndex(0);
  plot->setDateFlag(TRUE);
  plot->setInfoFlag(FALSE);
  plot->setCrosshairsFlag (FALSE); // turn off crosshairs
  QObject::connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  plot->setPlotFont(font);
*/

  slider = new QSlider(w);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderChanged(int)));
  vbox->addWidget(slider);
    
  addTab(w, tr("Chart"));
}

void Tester::slotSliderChanged (int v)
{
  emit signalIndex(v);
//  plot->draw();
  equityPlot->draw();
}

/*
void Tester::updateEquityCurve (int status, double enterPrice, exitPrice)
{
  if (status == 0)
  {
    equityCurve->append(equity);
    return;
  }

  double profit = 0;
  double bal = equity;
  QString type;

  if (status == -1)
    profit = (enterPrice - exitPrice) * volume;
  else
    profit = (exitPrice - enterPrice) * volume;

  if (! chartType.compare(tr("Futures")))
    profit = fd.getRate() * profit;

  bal = bal + profit;

  equityCurve->append(bal);
}
*/

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

  stream << edit->getText() << "\n";
  
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
  stream << "TradeDelay=" << tradeDelay->text() << "\n";
  stream << "Price Field=" << priceField->currentText() << "\n";
  stream << "Bars=" << bars->text() << "\n";
  stream << "Symbol=" << symbolButton->getPath() << "\n";
  stream << "Account=" << account->text() << "\n";
  stream << "Compression=" << compression->currentText() << "\n";
  
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
    
    if (! l2[0].compare("TradeDelay"))
    {
      tradeDelay->setValue(l2[1].toInt());
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

    if (! l2[0].compare("Compression"))
    {
      compression->setCurrentText(l2[1]);
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

int Tester::getVolume (int i, double d)
{
  double balance = d;
  int volume = 1;
  if (volumePercent->value() == 0)
    return volume;

  balance = balance * ((double) volumePercent->value() / 100.0);

  if (margin->value())
    volume = (int) (double) (balance / margin->value());
  else
    volume = (int) (double) (balance / getPrice(i));

  return volume;
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
/*
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
  plot->addIndicator(i);

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
*/
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

  QStringList l = QStringList::split("\n", customShortStopEdit->getText(), FALSE);

  QString plugin("CUS");  
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return TRUE;
  }

  plug->setCustomFunction(l);
  
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

  QStringList l = QStringList::split("\n", customLongStopEdit->getText(), FALSE);
  
  QString plugin("CUS");  
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return TRUE;
  }

  plug->setCustomFunction(l);
  
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

  stream << customShortStopEdit->getText() << "\n";
  
  f.close();
  
  s = config.getData(Config::TestPath) + "/" + ruleName + "/customLongStop";
  f.setName(s);
  if (! f.open(IO_WriteOnly))
    return;

  stream << customLongStopEdit->getText() << "\n";
  
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
//  plot->setScaleToScreen(d);
  equityPlot->setScaleToScreen(d);
  equityPlot->draw();
//  plot->draw();
}

void Tester::slotLogScaling (bool)
{
//  plot->setLogScale(d);
//  equityPlot->setLogScale(d);
//  equityPlot->draw();
//  plot->draw();
}

//*******************************
//******************* TEST
//**************************

void Tester::loadSignals ()
{
  enterLongSignal.clear();
  exitLongSignal.clear();
  enterShortSignal.clear();
  exitShortSignal.clear();

  // open the CUS plugin
  QString plugin("CUS");
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return;
  }

  int loop;
  for (loop = 0; loop < 4; loop++)
  {
    QStringList l;
    switch (loop)
    {
      case 0:
        l = QStringList::split("\n", enterLongEdit->getText());
        break;
      case 1:
        l = QStringList::split("\n", exitLongEdit->getText());
        break;
      case 2:
        l = QStringList::split("\n", enterShortEdit->getText());
        break;
      case 3:
        l = QStringList::split("\n", exitShortEdit->getText());
        break;
      default:
        break;
    }
  
    if (! l.count())
      continue;

    plug->setCustomFunction(l);
  
    // load the CUS plugin and calculate
    plug->setIndicatorInput(recordList);
    plug->calculate();
    Indicator *i = plug->getIndicator();
    PlotLine *line = i->getLine(0);
    if (! line)
    {
      qDebug("Tester::loadSignals: no PlotLine returned");
      continue;
    }
    
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
          QString key;
          recordList->getDate(loop2).getDateTimeString(FALSE, key);
          switch (loop)
          {
            case 0:
              enterLongSignal.replace(key, trade);
              break;
            case 1:
              exitLongSignal.replace(key, trade);
              break;
            case 2:
              enterShortSignal.replace(key, trade);
              break;
            case 3:
              exitShortSignal.replace(key, trade);
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

  config.closePlugin(plugin);
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

  db->setBarCompression((BarData::BarCompression) compressionList.findIndex(compression->currentText()));
  db->setBarRange(bars->value());
  if (recordList)
    delete recordList;
  recordList = new BarData;
  db->getHistory(recordList);
  config.closePlugin(plugin);

  equityPlot->clear();
    
  loadSignals();

  if (loadCustomLongStop())
    return;
  
  if (loadCustomShortStop())
    return;

  while (tradeList->numRows())
    tradeList->removeRow(0);

  trades.clear();

  QProgressDialog prog(tr("Testing..."),
                       tr("Cancel"),
		       bars->value(),
		       this,
		       "progress",
		       TRUE);
  prog.show();
  
  this->setEnabled(FALSE);
  
  currentRecord = 0;
  for (; currentRecord < (int) recordList->count(); currentRecord++)
  {
    prog.setProgress(currentRecord);
    emit message(QString());
    if (prog.wasCancelled())
      break;
  
    BarDate dt = recordList->getDate(currentRecord);
    QString key;
    dt.getDateTimeString(FALSE, key);

    if (tradeLong->isChecked())
    {
      Setting *set = enterLongSignal[key];
      if (set)
        enterLong();
    }

    if (tradeShort->isChecked())
    {
      Setting *set = enterShortSignal[key];
      if (set)
        enterShort();
    }
  }

  updateTradeList();

  createSummary();
  
//  updateChart();

  updateEquityCurve();

  config.closePlugin(plugin);
  
  this->setEnabled(TRUE);
}

void Tester::enterLong ()
{
  Setting *trade = new Setting;
  trade->setData("type", "L");
  trade->setData("enterSignal", tr("Enter Long"));

  int buyRecord = 0;
  if (currentRecord + tradeDelay->value() < recordList->count())
    buyRecord = currentRecord + tradeDelay->value();
  else
    buyRecord = currentRecord;

  QString s;
  recordList->getDate(buyRecord).getDateTimeString(FALSE, s);
  trade->setData("enterDate", s);

  double enterPrice = getPrice(buyRecord);
  trade->setData("enterPrice", QString::number(enterPrice));

  trailingHigh = getPrice(buyRecord);

  equity = equity - entryCom->value();

  trade->setData("volume", QString::number(getVolume(buyRecord, equity)));

  int loop = buyRecord;
  for (; loop < (int) recordList->count(); loop++)
  {
    BarDate dt = recordList->getDate(loop);
    QString key;
    dt.getDateTimeString(FALSE, key);

    Setting *set = exitLongSignal[key];
    if (set)
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Exit Long"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }

    double exitPrice = getPrice(loop);

    if (maximumLoss(FALSE, enterPrice, exitPrice))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Max Loss"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }

    if (profit(FALSE, enterPrice, exitPrice))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Profit"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }

    if (customStop(FALSE, loop))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("CUS Stop"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }
    
    if (trailing(FALSE, exitPrice))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Trailing"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }
  }

  s = trade->getData("exitDate");
  if (! s.length())
  {
    recordList->getDate(recordList->count() - 1).getDateTimeString(FALSE, s);
    trade->setData("exitDate", s);
    trade->setData("exitPrice", QString::number(getPrice(recordList->count() - 1)));
    trade->setData("exitSignal", tr("End of test"));
    trades.append(trade);
    currentRecord = loop;
  }
}

void Tester::enterShort ()
{
  Setting *trade = new Setting;
  trade->setData("type", "S");
  trade->setData("enterSignal", tr("Enter Short"));

  int buyRecord = 0;
  if (currentRecord + tradeDelay->value() < recordList->count())
    buyRecord = currentRecord + tradeDelay->value();
  else
    buyRecord = currentRecord;

  QString s;
  recordList->getDate(buyRecord).getDateTimeString(FALSE, s);
  trade->setData("enterDate", s);

  double enterPrice = getPrice(buyRecord);
  trade->setData("enterPrice", QString::number(enterPrice));

  trailingHigh = getPrice(buyRecord);

  equity = equity - entryCom->value();

  trade->setData("volume", QString::number(getVolume(buyRecord, equity)));

  int loop = buyRecord;
  for (; loop < (int) recordList->count(); loop++)
  {
    BarDate dt = recordList->getDate(loop);
    QString key;
    dt.getDateTimeString(FALSE, key);

    Setting *set = exitShortSignal[key];
    if (set)
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Exit Short"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }

    double exitPrice = getPrice(loop);

    if (maximumLoss(TRUE, enterPrice, exitPrice))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Max Loss"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }

    if (profit(TRUE, enterPrice, exitPrice))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Profit"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }

    if (customStop(TRUE, loop))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("CUS Stop"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }
    
    if (trailing(TRUE, exitPrice))
    {
      int sellRecord = 0;
      if (loop + tradeDelay->value() < recordList->count())
        sellRecord = loop + tradeDelay->value();
      else
        sellRecord = loop;

      recordList->getDate(sellRecord).getDateTimeString(FALSE, s);
      trade->setData("exitDate", s);
      trade->setData("exitPrice", QString::number(getPrice(sellRecord)));
      trade->setData("exitSignal", tr("Trailing"));
      trades.append(trade);
      currentRecord = loop - 1;
      break;
    }
  }

  s = trade->getData("exitDate");
  if (! s.length())
  {
    recordList->getDate(recordList->count() - 1).getDateTimeString(FALSE, s);
    trade->setData("exitDate", s);
    trade->setData("exitPrice", QString::number(getPrice(recordList->count() - 1)));
    trade->setData("exitSignal", tr("End of test"));
    trades.append(trade);
    currentRecord = loop;
  }
}

bool Tester::maximumLoss (bool flag, double enterPrice, double exitPrice)
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

bool Tester::profit (bool flag, double enterPrice, double exitPrice)
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

bool Tester::trailing (bool flag, double exitPrice)
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

bool Tester::customStop (bool flag, int index)
{
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

  return FALSE;
}

void Tester::updateTradeList ()
{
  int loop;
  for (loop = 0; loop < (int) trades.count(); loop++)
  {
    Setting *trade = trades.at(loop);

    double profit = 0;
    QString type = trade->getData("type");
    if (! type.compare("S"))
      profit = (trade->getDouble("enterPrice") - trade->getDouble("exitPrice")) * trade->getInt("volume");
    else
      profit = (trade->getDouble("exitPrice") - trade->getDouble("enterPrice")) * trade->getInt("volume");

    if (! chartType.compare(tr("Futures")))
      profit = fd.getRate() * profit;

    equity = equity + profit;

    equity = equity - exitCom->value();

    trade->setData("equity", QString::number(equity));

    QString s;
    tradeList->setNumRows(tradeList->numRows() + 1);
    tradeList->setText(tradeList->numRows() - 1, 0, type);
    tradeList->setText(tradeList->numRows() - 1, 1, trade->getData("enterDate"));
    tradeList->setText(tradeList->numRows() - 1, 2, trade->getData("enterPrice"));
    tradeList->setText(tradeList->numRows() - 1, 3, trade->getData("exitDate"));
    tradeList->setText(tradeList->numRows() - 1, 4, trade->getData("exitPrice"));
    tradeList->setText(tradeList->numRows() - 1, 5, trade->getData("exitSignal"));
    tradeList->setText(tradeList->numRows() - 1, 6, QString::number(profit));
    tradeList->setText(tradeList->numRows() - 1, 7, QString::number(equity));
    tradeList->setText(tradeList->numRows() - 1, 8, trade->getData("volume"));
  }
}

void Tester::updateEquityCurve ()
{
  if (! trades.count())
    return;

  equityPlot->setData(recordList);

  PlotLine *line = new PlotLine;
  QString s = "green";
  line->setColor(s);

  int loop = 0;
  int loop2 = 0;
  double equity = account->value();

  for (; loop < (int) recordList->count(); loop++)
  {
    BarDate dt = recordList->getDate(loop);

    Setting *trade = trades.at(loop2);
    BarDate dt2;
    QString s = trade->getData("exitDate");
    dt2.setDate(s);

    if (dt.getDateValue() == dt2.getDateValue())
    {
      equity = trade->getDouble("equity");

      loop2++;
      if (loop2 >= (int) trades.count())
        loop2 = trades.count() - 1;
    }
    
    line->append(equity);
  }
    
  //set up indicator
  Indicator *i = new Indicator;
  QString str = "equity";
  i->setName(str);
  i->addLine(line);
  str = i->getName();
  equityPlot->addIndicator(i);

  equityPlot->draw();
}
