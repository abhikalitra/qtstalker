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
#include <qlabel.h>
#include "Tester.h"
#include "edit.xpm"
#include "delete.xpm"
#include "indicator.xpm"
#include "stop.xpm"
#include "ok.xpm"
#include "EditDialog.h"
#include "Plugin.h"
#include "SettingView.h"

Tester::Tester (Config *c, QString n) : QDialog (0, 0, FALSE)
{
  config = c;
  ruleName = n;
  enterLongIndicators.setAutoDelete(TRUE);
  exitLongIndicators.setAutoDelete(TRUE);
  enterShortIndicators.setAutoDelete(TRUE);
  exitShortIndicators.setAutoDelete(TRUE);

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
  button->setText(tr("EL"));
  button->setToggleButton(TRUE);
  tb->addWidget(button, 0, 0);
  buttonGroup->insert(button, 0);

  button = new QToolButton(w);
  QToolTip::add(button, tr("Exit Long Rule"));
  button->setAutoRaise(TRUE);
  button->setMaximumWidth(30);
  button->setText(tr("XL"));
  button->setToggleButton(TRUE);
  tb->addWidget(button, 0, 1);
  buttonGroup->insert(button, 1);

  button = new QToolButton(w);
  QToolTip::add(button, tr("Enter Short Rule"));
  button->setAutoRaise(TRUE);
  button->setMaximumWidth(30);
  button->setText(tr("ES"));
  button->setToggleButton(TRUE);
  tb->addWidget(button, 0, 2);
  buttonGroup->insert(button, 2);

  button = new QToolButton(w);
  QToolTip::add(button, tr("Exit Short Rule"));
  button->setAutoRaise(TRUE);
  button->setMaximumWidth(30);
  button->setText(tr("XS"));
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

  QVGroupBox *gbox = new QVGroupBox(tr("Breakeven"), w);
  gbox->setInsideSpacing(2);
  grid->addWidget(gbox, 0, 0);

  breakevenCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(breakevenCheck, SIGNAL(toggled(bool)), this, SLOT(breakevenToggled(bool)));

  breakevenLong = new QCheckBox(tr("Long"), gbox);

  breakevenShort = new QCheckBox(tr("Short"), gbox);

  gbox = new QVGroupBox(tr("Maximum Loss"), w);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  grid->addWidget(gbox, 0, 1);

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
  grid->addWidget(gbox, 1, 0);

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
  grid->addWidget(gbox, 1, 1);

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

  breakevenToggled(FALSE);
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
  
  QGridLayout *grid = new QGridLayout(vbox, 2, 2);
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

  delayDays = new QSpinBox(0, 999999, 1, gbox);

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

  label = new QLabel(tr("Entry Commision"), gbox);

  entryCom = new QSpinBox(0, 999999, 1, gbox);
  entryCom->setValue(25);

  label = new QLabel(tr("Exit Commision"), gbox);

  exitCom = new QSpinBox(0, 999999, 1, gbox);
  exitCom->setValue(25);

  label = new QLabel(tr("Account Balance"), gbox);

  account = new QSpinBox(0, 999999, 1, gbox);
  account->setValue(10000);

  gbox = new QVGroupBox(tr("Symbol"), w);
  gbox->setInsideSpacing(2);
  grid->addWidget(gbox, 1, 1);

  symbolButton = new QPushButton(gbox);
  connect(symbolButton, SIGNAL(clicked()), this, SLOT(symbolButtonPressed()));

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

  tradeList = new QListView(w);
  tradeList->addColumn(tr("Type"), -1);
  tradeList->addColumn(tr("Entry"), -1);
  tradeList->addColumn(tr("Entry price"), -1);
  tradeList->addColumn(tr("Exit"), -1);
  tradeList->addColumn(tr("Exit price"), -1);
  tradeList->addColumn(tr("Signal"), -1);
  tradeList->addColumn(tr("Profit"), -1);
  tradeList->setSelectionMode(QListView::Single);
  vbox->addWidget(tradeList);

  tabs->addTab(w, tr("Reports"));
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
	break;
      case 1:
        exitLongIndicators.insert(name, i);
	break;
      case 2:
        enterShortIndicators.insert(name, i);
	break;
      default:
        exitShortIndicators.insert(name, i);
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
      break;
    case 1:
      exitLongIndicators.remove(item->text(0));
      break;
    case 2:
      enterShortIndicators.remove(item->text(0));
      break;
    default:
      exitShortIndicators.remove(item->text(0));
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

  double equity = account->text().toDouble();
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

  tradeList->clear();

  db->getHistory(ChartDb::Daily, sd);
  
  loadIndicators(0, db);
  loadIndicators(1, db);
  loadIndicators(2, db);
  loadIndicators(3, db);

  status = 0;
  for (testLoop = 0; testLoop < db->getDataSize(); testLoop++)
  {
    currentRecord = db->getRecordIndex(testLoop);

    QDateTime dt = QDateTime::fromString(currentRecord->getDateTime("Date"), ISODate);
    if (dt > ed)
      break;

    if (checkEnterLong())
      continue;

    if (checkExitLong())
      continue;

    if (checkEnterShort())
      continue;

    if (checkExitShort())
      continue;

    if (breakeven())
      continue;

    if (maximumLoss())
      continue;

    if (profit())
      continue;

    trailing();
  }

  delete db;
}

bool Tester::checkEnterLong ()
{
  if (status == 1)
    return FALSE;

  if (! tradeLong->isChecked())
    return FALSE;

  if (! enterLongIndicators.count())
    return FALSE;

  int count = 0;
  QDictIterator<Indicator> it(enterLongIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();
    if (i->getAlert(testLoop) == 1)
      count++;
  }

  if ((int) enterLongIndicators.count() != count)
    return FALSE;

  if (status != 0)
    exitPosition("Enter Long");

  status = 1;
  
  buyRecord = currentRecord;
  
  trailingHigh = buyRecord->getFloat("Close");

  return TRUE;
}

bool Tester::checkExitLong ()
{
  if (status != 1)
    return FALSE;

  if (! tradeLong->isChecked())
    return FALSE;

  if (! exitLongIndicators.count())
    return FALSE;

  int count = 0;
  QDictIterator<Indicator> it(exitLongIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();
    if (i->getAlert(testLoop) == -1)
      count++;
  }

  if ((int) exitLongIndicators.count() != count)
    return FALSE;

  exitPosition("Exit Long");

  status = 0;

  return TRUE;
}

bool Tester::checkEnterShort ()
{
  if (status == -1)
    return FALSE;

  if (! tradeShort->isChecked())
    return FALSE;

  if (! enterShortIndicators.count())
    return FALSE;

  int count = 0;
  QDictIterator<Indicator> it(enterShortIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();
    if (i->getAlert(testLoop) == -1)
      count++;
  }

  if ((int) enterShortIndicators.count() != count)
    return FALSE;

  if (status != 0)
    exitPosition("Enter Short");

  status = -1;

  buyRecord = currentRecord;

  trailingLow = buyRecord->getFloat("Close");

  return TRUE;
}

bool Tester::checkExitShort ()
{
  if (status != -1)
    return FALSE;

  if (! tradeShort->isChecked())
    return FALSE;

  if (! exitShortIndicators.count())
    return FALSE;

  int count = 0;
  QDictIterator<Indicator> it(exitShortIndicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();
    if (i->getAlert(testLoop) == 1)
      count++;
  }

  if ((int) exitShortIndicators.count() != count)
    return FALSE;

  exitPosition("Exit Short");

  status = 0;
  
  return TRUE;
}

void Tester::exitPosition (QString signal)
{
  double enterPrice = buyRecord->getFloat("Close");
  double exitPrice = currentRecord->getFloat("Close");
  double profit = 0;
  QString type;

  if (status == -1)
  {
    profit = enterPrice - exitPrice;
    type = tr("Short");
  }
  else
  {
    profit = exitPrice - enterPrice;
    type = tr("Long");
  }

  item = new QListViewItem(tradeList,
  			   type,
			   buyRecord->getData("Date").left(8),
			   buyRecord->getData("Close"),
			   currentRecord->getData("Date").left(8),
			   currentRecord->getData("Close"),
			   signal,
			   QString::number(profit));

}

bool Tester::breakeven ()
{
  if (status == 0)
    return FALSE;

  if (! breakevenCheck->isChecked())
    return FALSE;

  if ((status == 1) && (breakevenLong->isChecked()))
  {
    double t = currentRecord->getFloat("Close") - buyRecord->getFloat("Close");
    if (t <= 0)
    {
      exitPosition("Breakeven");
      status = 0;
      return TRUE;
    }
  }

  if ((status == -1) && (breakevenShort->isChecked()))
  {
    double t = buyRecord->getFloat("Close") - currentRecord->getFloat("Close");
    if (t <= 0)
    {
      exitPosition("Breakeven");
      status = 0;
      return TRUE;
    }
  }

  return FALSE;
}

bool Tester::maximumLoss ()
{
  if (status == 0)
    return FALSE;

  if (! maximumLossCheck->isChecked())
    return FALSE;

  double loss = maximumLossEdit->text().toDouble();
  double t = 0;

  if ((status == 1) && (maximumLossLong->isChecked()))
  {
    t = ((currentRecord->getFloat("Close") - buyRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
    if (t < 0)
    {
      t = +t;
      if (t >= loss)
        return TRUE;
    }
  }

  if ((status == -1) && (maximumLossShort->isChecked()))
  {
    t = ((buyRecord->getFloat("Close") - currentRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
    if (t < 0)
    {
      t = +t;
      if (t >= loss)
        return TRUE;
    }
  }

  return FALSE;
}

bool Tester::profit ()
{
  if (status == 0)
    return FALSE;

  if (! profitCheck->isChecked())
    return FALSE;

  double profit = profitEdit->text().toDouble();
  double t = 0;

  if ((status == 1) && (profitLong->isChecked()))
  {
    t = ((currentRecord->getFloat("Close") - buyRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
    if (t > 0)
    {
      if (t >= profit)
        return TRUE;
    }
  }

  if ((status == -1) && (profitShort->isChecked()))
  {
    t = ((buyRecord->getFloat("Close") - currentRecord->getFloat("Close")) / buyRecord->getFloat("Close")) * 100;
    if (t > 0)
    {
      if (t >= profit)
        return TRUE;
    }
  }

  return FALSE;
}

bool Tester::trailing ()
{
  if (status == 0)
    return FALSE;

  if (! trailingCheck->isChecked())
    return FALSE;
    
  double loss = trailingEdit->text().toDouble();
  double t = 0;

  if ((status == 1) && (trailingLong->isChecked()))
  {
    if (currentRecord->getFloat("Close") > trailingHigh)
      trailingHigh = currentRecord->getFloat("Close");

    t = ((currentRecord->getFloat("Close") - trailingHigh) / trailingHigh) * 100;
    if (t < 0)
    {
      t = +t;
      if (t >= loss)
        return TRUE;
    }
  }

  if ((status == -1) && (trailingShort->isChecked()))
  {
    if (currentRecord->getFloat("Close") < trailingLow)
      trailingLow = currentRecord->getFloat("Close");

    t = ((trailingLow - currentRecord->getFloat("Close")) / trailingLow) * 100;
    if (t < 0)
    {
      t = +t;
      if (t >= loss)
        return TRUE;
    }
  }

  return FALSE;
}

void Tester::breakevenToggled (bool status)
{
  if (status)
  {
    breakevenLong->setEnabled(TRUE);
    breakevenShort->setEnabled(TRUE);
  }
  else
  {
    breakevenLong->setEnabled(FALSE);
    breakevenShort->setEnabled(FALSE);
  }
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
    symbolButton->setText(dialog->getSymbol());

    QString s = config->getData(Config::DataPath);
    s.append("/");
    s.append(dialog->getSymbol());
    ChartDb *db = new ChartDb;
    if (db->openChart(s))
    {
      delete db;
      qDebug("Tester: Cant open db");
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

  // save breakeven stop
  if (breakevenCheck->isChecked())
    l.append("Breakeven Check=True");
  else
    l.append("Breakeven Check=False");

  if (breakevenLong->isChecked())
    l.append("Breakeven Long=True");
  else
    l.append("Breakeven Long=False");

  if (breakevenShort->isChecked())
    l.append("Breakeven Short=True");
  else
    l.append("Breakeven Short=False");


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
      continue;
    }

    if (! l2[0].compare("Exit Long"))
    {
      Indicator *i = new Indicator;
      i->parse(l2[1]);
      exitLongIndicators.insert(i->getData("Name"), i);
      continue;
    }

    if (! l2[0].compare("Enter Short"))
    {
      Indicator *i = new Indicator;
      i->parse(l2[1]);
      enterShortIndicators.insert(i->getData("Name"), i);
      continue;
    }

    if (! l2[0].compare("Exit Short"))
    {
      Indicator *i = new Indicator;
      i->parse(l2[1]);
      exitShortIndicators.insert(i->getData("Name"), i);
      continue;
    }

    if (! l2[0].compare("Breakeven Check"))
    {
      if (! l2[1].compare("True"))
        breakevenCheck->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Breakeven Long"))
    {
      if (! l2[1].compare("True"))
        breakevenLong->setChecked(TRUE);
      continue;
    }

    if (! l2[0].compare("Breakeven Short"))
    {
      if (! l2[1].compare("True"))
        breakevenShort->setChecked(TRUE);
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

