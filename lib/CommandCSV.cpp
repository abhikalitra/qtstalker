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

#include "CommandCSV.h"
#include "QuoteDataBase.h"
#include "Symbol.h"
#include "CurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "SymbolKey.h"
#include "CurveBarKey.h"
#include "GlobalParent.h"
#include "Global.h"

#include <QtDebug>
#include <QFile>
#include <QObject>

CommandCSV::CommandCSV ()
{
  _name = "CSV";
  _formatType << "X" << "S" << "N" << "D" << "O" << "H" << "L" << "C" << "V" << "I";
  _delimiterType << "Comma" << "Semicolon";
  _quoteType << "Stock" << "Futures" << "ETF" << "Mutual";

  QStringList l;
  l << "/tmp/file.csv";
  Data td(DataType::_FILE);
  td.set(l);
  td.setLabel(QObject::tr("CSV File"));
  Entity::set(QString("CSV_FILE"), td);

  td = Data(QString("XSNDOHLCV"));
  td.setLabel(QObject::tr("Format"));
  Entity::set(QString("FORMAT"), td);
  
  td = Data(QString("yyyy-MM-dd"));
  td.setLabel(QObject::tr("Date Format"));
  Entity::set(QString("DATE_FORMAT"), td);
  
  td = Data(_delimiterType, _delimiterType.at(0));
  td.setLabel(QObject::tr("Delimiter"));
  Entity::set(QString("DELIMITER"), td);
  
  td = Data(_quoteType, _quoteType.at(0));
  td.setLabel(QObject::tr("Type"));
  Entity::set(QString("TYPE"), td);
  
  td = Data(FALSE);
  td.setLabel(QObject::tr("Use Filename As Symbol"));
  Entity::set(QString("FILENAME_AS_SYMBOL"), td);
  
  td = Data(QString());
  td.setLabel(QObject::tr("Exchange Override"));
  Entity::set(QString("EXCHANGE"), td);
}

QString CommandCSV::run (CommandParse &, void *)
{
  // CSV file
  Data td;
  Entity::toData(QString("CSV_FILE"), td);
  QStringList files = td.toList();

  // FORMAT
  Entity::toData(QString("FORMAT"), td);
  QString s = td.toString();
  QStringList format;
  int loop = 0;
  for (; loop < s.length(); loop++)
    format << s.at(loop);

  // DATE_FORMAT
  Entity::toData(QString("DATE_FORMAT"), td);
  QString dateFormat = td.toString();

  // DELIMITER
  QString delimiter;
  Entity::toData(QString("DELIMITER"), td);
  s = td.toString();
  switch (_delimiterType.indexOf(s))
  {
    case 0: // Comma
      delimiter = ",";
      break;
    default: // Semicolon
      delimiter = ";";
      break;
  }

  // TYPE
  Entity::toData(QString("TYPE"), td);
  QString type = td.toString();

  // FILENAME AS SYMBOL
  Entity::toData(QString("FILENAME_AS_SYMBOL"), td);
  bool fileNameFlag = td.toBool();

  // EXCHANGE
  Entity::toData(QString("EXCHANGE"), td);
  QString exchange = td.toString();

  CurveBarKey cbkeys;
  SymbolKey symkeys;
  
  // away we go
  QHash<QString, Symbol *> symbols;
  for (loop = 0; loop < files.size(); loop++)
  {
    QFile f(files.at(loop));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug() << "CommandCSV::runScript: CSV_FILE error" << files.at(loop);
      continue;
    }

    QString fileNameSymbol;
    if (fileNameFlag)
    {
      QFileInfo fi(files.at(loop));
      fileNameSymbol = fi.baseName();
    }

    while (! f.atEnd())
    {
      QString s = f.readLine();
      s = s.trimmed();
      if (s.isEmpty())
        continue;

      QStringList data = s.split(delimiter);

      if (format.size() != data.size())
      {
        qDebug() << "CommandCSV::runScript: invalid number of fields " << format.size() << data.size() << s;
        continue;
      }

      QString symbol, name;
      if (fileNameFlag)
      {
        symbol = fileNameSymbol;
        name = fileNameSymbol;
      }

      QString texchange = exchange;

      CurveBar bar;
      int loop2 = 0;
      int flag = 0;
      for (; loop2 < format.count(); loop2++)
      {
        switch ((FormatType) _formatType.indexOf(format.at(loop2)))
        {
          case _EXCHANGE:
            texchange = data.at(loop2).trimmed();
            break;
          case _SYMBOL:
            symbol = data.at(loop2).trimmed();
            break;
          case _NAME:
            name = data.at(loop2).trimmed();
            break;
          case _DATE:
          {
            QDateTime dt = QDateTime::fromString(data.at(loop2).trimmed(), dateFormat);
            if (! dt.isValid())
            {
              qDebug() << "CommandCSV::runScript: invalid DATE " << data.at(loop2);
              flag++;
            }
            else
              bar.set(cbkeys.indexToString(CurveBarKey::_DATE), Data(dt));
            break;
          }
          case _OPEN:
          case _HIGH:
          case _LOW:
          case _CLOSE:
          case _VOLUME:
          case _OI:
          {
            bool ok;
            s = data.at(loop2).trimmed();
            double t = s.toDouble(&ok);
            if (! ok)
            {
              qDebug() << "CommandCSV::runScript: invalid " << format.at(loop2) << " " + data.at(loop2);
              flag++;
              break;
            }

            switch ((FormatType) _formatType.indexOf(format.at(loop2)))
            {
              case _OPEN:
                bar.set(cbkeys.indexToString(CurveBarKey::_OPEN), Data(t));
                break;
              case _HIGH:
                bar.set(cbkeys.indexToString(CurveBarKey::_HIGH), Data(t));
                break;
              case _LOW:
                bar.set(cbkeys.indexToString(CurveBarKey::_LOW), Data(t));
                break;
              case _CLOSE:
                bar.set(cbkeys.indexToString(CurveBarKey::_CLOSE), Data(t));
                break;
              case _VOLUME:
                bar.set(cbkeys.indexToString(CurveBarKey::_VOLUME), Data(t));
                break;
              case _OI:
                bar.set(cbkeys.indexToString(CurveBarKey::_OI), Data(t));
                break;
              default:
                break;
            }

            break;
          }
          default:
          {
            qDeleteAll(symbols);
            qDebug() << "CommandCSV::runScript: invalid format";
            return _returnCode;
            break;
          }
        }

        if (flag)
          break;
      }

      if (flag)
        continue;

      if (texchange.isEmpty())
      {
        qDebug() << "CommandCSV::runScript: invalid EXCHANGE " << texchange;
        continue;
      }

      if (symbol.isEmpty())
      {
        qDebug() << "CommandCSV::runScript: invalid SYMBOL " << symbol;
        continue;
      }

      QString key = texchange + ":" + symbol;
      Symbol *bd = symbols.value(key);
      if (! bd)
      {
        bd = new Symbol;
        bd->set(symkeys.indexToString(SymbolKey::_SYMBOL), Data(key));
	bd->set(symkeys.indexToString(SymbolKey::_TYPE), Data(type));
	bd->set(symkeys.indexToString(SymbolKey::_NAME), Data(name));
        symbols.insert(key, bd);
      }

      QString ts = QString::number(bd->ekeyCount() + 1);
      bd->setEntity(ts, bar);
    }

    f.close();
  }

  QuoteDataBase db;
  QHashIterator<QString, Symbol *> it(symbols);
  while (it.hasNext())
  {
    it.next();
    Symbol *bd = it.value();

    db.transaction();
    db.setBars(bd);
    db.commit();
  }

  qDeleteAll(symbols);

  _returnCode = "OK";
  return _returnCode;
}
