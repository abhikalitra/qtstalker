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
#include "newchart.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

PortfolioDialog::PortfolioDialog (Config *c, QString p) : EditDialog (c)
{
  portfolio = p;
  
  hideSettingView(TRUE);

  QString s = tr("Qtstalker: Portfolio");
  s.append(" ");
  s.append(portfolio);
  setCaption(s);

  unhookButton(0);
  connect(getButton(0), SIGNAL(clicked()), this, SLOT(savePortfolio()));

  setButton(QPixmap(newchart), tr("Add"), 2);
  connect(getButton(2), SIGNAL(clicked()), this, SLOT(addItem()));

  setButton(QPixmap(edit), tr("Edit"), 3);
  connect(getButton(3), SIGNAL(clicked()), this, SLOT(modifyItem()));
  setButtonStatus(3, FALSE);

  setButton(QPixmap(deletefile), tr("Delete"), 4);
  connect(getButton(4), SIGNAL(clicked()), this, SLOT(deleteItem()));
  setButtonStatus(4, FALSE);

  plist = new QListView(this);
  plist->setSelectionMode(QListView::Single);
  plist->addColumn(QObject::tr("Ticker"), -1);
  plist->addColumn(QObject::tr("L/S"), -1);
  plist->addColumn(QObject::tr("Vol"), -1);
  plist->addColumn(QObject::tr("Buy"), -1);
  plist->addColumn(QObject::tr("Last Date"), -1);
  plist->addColumn(QObject::tr("Value"), -1);
  plist->addColumn(QObject::tr("Profit"), -1);
  connect(plist, SIGNAL(clicked(QListViewItem *)), this, SLOT(buttonStatus(QListViewItem *)));
  basebox->addWidget(plist);

  buttonStatus(0);

  updatePortfolio();
}

PortfolioDialog::~PortfolioDialog ()
{
}

void PortfolioDialog::updatePortfolio ()
{
  plist->clear();

  QString s = config->getData(Config::PortfolioPath);
  s.append("/");
  s.append(portfolio);

  QFile f(s);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);
  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (s.length())
    {
      QStringList l = QStringList::split(",", s, FALSE);
      item = new QListViewItem(plist, l[0], l[1], l[2], l[3]);
    }
  }

  f.close();

  updatePortfolioItems();
}

void PortfolioDialog::updatePortfolioItems ()
{
  QListViewItemIterator it(plist);
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

    QDateTime dt = QDateTime::fromString(details->getDateTime("Last Date"), Qt::ISODate);
    item->setText(4, dt.toString("yyyyMMdd"));

    Setting *r = db->getRecord(dt.toString(DATE_FORMAT), db->getData(dt.toString(DATE_FORMAT)));
    QString last = r->getData("Close");
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
  QString s = config->getData(Config::PortfolioPath);
  s.append("/");
  s.append(portfolio);
  QFile f(s);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);

  QListViewItemIterator it(plist);
  for (; it.current(); ++it)
  {
    item = it.current();
    s = item->text(0);
    s.append(",");
    s.append(item->text(1));
    s.append(",");
    s.append(item->text(2));
    s.append(",");
    s.append(item->text(3));
    stream << s << "\n";
  }

  f.close();

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
    symbol = symbol.remove(0, config->getData(Config::DataPath).length());
    if (symbol.isNull())
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("No symbol selected."));
    else
    {
      QString action = set->getData(tr("Action"));
      QString vol = set->getData(tr("Volume"));
      QString price = set->getData(tr("Price"));

      item = new QListViewItem(plist, symbol, action, vol, price);
      updatePortfolioItems();
    }
  }

  delete set;
  delete dialog;
}

void PortfolioDialog::deleteItem ()
{
  item = plist->selectedItem();
  if (item)
    delete item;

  buttonStatus(0);
}

void PortfolioDialog::modifyItem ()
{
  item = plist->currentItem();
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
    symbol = symbol.remove(0, config->getData(Config::DataPath).length());
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
    setButtonStatus(3, FALSE);
    setButtonStatus(4, FALSE);
  }
  else
  {
    setButtonStatus(3, TRUE);
    setButtonStatus(4, TRUE);
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

