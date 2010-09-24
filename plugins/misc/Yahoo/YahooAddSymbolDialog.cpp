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

#include "YahooAddSymbolDialog.h"
#include "Globals.h"
#include "YahooDataBase.h"

#include <QtDebug>
#include <QLineEdit>

YahooAddSymbolDialog::YahooAddSymbolDialog ()
{
  _yexchange << "NYSE" << "AX" << "SA" << "TO" << "BO" << "NS" << "L";

  setWindowTitle("QtStalker" + g_session + ": Yahoo " + tr("Add Symbols"));

  QLabel *label = new QLabel(tr("Enter Yahoo symbols separated by a space"));
  _ndvbox->insertWidget(0, label);

  _tabs->setTabText(0, tr("Symbols"));
}

void YahooAddSymbolDialog::done ()
{
  QString s = _name->lineEdit()->text();
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
      errorList.append(l[loop]);
    else
      db.setSymbol(l[loop], symbol, exchange);
  }

  db.commit();

  if (errorList.count())
  {
    errorList.prepend(tr("Invalid symbols found\n"));
    setMessage(errorList.join("\n"));
    return;
  }

  emit signalNew();

  accept();
}

int YahooAddSymbolDialog::getSymbolExchange (QString &ysymbol, QString &symbol, QString &exchange)
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

