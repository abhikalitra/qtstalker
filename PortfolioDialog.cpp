/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "PortfolioDialog.h"
#include "ChartDb.h"
#include "Setting.h"
#include "EditDialog.h"
#include "FuturesData.h"
#include <qfiledialog.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <qframe.h>
#include <qvgroupbox.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qdatetime.h>

PortfolioDialog::PortfolioDialog (Config *c, QString p) : QDialog (0, "PortfolioDialog", FALSE, WDestructiveClose)
{
  config = c;

  portfolio = p;

  QString s = tr("Qtstalker: Portfolio");
  s.append(" ");
  s.append(portfolio);
  setCaption(s);

  QHBoxLayout *hbox = new QHBoxLayout (this);
  hbox->setMargin(5);
  hbox->setSpacing(10);

  QVBoxLayout *vbox = new QVBoxLayout (hbox);
  vbox->setSpacing(5);

  QVGroupBox *gbox = new QVGroupBox(tr("Portfolio"), this);
  vbox->addWidget(gbox);

  list = new QListView(gbox);
  list->setSelectionMode(QListView::Single);
  list->setRootIsDecorated(TRUE);
  list->addColumn(QObject::tr("Ticker"), 150);
  list->addColumn(QObject::tr("L/S"));
  list->addColumn(QObject::tr("Vol"));
  list->addColumn(QObject::tr("Buy"), 70);
  list->addColumn(QObject::tr("Last Date"), 80);
  list->addColumn(QObject::tr("Value"), 70);
  list->addColumn(QObject::tr("Profit"), 70);
  connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));

  QGridLayout *grid = new QGridLayout(hbox, 7, 1);
  grid->setSpacing(2);
  grid->setMargin(5);

  addButton = new QPushButton (tr("Add"), this);
  connect (addButton, SIGNAL (clicked()), this, SLOT (addItem()));
  grid->addWidget (addButton, 0, 0);

  modifyButton = new QPushButton (tr("Edit"), this);
  connect (modifyButton, SIGNAL (clicked()), this, SLOT (modifyItem()));
  grid->addWidget(modifyButton, 1, 0);

  deleteButton = new QPushButton (tr("Delete"), this);
  connect (deleteButton, SIGNAL (clicked()), this, SLOT (deleteItem()));
  grid->addWidget (deleteButton, 2, 0);

  QPushButton *button = new QPushButton (tr("Done"), this);
  connect (button, SIGNAL (clicked()), this, SLOT (savePortfolio()));
  grid->addWidget(button, 3, 0);

  buttonStatus();

  updatePortfolio();
}

PortfolioDialog::~PortfolioDialog ()
{
}

void PortfolioDialog::updatePortfolio ()
{
  list->clear();

  QStringList l = config->getPortfolio(portfolio);

  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 4)
    item = new QListViewItem(list, l[loop], l[loop + 1], l[loop + 2], l[loop + 3]);

  updatePortfolioItems();
}

void PortfolioDialog::updatePortfolioItems ()
{
  QListViewItemIterator it(list);
  for (; it.current(); ++it)
  {
    item = it.current();

    QString symbol = item->text(0);
    QString action = item->text(1);
    QString volume = item->text(2);
    QString price = item->text(3);

    QString s = config->getData(Config::DataPath);
    s.append("/");
    s.append(symbol);
    QDir dir(s);
    if (! dir.exists(s, TRUE))
      continue;
      
    ChartDb *db = new ChartDb();
    db->setPath(s);
    if (db->openChart())
    {
      qDebug("Qtstalker: Portfolio: Unable to open chart db.");
      delete db;
      continue;
    }
    
    Setting *details = db->getDetails();

    QDateTime dt = db->getDateTime(details->getData("Last Date"));
    item->setText(4, dt.toString("yyyyMMdd"));

    QString last = QString::number(db->getCloseData(dt));
    item->setText(5, last);

    float total;
    if (! action.compare("Long"))
      total = volume.toFloat() * (last.toFloat() - price.toFloat());
    else
      total = volume.toFloat() * (price.toFloat() - last.toFloat());

    QString type = details->getData("Chart Type");
    if (! type.compare("Futures"))
      total = futuresProfit(details->getData("Futures Type"), total);

    item->setText(6, QString::number(total));

    delete details;
    delete db;
  }
}

void PortfolioDialog::savePortfolio ()
{
  QStringList l;

  QListViewItemIterator it(list);
  for (; it.current(); ++it)
  {
    item = it.current();

    QStringList l2;
    l2.append(item->text(0));
    l2.append(item->text(1));
    l2.append(item->text(2));
    l2.append(item->text(3));
    QString s = l2.join(",");
    l.append(s);
  }

  config->setPortfolio(portfolio, l);

  reject();
}

void PortfolioDialog::addItem ()
{
  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("New Portfolio Item"));

  Setting *set = new Setting();
  set->set(tr("Symbol"), "", Setting::Symbol);
  QStringList l;
  l.append(tr("Long"));
  l.append(tr("Short"));
  set->set(tr("Action"), tr("Long"), Setting::List);
  SettingItem *i = set->getItem(tr("Action"));
  i->list = l;
  set->set(tr("Price"), "0", Setting::Float);
  set->set(tr("Volume"), "1", Setting::Integer);

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString symbol = set->getData(tr("Symbol"));
    if (symbol.isNull())
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("No symbol selected."));
    else
    {
      QString action = set->getData(tr("Action"));
      QString vol = set->getData(tr("Volume"));
      QString price = set->getData(tr("Price"));

      item = new QListViewItem(list, symbol, action, vol, price);
      updatePortfolioItems();
      buttonStatus();
    }
  }

  delete set;
  delete dialog;
}

void PortfolioDialog::deleteItem ()
{
  item = list->selectedItem();
  if (item)
    delete item;

  list->selectAll(FALSE);

  buttonStatus();
}

void PortfolioDialog::modifyItem ()
{
  item = list->currentItem();
  if (item)
  {
    EditDialog *dialog = new EditDialog(config);
    dialog->setCaption(tr("Edit Portfolio Item"));

    Setting *set = new Setting();
    set->set(tr("Symbol"), item->text(0), Setting::Symbol);
    QStringList l;
    l.append(tr("Long"));
    l.append(tr("Short"));
    set->set(tr("Action"), item->text(1), Setting::List);
    SettingItem *i = set->getItem(tr("Action"));
    i->list = l;
    set->set(tr("Price"), item->text(3), Setting::Float);
    set->set(tr("Volume"), item->text(2), Setting::Integer);

    dialog->setItems(set);

    int rc = dialog->exec();

    if (rc == QDialog::Accepted)
    {
      QString symbol = set->getData(tr("Symbol"));
      QString action = set->getData(tr("Action"));
      QString vol = set->getData(tr("Volume"));
      QString price = set->getData(tr("Price"));

      item->setText(0, symbol);
      item->setText(1, action);
      item->setText(2, vol);
      item->setText(3, price);

      updatePortfolioItems();
      buttonStatus();
    }

    delete set;
    delete dialog;
  }
}

void PortfolioDialog::buttonStatus ()
{
  item = list->selectedItem();
  if (item)
  {
    deleteButton->setEnabled(TRUE);
    modifyButton->setEnabled(TRUE);
  }
  else
  {
    deleteButton->setEnabled(FALSE);
    modifyButton->setEnabled(FALSE);
  }
}

float PortfolioDialog::futuresProfit (QString sym, float diff)
{
  FuturesData *fd = new FuturesData();
  fd->setSymbol(sym);
  float rate = fd->getRate();
  float t = diff * rate;
  delete fd;

  return t;
}

