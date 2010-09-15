/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "YahooSymbolDialog.h"
#include "YahooDataBase.h"
#include "Dialog.h"

#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QInputDialog>
#include <QDebug>

YahooSymbolDialog::YahooSymbolDialog ()
{
  _yexchange << "NYSE" << "AX" << "SA" << "TO" << "BO" << "NS" << "L";

  setWindowTitle(tr("Yahoo: Edit Symbols"));
  createMainPage();
  loadSettings();
}

YahooSymbolDialog::~YahooSymbolDialog ()
{
}

void YahooSymbolDialog::createMainPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  setLayout(vbox);

  // symbols box
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Yahoo Symbols"));
  vbox->addWidget(gbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  gbox->setLayout(hbox);

  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  hbox->addWidget(_list);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  _addButton = new QPushButton;
  _addButton->setText(tr("Add"));
  connect(_addButton, SIGNAL(clicked()), this, SLOT(addSymbol()));
  tvbox->addWidget(_addButton);

  _deleteButton = new QPushButton;
  _deleteButton->setText(tr("Delete"));
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteSymbol()));
  tvbox->addWidget(_deleteButton);

  tvbox->addStretch(1);

  // button box
  _buttonBox = new QDialogButtonBox;
  vbox->addWidget(_buttonBox);

  QPushButton *b = new QPushButton(tr("&OK"));
  _buttonBox->addButton(b, QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked()), this, SLOT(done()));

  b = new QPushButton(tr("&Cancel"));
  _buttonBox->addButton(b, QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked()), this, SLOT(reject()));
}

void YahooSymbolDialog::addSymbol ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle(tr("Qtstalker: Add Yahoo Symbol"));
  dialog->setLabelText(tr("Enter Yahoo symbols separated by a space"));
  dialog->setInputMode(QInputDialog::TextInput);
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addSymbol2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}
  
void YahooSymbolDialog::addSymbol2 (QString s)
{
  if (s.isEmpty())
    return;

  s = s.trimmed();
  s = s.toUpper();
  QStringList l = s.split(" ");

  YahooDataBase db;
  db.transaction();
  
  QStringList errorList;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString symbol, exchange;
    if (getSymbolExchange(l[loop], symbol, exchange))
    {
      qDebug() << l[loop] << symbol << exchange;
//      errorList.append(l[loop]);
    }
    else
      db.setSymbol(l[loop], symbol, exchange);
  }

  db.commit();

  if (errorList.count())
  {
    qDebug() << "Yahoo::addSymbol:" << errorList;
    
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle(tr("Qtstalker: Yahoo Error"));
    dialog->setMessage(tr("Invalid symbols found. See console messages for details."));
    dialog->show();
  }

  loadSettings();
}

void YahooSymbolDialog::deleteSymbol ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;
  
  YahooDataBase db;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    QString s = item->text();
    db.deleteSymbol(s);
  }
}

void YahooSymbolDialog::loadSettings ()
{
  _list->clear();
  
  YahooDataBase db;
  QStringList l;
  db.getSymbols(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
    new QListWidgetItem(l.at(loop), _list, 0);
}

int YahooSymbolDialog::getSymbolExchange (QString &ysymbol, QString &symbol, QString &exchange)
{
  QString s;
  QStringList l = ysymbol.split(".");
  if (l.count() == 1)
    l.append("NYSE");

  int rc = 0;
  
  switch (_yexchange.indexOf(l[1]))
  {
    // unfortunately yahoo does not separate NYSE,NASDAQ,ASE quotes
    // so we lump them into the NYSE exchange
    case 0: // NYSE
      symbol = l[0];
      exchange = "XNYS";
      break;
    case 1: // AX
      symbol = l[0];
      exchange = "XASX";
      break;
    case 2: // SA
      symbol = l[0];
      exchange = "BVMF";
      break;
    case 3: // TO
      symbol = l[0];
      exchange = "XTSE";
      break;
    case 4: // BO
    case 5: // NS
      symbol = l[0];
      exchange = "XNSE";
      break;
    case 6: // L
      symbol = l[0];
      exchange = "XLON";
      break;
    default: // error
      rc = 1;
      break;
  }

  return rc;
}

void YahooSymbolDialog::done ()
{
  QList<QListWidgetItem *> sel = _list->selectedItems();
  if (! sel.count())
  {
    accept();
    return;
  }

  QStringList l;
  int loop = 0;
  for (; loop < sel.count(); loop++)
    l.append(sel.at(loop)->text());

  emit signalSymbols(l);

  accept();
}

