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
#include "FuturesData.h"
#include "EditDialog.h"
#include "ok.xpm"
#include "newchart.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "stop.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtooltip.h>
#include <qdir.h>

PortfolioDialog::PortfolioDialog (Config *c, QString p) : QDialog (0, "PortfolioDialog", TRUE)
{
  config = c;
  portfolio = p;

  QString s = tr("Qtstalker: Portfolio");
  s.append(" ");
  s.append(portfolio);
  setCaption(s);
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  toolbar = new QGridLayout(vbox, 1, 6);
  toolbar->setSpacing(1);
  
  okButton = new QToolButton(this);
  QToolTip::add(okButton, tr("OK"));
  okButton->setPixmap(QPixmap(ok));
  connect(okButton, SIGNAL(clicked()), this, SLOT(savePortfolio()));
  okButton->setMaximumWidth(30);
  okButton->setAutoRaise(TRUE);
  toolbar->addWidget(okButton, 0, 0);

  cancelButton = new QToolButton(this);
  QToolTip::add(cancelButton, tr("Cancel"));
  cancelButton->setPixmap(QPixmap(stop));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(savePortfolio()));
  cancelButton->setMaximumWidth(30);
  cancelButton->setAutoRaise(TRUE);
  toolbar->addWidget(cancelButton, 0, 1);

  addButton = new QToolButton(this);
  QToolTip::add(addButton, tr("Add"));
  addButton->setPixmap(QPixmap(newchart));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addItem()));
  addButton->setMaximumWidth(30);
  addButton->setAutoRaise(TRUE);
  toolbar->addWidget(addButton, 0, 2);

  modifyButton = new QToolButton(this);
  QToolTip::add(modifyButton, tr("Edit"));
  modifyButton->setPixmap(QPixmap(edit));
  connect(modifyButton, SIGNAL(clicked()), this, SLOT(modifyItem()));
  modifyButton->setMaximumWidth(30);
  modifyButton->setAutoRaise(TRUE);
  toolbar->addWidget(modifyButton, 0, 3);
  modifyButton->setEnabled(FALSE);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 4);
  deleteButton->setEnabled(FALSE);

  list = new QListView(this);
  list->setSelectionMode(QListView::Single);
  list->addColumn(QObject::tr("Ticker"), -1);
  list->addColumn(QObject::tr("L/S"), -1);
  list->addColumn(QObject::tr("Vol"), -1);
  list->addColumn(QObject::tr("Buy"), -1);
  list->addColumn(QObject::tr("Last Date"), -1);
  list->addColumn(QObject::tr("Value"), -1);
  list->addColumn(QObject::tr("Profit"), -1);
  connect(list, SIGNAL(clicked(QListViewItem *)), this, SLOT(buttonStatus(QListViewItem *)));
  vbox->addWidget(list);

  buttonStatus(0);

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
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QStringList l2 = QStringList::split(",", l[loop], FALSE);
    item = new QListViewItem(list, l2[0], l2[1], l2[2], l2[3]);
  }

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
    if (db->openChart(s))
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
  set->setList(tr("Action"), l);
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

  buttonStatus(0);
}

void PortfolioDialog::modifyItem ()
{
  item = list->currentItem();
  if (! item)
    return;

  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("Edit Portfolio Item"));

  Setting *set = new Setting();
  set->set(tr("Symbol"), item->text(0), Setting::Symbol);
  QStringList l;
  l.append(tr("Long"));
  l.append(tr("Short"));
  set->set(tr("Action"), item->text(1), Setting::List);
  set->setList(tr("Action"), l);
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
  }

  delete set;
  delete dialog;
}

void PortfolioDialog::buttonStatus (QListViewItem *i)
{
  if (! i)
  {
    deleteButton->setEnabled(FALSE);
    modifyButton->setEnabled(FALSE);
  }
  else
  {
    deleteButton->setEnabled(TRUE);
    modifyButton->setEnabled(TRUE);
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

