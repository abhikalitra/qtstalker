/*
 *  Qtstalker stock charter
 *
 *  Copyright (c) 2001 Stefan S. Stratigakos
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
#include <qtooltip.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qlibrary.h>
#include <qvgroupbox.h>
#include <qinputdialog.h>
#include <qsplitter.h>
#include <qheader.h>
#include "Tester.h"
#include "edit.xpm"
#include "delete.xpm"
#include "indicator.xpm"
#include "stop.xpm"
#include "ok.xpm"
#include "enterlong.xpm"
#include "entershort.xpm"
#include "exitlong.xpm"
#include "exitshort.xpm"
#include "EditDialog.h"
#include "Plugin.h"
#include "SettingView.h"
#include "FuturesData.h"

Tester::Tester (Config *c, QString n) : QDialog (0, 0, FALSE)
{
  config = c;
  ruleName = n;
  enterLongIndicators.setAutoDelete(TRUE);
  exitLongIndicators.setAutoDelete(TRUE);
  enterShortIndicators.setAutoDelete(TRUE);
  exitShortIndicators.setAutoDelete(TRUE);
  enterLongAlerts.setAutoDelete(TRUE);
  exitLongAlerts.setAutoDelete(TRUE);
  enterShortAlerts.setAutoDelete(TRUE);
  exitShortAlerts.setAutoDelete(TRUE);

  setCaption ("Back Tester");

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QGridLayout *tb = new QGridLayout(vbox, 1, 3);
  tb->setSpacing(1);

  QToolButton *button = new QToolButton(this);
  QToolTip::add(button, tr("OK"));
  button->setPixmap(QPixmap(ok));
  connect(button, SIGNAL(clicked()), this, SLOT(exitDialog()));
  button->setMaximumWidth(30);
  button->setAutoRaise(TRUE);
  tb->addWidget(button, 0, 0);

  button = new QToolButton(this);
  QToolTip::add(button, tr("Cancel"));
  button->setPixmap(QPixmap(stop));
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));
  button->setMaximumWidth(30);
  button->setAutoRaise(TRUE);
  tb->addWidget(button, 0, 1);

  tabs = new QTabWidget(this);
  vbox->addWidget(tabs);

  createFormulaPage();

  createStopPage();

  createTestPage();

  createReportPage();

  createChartPage();

  loadRule();
  
  showRule(0);
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

  QGridLayout *tb = new QGridLayout(vbox, 1, 8);
  tb->setSpacing(1);

  buttonGroup = new QButtonGroup(w);
  buttonGroup->setExclusive(TRUE);
  buttonGroup->hide();

  QToolButton *button = new QToolButton(w);
  QToolTip::add(button, tr("Enter Long Rule"));
  button->setAutoRaise(TRUE);
  button->setMaximumWidth(30);
  button->setPixmap(QPixmap(enterlong));
  button->setToggleButton(TRUE);
  tb->addWidget(button, 0, 0);
  buttonGroup->insert(button, 0);

  button = new QToolButton(w);
  QToolTip::add(button, tr("Exit Long Rule"));
  button->setAutoRaise(TRUE);
  button->setMaximumWidth(30);
  button->setPixmap(QPixmap(exitlong));
  button->setToggleButton(TRUE);
  tb->addWidget(button, 0, 1);
  buttonGroup->insert(button, 1);

  button = new QToolButton(w);
  QToolTip::add(button, tr("Enter Short Rule"));
  button->setAutoRaise(TRUE);
  button->setMaximumWidth(30);
  button->setPixmap(QPixmap(entershort));
  button->setToggleButton(TRUE);
  tb->addWidget(button, 0, 2);
  buttonGroup->insert(button, 2);

  button = new QToolButton(w);
  QToolTip::add(button, tr("Exit Short Rule"));
  button->setAutoRaise(TRUE);
  button->setMaximumWidth(30);
  button->setPixmap(QPixmap(exitshort));
  button->setToggleButton(TRUE);
  tb->addWidget(button, 0, 3);
  buttonGroup->insert(button, 3);

  addIndicatorButton = new QToolButton(w);
  QToolTip::add(addIndicatorButton, tr("Add Indicator"));
  addIndicatorButton->setPixmap(QPixmap(indicator));
  connect(addIndicatorButton, SIGNAL(clicked()), this, SLOT(addIndicator()));
  addIndicatorButton->setMaximumWidth(30);
  addIndicatorButton->setAutoRaise(TRUE);
  tb->addWidget(addIndicatorButton, 0, 4);

  editIndicatorButton = new QToolButton(w);
  QToolTip::add(editIndicatorButton, tr("Edit Indicator"));
  editIndicatorButton->setPixmap(QPixmap(edit));
  connect(editIndicatorButton, SIGNAL(clicked()), this, SLOT(editIndicator()));
  editIndicatorButton->setMaximumWidth(30);
  editIndicatorButton->setAutoRaise(TRUE);
  tb->addWidget(editIndicatorButton, 0, 5);
  editIndicatorButton->setEnabled(FALSE);

  deleteIndicatorButton = new QToolButton(w);
  QToolTip::add(deleteIndicatorButton, tr("Delete Indicator"));
  deleteIndicatorButton->setPixmap(QPixmap(deletefile));
  connect(deleteIndicatorButton, SIGNAL(clicked()), this, SLOT(deleteIndicator()));
  deleteIndicatorButton->setMaximumWidth(30);
  deleteIndicatorButton->setAutoRaise(TRUE);
  tb->addWidget(deleteIndicatorButton, 0, 6);
  deleteIndicatorButton->setEnabled(FALSE);

  indicatorList = new QListView(w);
  indicatorList->addColumn("", 150);
  indicatorList->setColumnWidthMode(0, QListView::Maximum);
  indicatorList->setSelectionMode(QListView::Single);
  connect(indicatorList, SIGNAL(clicked(QListViewItem *)), this, SLOT(indicatorSelected(QListViewItem *)));
  vbox->addWidget(indicatorList);
  
  connect(buttonGroup, SIGNAL(clicked(int)), this, SLOT(showRule(int)));
  buttonGroup->setButton(0);
  showRule(0);

  tabs->addTab(w, tr("Rules"));
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

  tabs->addTab(w, tr("Stops"));
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

  gbox = new QVGroupBox(tr("Date Range"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 0, 1);

  QLabel *label = new QLabel(tr("Start Date"), gbox);

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
  entryCom->setValue(25);

  label = new QLabel(tr("Exit Commission"), gbox);

  exitCom = new QSpinBox(0, 999999, 1, gbox);
  exitCom->setValue(25);

  label = new QLabel(tr("Account Balance"), gbox);

  account = new QSpinBox(0, 999999, 1, gbox);
  account->setValue(10000);
  
  label = new QLabel(tr("Futures Margin"), gbox);

  margin = new QSpinBox(0, 999999, 1, gbox);

  gbox = new QVGroupBox(tr("Symbol"), w);
  gbox->setInsideSpacing(2);
  grid->addWidget(gbox, 1, 1);

  symbolButton = new QPushButton(gbox);
  connect(symbolButton, SIGNAL(clicked()), this, SLOT(symbolButtonPressed()));

  gbox = new QVGroupBox(tr("Volume"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 2, 0);

  label = new QLabel(tr("Account %"), gbox);

  volumePercent = new QSpinBox(0, 100, 1, gbox);
  
  testButton = new QPushButton(tr("Perform Test"), w);
  connect(testButton, SIGNAL(clicked()), this, SLOT(test()));
  vbox->addWidget(testButton);

  tabs->addTab(w, tr("Testing"));
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
  header->setLabel(5, tr("Signal"), 70);
  header->setLabel(6, tr("Profit"), 70);
  header->setLabel(7, tr("Account"), 70);
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

  tabs->addTab(w, tr("Reports"));
}

void Tester::createChartPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QSplitter *split = new QSplitter(w);
  split->setOrientation(Vertical);
  vbox->addWidget(split);

  equityPlot = new Plot (split);
  equityPlot->setDateFlag(TRUE);
  equityPlot->setChartType("Line");
  equityPlot->setMainFlag(FALSE);
  equityPlot->clear();

  closePlot = new Plot (split);
  closePlot->setDateFlag(TRUE);
  closePlot->setChartType("Line");
  closePlot->setMainFlag(TRUE);
  closePlot->clear();

  slider = new QSlider(w);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
  QToolTip::add(slider, tr("Pan Chart"));
  vbox->addWidget(slider);

  tabs->addTab(w, tr("Charts"));
}

void Tester::addIndicator ()
{
  bool ok;
  QString name = QInputDialog::getText(tr("Indicator Name"),
  				       tr("Enter a unique name for this indicator."),
                                       QLineEdit::Normal,
				       tr("New Indicator"),
				       &ok,
				       this);
  if (! ok || ! name.length())
    return;

  int id = buttonGroup->id(buttonGroup->selected());
  Indicator *i = 0;
  switch (id)
  {
    case 0:
      i = enterLongIndicators[name];
      break;
    case 1:
      i = exitLongIndicators[name];
      break;
    case 2:
      i = enterShortIndicators[name];
      break;
    default:
      i = exitShortIndicators[name];
      break;
  }

  if (i)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  QString ind = QInputDialog::getItem(tr("Add Indicator"),
  				      tr("Choose an indicator to add"),
				      config->getIndicatorPlugins(),
				      0,
				      FALSE,
				      &ok,
				      this);

  if (! ok || ! ind.length())
    return;

  i = new Indicator;
  Setting *set = new Setting;

  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("Edit Indicator"));

  QString s = config->getData(Config::IndicatorPluginPath);
  s.append("/");
  s.append(ind);
  s.append(".so");

  QLibrary *lib = new QLibrary(s);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("create");
  if (so)
  {
    Plugin *plug = (*so)();

    QStringList key = plug->getKeyList();

    int loop;
    for(loop = 0; loop < (int) key.count(); loop++)
    {
      set->set(key[loop], plug->getData(key[loop]), plug->getType(key[loop]));
      set->setList(key[loop], plug->getList(key[loop]));
    }

    delete plug;
  }

  delete lib;

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QStringList key = set->getKeyList();
    int loop;
    for(loop = 0; loop < (int) key.count(); loop++)
    {
      i->set(key[loop], set->getData(key[loop]), set->getType(key[loop]));
      i->setList(key[loop], set->getList(key[loop]));
    }

    i->set("Name", name, Setting::None);

    switch (id)
    {
      case 0:
        enterLongIndicators.insert(name, i);
        enterLongAlerts.insert(name, new Setting);
	break;
      case 1:
        exitLongIndicators.insert(name, i);
        exitLongAlerts.insert(name, new Setting);
	break;
      case 2:
        enterShortIndicators.insert(name, i);
        enterShortAlerts.insert(name, new Setting);
	break;
      default:
        exitShortIndicators.insert(name, i);
        exitShortAlerts.insert(name, new Setting);
	break;
    }

    item = new QListViewItem(indicatorList, name);
  }

  delete dialog;
  delete set;
}

void Tester::editIndicator ()
{
  item = indicatorList->selectedItem();
  if (! item)
    return;

  QString name = item->text(0);

  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("Edit Indicator"));
  
  int id = buttonGroup->id(buttonGroup->selected());
  Indicator *i = 0;
  switch (id)
  {
    case 0:
      i = enterLongIndicators[item->text(0)];
      break;
    case 1:
      i = exitLongIndicators[item->text(0)];
      break;
    case 2:
      i = enterShortIndicators[item->text(0)];
      break;
    default:
      i = exitShortIndicators[item->text(0)];
      break;
  }

  QStringList key = i->getKeyList();
  int loop;
  Setting *set = new Setting;
  for(loop = 0; loop < (int) key.count(); loop++)
  {
    set->set(key[loop], i->getData(key[loop]), i->getType(key[loop]));
    set->setList(key[loop], i->getList(key[loop]));
  }

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QStringList key = set->getKeyList();
    int loop;
    for(loop = 0; loop < (int) key.count(); loop++)
    {
      i->set(key[loop], set->getData(key[loop]), set->getType(key[loop]));
      i->setList(key[loop], set->getList(key[loop]));
    }
  }

  delete dialog;
  delete set;
}

void Tester::deleteIndicator ()
{
  item = indicatorList->selectedItem();
  if (! item)
    return;

  int id = buttonGroup->id(buttonGroup->selected());
  switch (id)
  {
    case 0:
      enterLongIndicators.remove(item->text(0));
      enterLongAlerts.remove(item->text(0));
      break;
    case 1:
      exitLongIndicators.remove(item->text(0));
      exitLongAlerts.remove(item->text(0));
      break;
    case 2:
      enterShortIndicators.remove(item->text(0));
      enterShortAlerts.remove(item->text(0));
      break;
    default:
      exitShortIndicators.remove(item->text(0));
      exitShortAlerts.remove(item->text(0));
      break;
  }

  delete item;
  
  indicatorSelected(0);
}

void Tester::indicatorSelected (QListViewItem *i)
{
  if (i)
  {
    editIndicatorButton->setEnabled(TRUE);
    deleteIndicatorButton->setEnabled(TRUE);
  }
  else
  {
    editIndicatorButton->setEnabled(FALSE);
    deleteIndicatorButton->setEnabled(FALSE);
  }
}

void Tester::test ()
{
  if (! tradeLong->isChecked() && ! tradeShort->isChecked())
    return;

  equity = account->text().toDouble();
  if (equity == 0)
    return;

  QString symbol = symbolButton->text();
  if (! symbol.length())
    return;

  QDateTime sd(startDate->date());
  if (! sd.isValid())
    return;

  QDateTime ed(endDate->date());
  if (! ed.isValid())
    return;

  QString s = config->getData(Config::DataPath);
  s.append("/");
  s.append(symbol);
  db = new ChartDb;
  if (db->openChart(s))
  {
    delete db;
    qDebug("Tester: Cant open db");
    return;
  }

  while (tradeList->numRows())
    tradeList->removeRow(0);

  db->getHistory(ChartDb::Daily, sd);
  
  loadIndicators(0, db);
  loadIndicators(1, db);
  loadIndicators(2, db);
  loadIndicators(3, db);
  
  loadEnterLongAlerts ();
  loadExitLongAlerts ();
  loadEnterShortAlerts ();
  loadExitShortAlerts ();
  
  clearAlertCounts();
  
  for (testLoop = 0; testLoop < db->getDataSize(); testLoop++)
  {
    currentRecord = db->getRecordIndex(testLoop);
    QDateTime td = QDateTime::fromString(currentRecord->getDateTime("Date"), ISODate);
    if (td >= sd)
      break;
  }

  status = 0;
  for (; testLoop < db->getDataSize(); testLoop++)
  {
    currentRecord = db->getRecordIndex(testLoop);

    QDateTime dt = QDateTime::fromString(currentRecord->getDateTime("Date"), ISODate);
    if (dt > ed)
      break;

    checkAlerts();

    if (status == 0)
    {
      if (enterLongAlerts.count() && tradeLong->isChecked())
      {
        if ((int) enterLongAlerts.count() == enterLongCount)
        {
          enterLong();
          continue;
        }
      }

      if (enterShortAlerts.count() && tradeShort->isChecked())
      {
        if ((int) enterShortAlerts.count() == enterShortCount)
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
        if ((int) enterShortAlerts.count() == enterShortCount)
        {
          enterShort();
          continue;
        }
      }

      if (exitLongAlerts.count() && tradeLong->isChecked())
      {
        if ((int) exitLongAlerts.count() == exitLongCount)
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
        if ((int) enterLongAlerts.count() == enterLongCount)
        {
          enterLong();
          continue;
        }
      }

      if (exitShortAlerts.count() && tradeShort->isChecked())
      {
        if ((int) exitShortAlerts.count() == exitShortCount)
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

  closePlot->clear();
  equityPlot->clear();

  closePlot->setData(db->getRecordList());
  equityPlot->setData(db->getRecordList());

  PlotLine *line = new PlotLine;
  line->setLabel("Equity");
  line->setColor("green");
  Indicator *ind = new Indicator;
  ind->set("Plot", "True", Setting::None);
  int loop;
  int loop2 = 0;
  double t = account->value();
  for (loop = 0; loop < db->getDataSize(); loop++)
  {
    if (loop2 < tradeList->numRows())
    {
      Setting *r = db->getRecordIndex(loop);

      QString s = r->getData("Date");
      s.truncate(s.length() - 6);
      if (! s.compare(tradeList->text(loop2, 3)))
      {
        t = tradeList->text(loop2, 7).toDouble();
	loop2++;
      }
    }

    line->append(t);
  }
  ind->addLine(line);
  equityPlot->addIndicator("Equity", ind);
  equityPlot->setCurrentIndicator("Equity");

  createSummary();

  int page = closePlot->getWidth() / closePlot->getPixelspace();
  int max = db->getDataSize() - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, db->getDataSize() - 1);
  slider->setValue(max);
  slider->blockSignals(FALSE);

  closePlot->draw();
  equityPlot->draw();

  delete db;
}

void Tester::checkAlerts ()
{
  QString key = currentRecord->getData("Date").left(8);

  QDictIterator<Setting> it(enterLongAlerts);
  for (; it.current(); ++it)
  {
    Setting *set = it.current();
    QString s = set->getData(key);
    if (s.length())
      enterLongCount++;
  }

  QDictIterator<Setting> it2(exitLongAlerts);
  for (; it2.current(); ++it2)
  {
    Setting *set = it2.current();
    QString s = set->getData(key);
    if (s.length())
      exitLongCount++;
  }

  QDictIterator<Setting> it3(enterShortAlerts);
  for (; it3.current(); ++it3)
  {
    Setting *set = it3.current();
    QString s = set->getData(key);
    if (s.length())
      enterShortCount++;
  }

  QDictIterator<Setting> it4(exitShortAlerts);
  for (; it4.current(); ++it4)
  {
    Setting *set = it4.current();
    QString s = set->getData(key);
    if (s.length())
      exitShortCount++;
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
  trailingHigh = buyRecord->getFloat("Close");
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
  trailingLow = buyRecord->getFloat("Close");
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
  double enterPrice = buyRecord->getFloat("Close");
  double exitPrice = currentRecord->getFloat("Close");
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

  Setting *details = db->getDetails();
  if (! details->getData("Chart Type").compare(tr("Futures")))
  {
    FuturesData *fd = new FuturesData;
    fd->setSymbol(details->getData("Futures Type"));
    profit = fd->getRate() * profit;
    delete fd;
  }

  equity = equity + profit;

  equity = equity - exitCom->value();

  tradeList->setNumRows(tradeList->numRows() + 1);
  tradeList->setText(tradeList->numRows() - 1, 0, type);
  tradeList->setText(tradeList->numRows() - 1, 1, buyRecord->getData("Date").left(8));
  tradeList->setText(tradeList->numRows() - 1, 2, buyRecord->getData("Close"));
  tradeList->setText(tradeList->numRows() - 1, 3, currentRecord->getData("Date").left(8));
  tradeList->setText(tradeList->numRows() - 1, 4, currentRecord->getData("Close"));
  tradeList->setText(tradeList->numRows() - 1, 5, signal);
  tradeList->setText(tradeList->numRows() - 1, 6, QString::number(profit));
  tradeList->setText(tradeList->numRows() - 1, 7, QString::number(equity));
  tradeList->setText(tradeList->numRows() - 1, 8, QString::number(volume));
}

bool Tester::maximumLoss ()
{
  if (! maximumLossCheck->isChecked())
    return FALSE;

  if ((status == 1) && (maximumLossLong->isChecked()))
  {
    double t = ((currentRecord->getFloat("Close") - buyRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
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
    double t = ((buyRecord->getFloat("Close") - currentRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
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

  if ((status == 1) && (profitLong->isChecked()))
  {
    double t = ((currentRecord->getFloat("Close") - buyRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
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
    double t = ((buyRecord->getFloat("Close") - currentRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
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

  if ((status == 1) && (trailingLong->isChecked()))
  {
    if (currentRecord->getFloat("Close") > trailingHigh)
      trailingHigh = currentRecord->getFloat("Close");

    double t = ((currentRecord->getFloat("Close") - trailingHigh) / trailingHigh) * 100;
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
    if (currentRecord->getFloat("Close") < trailingLow)
      trailingLow = currentRecord->getFloat("Close");

    double t = ((trailingLow - currentRecord->getFloat("Close")) / trailingLow) * 100;
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
  SymbolDialog *dialog = new SymbolDialog(config->getData(Config::DataPath));
  dialog->setCaption(tr("Select Chart"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString symbol = dialog->getSymbol();
    if (! symbol.length())
    {
      delete dialog;
      return;
    }

    symbolButton->setText(symbol);

    QString s = config->getData(Config::DataPath);
    s.append("/");
    s.append(symbol);
    db = new ChartDb;
    if (db->openChart(s))
    {
      delete db;
      delete dialog;
      qDebug("Tester: Cant open db");
      return;
    }
    
    Setting *details = db->getDetails();

    QDateTime dt = QDateTime::fromString(details->getDateTime("First Date"), ISODate);
    startDate->setDate(dt.date());

    dt = QDateTime::fromString(details->getDateTime("Last Date"), ISODate);
    endDate->setDate(dt.date());

    delete db;
  }

  delete dialog;
}

void Tester::showRule (int button)
{
  indicatorList->clear();

  QDict<Indicator> list;
  QString s;
  switch (button)
  {
    case 0:
      list = enterLongIndicators;
      indicatorList->setColumnText(0, tr("Enter Long Indicators"));
      break;
    case 1:
      list = exitLongIndicators;
      indicatorList->setColumnText(0, tr("Exit Long Indicators"));
      break;
    case 2:
      list = enterShortIndicators;
      indicatorList->setColumnText(0, tr("Enter Short Indicators"));
      break;
    default:
      list = exitShortIndicators;
      indicatorList->setColumnText(0, tr("Exit Short Indicators"));
      break;
  }

  QDictIterator<Indicator> it(list);
  for (; it.current(); ++it)
    item = new QListViewItem(indicatorList, it.currentKey());

  indicatorSelected(0);
}

void Tester::loadIndicators (int button, ChartDb *db)
{
  QDict<Indicator> list;
  switch (button)
  {
    case 0:
      list = enterLongIndicators;
      break;
    case 1:
      list = exitLongIndicators;
      break;
    case 2:
      list = enterShortIndicators;
      break;
    default:
      list = exitShortIndicators;
      break;
  }

  QDictIterator<Indicator> it(list);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();

    i->clearLines();

    QString s = config->getData(Config::IndicatorPluginPath);
    s.append("/");
    s.append(i->getData(QObject::tr("Type")));
    s.append(".so");

    QLibrary *lib = new QLibrary(s);
    Plugin *(*so)() = 0;
    so = (Plugin *(*)()) lib->resolve("create");
    if (so)
    {
      Plugin *plug = (*so)();

      plug->setIndicatorInput(db->getRecordList());

      plug->parse(i->getString());

      plug->calculate();

      s = i->getData(QObject::tr("Alert"));
      if (! s.compare(QObject::tr("True")))
        i->setAlerts(plug->getAlerts());

      i->clearLines();

      delete plug;
    }

    delete lib;
  }
}

void Tester::loadEnterLongAlerts ()
{
  QDictIterator<Indicator> it(enterLongIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();

    int loop;
    bool flag = FALSE;
    Setting *set = enterLongAlerts[it.currentKey()];
    set->clear();
    for (loop = 0; loop < db->getDataSize(); loop++)
    {
      if (i->getAlert(loop) == 1)
      {
        if (! flag)
        {
          Setting *r = db->getRecordIndex(loop);
          set->set(r->getData("Date").left(8), "1", Setting::None);
	  flag = TRUE;
        }
      }
      else
        flag = FALSE;
    }
  }
}

void Tester::loadExitLongAlerts ()
{
  QDictIterator<Indicator> it(exitLongIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();

    int loop;
    bool flag = FALSE;
    Setting *set = exitLongAlerts[it.currentKey()];
    set->clear();
    for (loop = 0; loop < db->getDataSize(); loop++)
    {
      if (i->getAlert(loop) == -1)
      {
        if (! flag)
        {
          Setting *r = db->getRecordIndex(loop);
          set->set(r->getData("Date").left(8), "1", Setting::None);
	  flag = TRUE;
        }
      }
      else
        flag = FALSE;
    }
  }
}

void Tester::loadEnterShortAlerts ()
{
  QDictIterator<Indicator> it(enterShortIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();

    int loop;
    bool flag = FALSE;
    Setting *set = enterShortAlerts[it.currentKey()];
    set->clear();
    for (loop = 0; loop < db->getDataSize(); loop++)
    {
      if (i->getAlert(loop) == -1)
      {
        if (! flag)
        {
          Setting *r = db->getRecordIndex(loop);
          set->set(r->getData("Date").left(8), "1", Setting::None);
	  flag = TRUE;
        }
      }
      else
        flag = FALSE;
    }
  }
}

void Tester::loadExitShortAlerts ()
{
  QDictIterator<Indicator> it(exitShortIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();

    int loop;
    bool flag = FALSE;
    Setting *set = exitShortAlerts[it.currentKey()];
    set->clear();
    for (loop = 0; loop < db->getDataSize(); loop++)
    {
      if (i->getAlert(loop) == 1)
      {
        if (! flag)
        {
          Setting *r = db->getRecordIndex(loop);
          set->set(r->getData("Date").left(8), "1", Setting::None);
	  flag = TRUE;
        }
      }
      else
        flag = FALSE;
    }
  }
}

void Tester::saveRule ()
{
  QStringList l;

  QDictIterator<Indicator> it(enterLongIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();
    QString s = "Enter Long=";
    s.append(i->getString());
    l.append(s);
  }

  QDictIterator<Indicator> it2(exitLongIndicators);
  for (; it2.current(); ++it2)
  {
    Indicator *i = it2.current();
    QString s = "Exit Long=";
    s.append(i->getString());
    l.append(s);
  }

  QDictIterator<Indicator> it3(enterShortIndicators);
  for (; it3.current(); ++it3)
  {
    Indicator *i = it3.current();
    QString s = "Enter Short=";
    s.append(i->getString());
    l.append(s);
  }

  QDictIterator<Indicator> it4(exitShortIndicators);
  for (; it4.current(); ++it4)
  {
    Indicator *i = it4.current();
    QString s = "Exit Short=";
    s.append(i->getString());
    l.append(s);
  }

  // save max loss stop
  if (maximumLossCheck->isChecked())
    l.append("Maximum Loss Check=True");
  else
    l.append("Maximum Loss Check=False");

  if (maximumLossLong->isChecked())
    l.append("Maximum Loss Long=True");
  else
    l.append("Maximum Loss Long=False");

  if (maximumLossShort->isChecked())
    l.append("Maximum Loss Short=True");
  else
    l.append("Maximum Loss Short=False");

  QString s = "Maximum Loss Edit=";
  s.append(maximumLossEdit->text());
  l.append(s);

  // save profit stop
  if (profitCheck->isChecked())
    l.append("Profit Check=True");
  else
    l.append("Profit Check=False");

  if (profitLong->isChecked())
    l.append("Profit Long=True");
  else
    l.append("Profit Long=False");

  if (profitShort->isChecked())
    l.append("Profit Short=True");
  else
    l.append("Profit Short=False");

  s = "Profit Edit=";
  s.append(profitEdit->text());
  l.append(s);

  // save trailing stop
  if (trailingCheck->isChecked())
    l.append("Trailing Check=True");
  else
    l.append("Trailing Check=False");

  if (trailingLong->isChecked())
    l.append("Trailing Long=True");
  else
    l.append("Trailing Long=False");

  if (trailingShort->isChecked())
    l.append("Trailing Short=True");
  else
    l.append("Trailing Short=False");

  s = "Trailing Edit=";
  s.append(trailingEdit->text());
  l.append(s);

  config->setTest(ruleName, l);
}

void Tester::loadRule ()
{
  QStringList l = config->getTest(ruleName);
  if (! l.count())
    return;
    
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QStringList l2 = QStringList::split("=", l[loop], FALSE);
    
    if (! l2[0].compare("Enter Long"))
    {
      Indicator *i = new Indicator;
      i->parse(l2[1]);
      enterLongIndicators.insert(i->getData("Name"), i);
      enterLongAlerts.insert(i->getData("Name"), new Setting);
      continue;
    }

    if (! l2[0].compare("Exit Long"))
    {
      Indicator *i = new Indicator;
      i->parse(l2[1]);
      exitLongIndicators.insert(i->getData("Name"), i);
      exitLongAlerts.insert(i->getData("Name"), new Setting);
      continue;
    }

    if (! l2[0].compare("Enter Short"))
    {
      Indicator *i = new Indicator;
      i->parse(l2[1]);
      enterShortIndicators.insert(i->getData("Name"), i);
      enterShortAlerts.insert(i->getData("Name"), new Setting);
      continue;
    }

    if (! l2[0].compare("Exit Short"))
    {
      Indicator *i = new Indicator;
      i->parse(l2[1]);
      exitShortIndicators.insert(i->getData("Name"), i);
      exitShortAlerts.insert(i->getData("Name"), new Setting);
      continue;
    }

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
      trailingEdit->setText(l2[1]);
  }
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
  
  Indicator *i = closePlot->getIndicator("Main Plot");

  int count = 0;
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
      
      Setting *co = closePlot->newChartObject(tr("Buy Arrow"));
      count++;
      co->set("Name", QString::number(count), Setting::None);
      co->setData(tr("Date"), tradeList->text(loop, 1));
      co->setData(tr("Value"), QString::number(tradeList->text(loop, 2).toDouble() * .98));
      i->addChartObject(co);

      co = closePlot->newChartObject(tr("Sell Arrow"));
      count++;
      co->set("Name", QString::number(count), Setting::None);
      co->setData(tr("Date"), tradeList->text(loop, 3));
      co->setData(tr("Value"), QString::number(tradeList->text(loop, 4).toDouble() * 1.02));
      i->addChartObject(co);
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

      Setting *co = closePlot->newChartObject(tr("Sell Arrow"));
      count++;
      co->set("Name", QString::number(count), Setting::None);
      co->setData(tr("Date"), tradeList->text(loop, 1));
      co->setData(tr("Value"), QString::number(tradeList->text(loop, 2).toDouble() * 1.02));
      i->addChartObject(co);
      
      co = closePlot->newChartObject(tr("Buy Arrow"));
      count++;
      co->set("Name", QString::number(count), Setting::None);
      co->setData(tr("Date"), tradeList->text(loop, 3));
      co->setData(tr("Value"), QString::number(tradeList->text(loop, 4).toDouble() * .98));
      i->addChartObject(co);
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

void Tester::sliderChanged (int v)
{
  equityPlot->setIndex(v);
  closePlot->setIndex(v);

  equityPlot->draw();
  closePlot->draw();
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
    volume = (int) (balance / buyRecord->getFloat("Close"));

  if (volume < 1)
    volume = 1;
}


