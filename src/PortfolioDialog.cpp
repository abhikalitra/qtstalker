/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "Setting.h"
#include "FuturesData.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "HelpWindow.h"
#include "newchart.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlayout.h>

PortfolioDialog::PortfolioDialog (QString p) : QTabDialog (0, "PortfolioDialog", TRUE)
{
  portfolio = p;
  
  QString s = tr("Qtstalker: Portfolio");
  s.append(" ");
  s.append(portfolio);
  setCaption(s);

  QWidget *w = new QWidget(this);
    
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);
  vbox->addSpacing(10);
  
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(savePortfolio()));

  toolbar->addButton("add", QPixmap(newchart), tr("&Add"));
  connect(toolbar->getButton("add"), SIGNAL(clicked()), this, SLOT(addItem()));
  toolbar->getButton("add")->setAccel(CTRL+Key_A);

  toolbar->addButton("edit", QPixmap(edit), tr("&Edit"));
  connect(toolbar->getButton("edit"), SIGNAL(clicked()), this, SLOT(modifyItem()));
  toolbar->setButtonStatus("edit", FALSE);
  toolbar->getButton("edit")->setAccel(CTRL+Key_E);

  toolbar->addButton("delete", QPixmap(deleteitem), tr("&Delete"));
  connect(toolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteItem()));
  toolbar->setButtonStatus("delete", FALSE);
  toolbar->getButton("delete")->setAccel(CTRL+Key_D);

  plist = new QListView(w);
  plist->setSelectionMode(QListView::Single);
  plist->addColumn(QObject::tr("Symbol"), -1);
  plist->addColumn(QObject::tr("L/S"), -1);
  plist->addColumn(QObject::tr("Vol"), -1);
  plist->addColumn(QObject::tr("Buy"), -1);
  plist->addColumn(QObject::tr("Last Date"), -1);
  plist->addColumn(QObject::tr("Value"), -1);
  plist->addColumn(QObject::tr("Profit"), -1);
  connect(plist, SIGNAL(clicked(QListViewItem *)), this, SLOT(buttonStatus(QListViewItem *)));
  vbox->insertWidget(2, plist);
  
  addTab(w, tr("Details"));
  
  setCancelButton();
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(slotHelp()));

  buttonStatus(0);

  updatePortfolio();
}

PortfolioDialog::~PortfolioDialog ()
{
}

void PortfolioDialog::updatePortfolio ()
{
  plist->clear();

  QString s = config.getData(Config::PortfolioPath);
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

    QString s = config.getData(Config::DataPath);
    s.append("/");
    s.append(symbol);
    QDir dir(s);
    if (! dir.exists(s, TRUE))
      continue;

    ChartDb *plug = new ChartDb;
    plug->openChart(s);
    
    QString type = plug->getData(tr("Type"));
    QString futuresType = plug->getData(tr("FuturesType"));
    Bar *bar = plug->getLastBar();
    
    if (! bar)
    {
      delete bar;
      delete plug;
      continue;
    }
    
    item->setText(4, bar->getDate().getDateString(TRUE));

    QString last = QString::number(bar->getClose());
    item->setText(5, last);

    float total;
    if (! action.compare("Long"))
      total = volume.toFloat() * (last.toFloat() - price.toFloat());
    else
      total = volume.toFloat() * (price.toFloat() - last.toFloat());

    if (! type.compare("Futures"))
      total = futuresProfit(futuresType, total);

    item->setText(6, QString::number(total));
    
    delete bar;
    delete plug;
  }
}

void PortfolioDialog::savePortfolio ()
{
  QString s = config.getData(Config::PortfolioPath);
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
  PrefDialog *dialog = new PrefDialog;
  dialog->createPage(tr("Details"));
  dialog->setCaption(tr("New Portfolio Item"));
  dialog->addSymbolItem(tr("Symbol"), tr("Details"), config.getData(Config::DataPath), QString());

  QStringList l;
  l.append(tr("Long"));
  l.append(tr("Short"));
  dialog->addComboItem(tr("Action"), tr("Details"), l, tr("Long"));
  
  dialog->addFloatItem(tr("Price"), tr("Details"), 0, 0, 9999999999.0);
  dialog->addIntItem(tr("Volume"), tr("Details"), 1, 1, 99999999);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QString symbol = dialog->getSymbol(tr("Symbol"));
    symbol = symbol.remove(0, config.getData(Config::DataPath).length());
    if (symbol.isNull())
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("No symbol selected."));
    else
    {
      QString action = dialog->getCombo(tr("Action"));
      int vol = dialog->getInt(tr("Volume"));
      double price = dialog->getFloat(tr("Price"));

      new QListViewItem(plist, symbol, action, QString::number(vol), QString::number(price));
      updatePortfolioItems();
    }
  }

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

  PrefDialog *dialog = new PrefDialog;
  dialog->createPage(tr("Details"));
  dialog->setCaption(tr("Edit Portfolio Item"));
  dialog->addSymbolItem(tr("Symbol"), tr("Details"), config.getData(Config::DataPath), item->text(0));

  QStringList l;
  l.append(tr("Long"));
  l.append(tr("Short"));
  dialog->addComboItem(tr("Action"), tr("Details"), l, item->text(1));
  
  dialog->addFloatItem(tr("Price"), tr("Details"), item->text(3).toFloat(), 0, 9999999999.0);
  dialog->addIntItem(tr("Volume"), tr("Details"), item->text(2).toInt(), 1, 99999999);
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString symbol = dialog->getSymbol(tr("Symbol"));
    symbol = symbol.remove(0, config.getData(Config::DataPath).length());
    if (symbol.isNull())
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("No symbol selected."));
    else
    {
      QString action = dialog->getCombo(tr("Action"));
      int vol = dialog->getInt(tr("Volume"));
      double price = dialog->getFloat(tr("Price"));
  
      item->setText(0, symbol);
      item->setText(1, action);
      item->setText(2, QString::number(vol));
      item->setText(3, QString::number(price));

      updatePortfolioItems();
    }
  }

  delete dialog;
}

void PortfolioDialog::buttonStatus (QListViewItem *i)
{
  if (! i)
  {
    toolbar->setButtonStatus("edit", FALSE);
    toolbar->setButtonStatus("delete", FALSE);
  }
  else
  {
    toolbar->setButtonStatus("edit", TRUE);
    toolbar->setButtonStatus("delete", TRUE);
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

void PortfolioDialog::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "portfolios.html");
  hw->show();
  reject();
}

