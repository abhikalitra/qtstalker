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
#include "Tester.h"
#include "edit.xpm"
#include "delete.xpm"
#include "indicator.xpm"
#include "stop.xpm"
#include "ok.xpm"
#include "EditDialog.h"
#include "ChartDb.h"
#include "Plugin.h"

Tester::Tester (Config *c) : QDialog (0, 0, FALSE)
{
  config = c;
  indicators.setAutoDelete(TRUE);
  stops.setAutoDelete(TRUE);

  setCaption ("Back Tester");

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QGridLayout *tb = new QGridLayout(vbox, 1, 3);
  tb->setSpacing(1);

  QToolButton *button = new QToolButton(this);
  QToolTip::add(button, tr("Start Testing"));
  button->setPixmap(QPixmap(ok));
  connect(button, SIGNAL(clicked()), this, SLOT(test()));
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
}

Tester::~Tester ()
{
  delete testSettings;
}

void Tester::createFormulaPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QGridLayout *tb = new QGridLayout(vbox, 1, 4);
  tb->setSpacing(1);

  addIndicatorButton = new QToolButton(w);
  QToolTip::add(addIndicatorButton, tr("Add Indicator"));
  addIndicatorButton->setPixmap(QPixmap(indicator));
  connect(addIndicatorButton, SIGNAL(clicked()), this, SLOT(addIndicator()));
  addIndicatorButton->setMaximumWidth(30);
  addIndicatorButton->setAutoRaise(TRUE);
  tb->addWidget(addIndicatorButton, 0, 0);
  addIndicatorButton->setEnabled(FALSE);

  editIndicatorButton = new QToolButton(w);
  QToolTip::add(editIndicatorButton, tr("Edit Indicator"));
  editIndicatorButton->setPixmap(QPixmap(edit));
  connect(editIndicatorButton, SIGNAL(clicked()), this, SLOT(editIndicator()));
  editIndicatorButton->setMaximumWidth(30);
  editIndicatorButton->setAutoRaise(TRUE);
  tb->addWidget(editIndicatorButton, 0, 1);
  editIndicatorButton->setEnabled(FALSE);

  deleteIndicatorButton = new QToolButton(w);
  QToolTip::add(deleteIndicatorButton, tr("Delete Indicator"));
  deleteIndicatorButton->setPixmap(QPixmap(deletefile));
  connect(deleteIndicatorButton, SIGNAL(clicked()), this, SLOT(deleteIndicator()));
  deleteIndicatorButton->setMaximumWidth(30);
  deleteIndicatorButton->setAutoRaise(TRUE);
  tb->addWidget(deleteIndicatorButton, 0, 2);
  deleteIndicatorButton->setEnabled(FALSE);

  ruleList = new QListView(w);
  ruleList->addColumn(tr("Indicators"), -1);
  ruleList->setSelectionMode(QListView::Single);
  ruleList->setRootIsDecorated(TRUE);
  ruleList->setSorting(-1, FALSE);
  connect(ruleList, SIGNAL(clicked(QListViewItem *)), this, SLOT(indicatorSelected(QListViewItem *)));
  vbox->addWidget(ruleList);

  baseItemList.setAutoDelete(FALSE);
  item = new QListViewItem(ruleList, tr("Enter Long"));
  baseItemList.append(item);
  item = new QListViewItem(ruleList, item, tr("Exit Long"));
  baseItemList.append(item);
  item = new QListViewItem(ruleList, item, tr("Enter Short"));
  baseItemList.append(item);
  item = new QListViewItem(ruleList, item, tr("Exit Short"));
  baseItemList.append(item);

  tabs->addTab(w, tr("Rules"));
}

void Tester::createStopPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  stopList = new QListView(w);
  stopList->addColumn(tr("Stops"), -1);
  stopList->setSelectionMode(QListView::Single);
  connect(stopList, SIGNAL(clicked(QListViewItem *)), this, SLOT(stopSelected(QListViewItem *)));
  vbox->addWidget(stopList);

  Setting *set = new Setting;
  set->set(tr("Long"), tr("False"), Setting::Bool);
  set->set(tr("Short"), tr("False"), Setting::Bool);
  stops.replace(tr("Breakeven"), set);

  set = new Setting;
  set->set(tr("Long"), tr("False"), Setting::Bool);
  set->set(tr("Short"), tr("False"), Setting::Bool);
  set->set(tr("Loss %"), "0", Setting::Float);
  stops.replace(tr("Maximum Loss"), set);

  set = new Setting;
  set->set(tr("Long"), tr("False"), Setting::Bool);
  set->set(tr("Short"), tr("False"), Setting::Bool);
  set->set(tr("Profit %"), "0", Setting::Float);
  stops.replace(tr("Profit"), set);

  set = new Setting;
  set->set(tr("Long"), tr("False"), Setting::Bool);
  set->set(tr("Short"), tr("False"), Setting::Bool);
  set->set(tr("Risk %"), "0", Setting::Float);
  stops.replace(tr("Trailing"), set);

  QDictIterator<Setting> it(stops);
  for (; it.current(); ++it)
    item = new QListViewItem(stopList, it.currentKey());

  tabs->addTab(w, tr("Stops"));
}

void Tester::createTestPage ()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  testSettings = new Setting;
  testSettings->set(tr("Long"), tr("True"), Setting::Bool);
  testSettings->set(tr("Short"), tr("True"), Setting::Bool);
  testSettings->set(tr("Delay"), "0", Setting::Integer);
  testSettings->set(tr("Entry Commission"), "25", Setting::Float);
  testSettings->set(tr("Exit Commission"), "25", Setting::Float);
  testSettings->set(tr("Initial Equity"), "10000", Setting::Float);
  testSettings->set(tr("Symbol"), "", Setting::Symbol);
  QDate dt = QDate::currentDate();
  testSettings->set(tr("Start Date"), dt.toString("yyyyMMdd"), Setting::Date);
  testSettings->set(tr("End Date"), dt.toString("yyyyMMdd"), Setting::Date);

  testList = new SettingView(w, config->getData(Config::DataPath));
  vbox->addWidget(testList);
  testList->setItems(testSettings);

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

  Indicator *i = indicators[name];
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

    item = ruleList->selectedItem();
    item = new QListViewItem(item, name);
    indicators.replace(name, i);
  }

  delete dialog;
  delete set;
}

void Tester::editIndicator ()
{
  item = ruleList->selectedItem();
  if (! item)
    return;

  QString name = item->text(0);

  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("Edit Indicator"));

  Indicator *i = indicators[name];

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
  item = ruleList->selectedItem();
  if (! item)
    return;

  indicators.remove(item->text(0));

  delete item;
}

void Tester::indicatorSelected (QListViewItem *i)
{
  int index = baseItemList.findRef(i);

  if (index == -1)
  {
    editIndicatorButton->setEnabled(TRUE);
    deleteIndicatorButton->setEnabled(TRUE);
    addIndicatorButton->setEnabled(FALSE);
  }
  else
  {
    editIndicatorButton->setEnabled(FALSE);
    deleteIndicatorButton->setEnabled(FALSE);
    addIndicatorButton->setEnabled(TRUE);
  }
}

void Tester::stopSelected (QListViewItem *i)
{
  if (! i)
    return;

  Setting *set = stops[i->text(0)];

  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("Edit Stop"));

  dialog->setItems(set);

  dialog->exec();

  delete dialog;
}

void Tester::test ()
{
  bool longFlag = FALSE;
  if (! testSettings->getData(tr("Long")).compare(tr("True")))
    longFlag = TRUE;

  bool shortFlag = FALSE;
  if (! testSettings->getData(tr("Short")).compare(tr("True")))
    shortFlag = TRUE;

  if (! longFlag && ! shortFlag)
    return;

//  int delay = testSettings->getInt(tr("Delay"));

//  double entryCom = testSettings->getFloat(tr("Entry Commission"));

//  double exitCom = testSettings->getFloat(tr("Exit Commission"));

  double equity = testSettings->getFloat(tr("Initial Equity"));
  if (equity <= 0)
    return;

  QString symbol = testSettings->getData(tr("Symbol"));
  if (! symbol.length())
    return;

  QString s = testSettings->getData(tr("Start Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  QDate startDate = QDate::fromString(s, ISODate);
  if (! startDate.isValid())
    return;

  s = testSettings->getData(tr("End Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  QDate endDate = QDate::fromString(s, ISODate);
  if (! endDate.isValid())
    return;

  s = config->getData(Config::DataPath);
  s.append("/");
  s.append(symbol);
  ChartDb *db = new ChartDb;
  if (db->openChart(s))
  {
    delete db;
    qDebug("Tester: Cant open db");
    return;
  }

  tradeList->clear();

//  Setting *details = db->getDetails();

  db->getHistory(ChartDb::Daily, startDate);

  QDictIterator<Indicator> it(indicators);
  for (; it.current(); ++it)
  {
    Indicator *i = it.current();
    
    s = config->getData(Config::IndicatorPluginPath);
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

      delete plug;
    }

    delete lib;
  }

  int loop;
  int status = 0;
  Setting *sr;
  double trailingHigh = 0;
  double trailingLow = 0;
  for (loop = 0; loop < db->getDataSize(); loop++)
  {
    Setting *r = db->getRecordIndex(loop);

    s = r->getData(tr("Date"));
    s.insert(4, "-");
    s.insert(7, "-");
    s.truncate(s.length() - 6);
    QDate dt = QDate::fromString(s, ISODate);
    if (dt > endDate)
      break;

    if (status == -1)
    {
      if (longFlag)
      {
        if (checkPosition(0, loop, db->getDataSize()))
        {
	  exitPosition(status, sr, r, tr("Enter Long"));
          status = 1;
	  sr = r;
          trailingHigh = r->getFloat(tr("Close"));
          trailingLow = r->getFloat(tr("Close"));
	  continue;
        }
      }

      if (checkPosition(3, loop, db->getDataSize()))
      {
	exitPosition(status, sr, r, tr("Exit Short"));
	status = 0;
	sr = r;
        continue;
      }
    }

    if (status == 1)
    {
      if (shortFlag)
      {
        if (checkPosition(2, loop, db->getDataSize()))
        {
      	  exitPosition(status, sr, r, tr("Enter Short"));
          status = -1;
	  sr = r;
          trailingHigh = r->getFloat(tr("Close"));
          trailingLow = r->getFloat(tr("Close"));
	  continue;
        }
      }

      if (checkPosition(1, loop, db->getDataSize()))
      {
	exitPosition(status, sr, r, tr("Exit Long"));
	status = 0;
	sr = r;
        continue;
      }
    }

    if (status == 0)
    {
      if (longFlag)
      {
        if (checkPosition(0, loop, db->getDataSize()))
        {
          status = 1;
	  sr = r;
          trailingHigh = r->getFloat(tr("Close"));
          trailingLow = r->getFloat(tr("Close"));
	  continue;
	}
      }

      if (shortFlag)
      {
        if (checkPosition(2, loop, db->getDataSize()))
	{
	  status = -1;
	  sr = r;
          trailingHigh = r->getFloat(tr("Close"));
          trailingLow = r->getFloat(tr("Close"));
          continue;
	}
      }
    }

    if (status == 0)
      continue;

    if (r->getFloat(tr("Close")) > trailingHigh)
      trailingHigh = r->getFloat(tr("Close"));

    if (r->getFloat(tr("Close")) < trailingLow)
      trailingLow = r->getFloat(tr("Close"));

    if (breakeven(status, sr, r))
    {
      exitPosition(status, sr, r, tr("Break"));
      status = 0;
      continue;
    }

    if (maximumLoss(status, sr, r))
    {
      exitPosition(status, sr, r, tr("Loss"));
      status = 0;
      continue;
    }

    if (profit(status, sr, r))
    {
      exitPosition(status, sr, r, tr("Profit"));
      status = 0;
      continue;
    }

    double t;
    if (status == 1)
      t = trailingHigh;
    if (status == -1)
      t = trailingLow;
    if (trailing(status, r, t))
    {
      exitPosition(status, sr, r, tr("Trail"));
      status = 0;
    }
  }

  delete db;
}

bool Tester::checkPosition (int pos, int record, int dataSize)
{
  int count = 0;
  int items = 0;
  QListViewItemIterator it(baseItemList.at(pos));
  for (; it.current(); ++it)
  {
    item = it.current();

    Indicator *i = indicators[item->text(0)];
    if (i)
    {
      items++;

      if (i->getLines())
      {
        PlotLine *line = i->getLine(i->getLines() - 1);

        int index = line->getSize() - dataSize + record;
        if (index >= 0)
        {
          if (line->getData(index) == TRUE)
            count++;
        }
      }
    }
  }

  if (items)
  {
    if (items == count)
      return TRUE;
    else
      return FALSE;
  }
  else
    return FALSE;
}

void Tester::exitPosition (int status, Setting *sr, Setting *er, QString signal)
{
  double enterPrice = sr->getFloat("Close");
  double exitPrice = er->getFloat("Close");
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
			   sr->getData("Date").left(8),
			   sr->getData("Close"),
			   er->getData("Date").left(8),
			   er->getData("Close"),
			   signal,
			   QString::number(profit));

}

bool Tester::breakeven (int status, Setting *sr, Setting *er)
{
  bool flag = FALSE;
  Setting *stop = stops[tr("Breakeven")];

  if ((status == 1) && (! stop->getData(tr("Long")).compare(tr("True"))))
  {
    double t = er->getFloat("Close") - sr->getFloat("Close");
    if (t <= 0)
      flag = TRUE;
  }

  if ((status == -1) && (! stop->getData(tr("Short")).compare(tr("True"))))
  {
    double t = sr->getFloat("Close") - er->getFloat("Close");
    if (t <= 0)
      flag = TRUE;
  }

  return flag;
}

bool Tester::maximumLoss (int status, Setting *sr, Setting *er)
{
  bool flag = FALSE;
  bool okFlag = FALSE;
  Setting *stop = stops[tr("Maximum Loss")];
  double loss = stop->getFloat(tr("Loss %"));
  double t = 0;

  if ((status == 1) && (! stop->getData(tr("Long")).compare(tr("True"))))
  {
    t = ((er->getFloat("Close") - sr->getFloat("Close")) / sr->getFloat("Close")) * 100;
    okFlag = TRUE;
  }

  if ((status == -1) && (! stop->getData(tr("Short")).compare(tr("True"))))
  {
    t = ((sr->getFloat("Close") - er->getFloat("Close")) / sr->getFloat("Close")) * 100;
    okFlag = TRUE;
  }

  if (okFlag)
  {
    if (t < 0)
    {
      t = +t;
      if (t >= loss)
        flag = TRUE;
    }
  }

  return flag;
}

bool Tester::profit (int status, Setting *sr, Setting *er)
{
  bool flag = FALSE;
  bool okFlag = FALSE;
  Setting *stop = stops[tr("Profit")];
  double profit = stop->getFloat(tr("Profit %"));
  double t = 0;

  if ((status == 1) && (! stop->getData(tr("Long")).compare(tr("True"))))
  {
    t = ((er->getFloat("Close") - sr->getFloat("Close")) / sr->getFloat("Close")) * 100;
    okFlag = TRUE;
  }

  if ((status == -1) && (! stop->getData(tr("Short")).compare(tr("True"))))
  {
    t = ((sr->getFloat("Close") - er->getFloat("Close")) / sr->getFloat("Close")) * 100;
    okFlag = TRUE;
  }

  if (okFlag)
  {
    if (t > 0)
    {
      if (t >= profit)
        flag = TRUE;
    }
  }

  return flag;
}

bool Tester::trailing (int status, Setting *r, double max)
{
  bool flag = FALSE;
  bool okFlag = FALSE;
  Setting *stop = stops[tr("Trailing")];
  double loss = stop->getFloat(tr("Risk %"));
  double t = 0;

  if ((status == 1) && (! stop->getData(tr("Long")).compare(tr("True"))))
  {
    t = ((r->getFloat("Close") - max) / max) * 100;
    okFlag = TRUE;
  }

  if ((status == -1) && (! stop->getData(tr("Short")).compare(tr("True"))))
  {
    t = ((max - r->getFloat("Close")) / max) * 100;
    okFlag = TRUE;
  }

  if (okFlag)
  {
    if (t < 0)
    {
      t = +t;
      if (t >= loss)
        flag = TRUE;
    }
  }

  return flag;
}



