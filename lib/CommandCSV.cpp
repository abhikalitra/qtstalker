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
#include "DataFile.h"
#include "DataDouble.h"
#include "DataDateTime.h"
#include "DataString.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "DataList.h"
#include "DataBool.h"

#include <QtDebug>
#include <QFile>

CommandCSV::CommandCSV (QObject *p) : Command (p)
{
  _name = "CSV";

  _formatType << "EXCHANGE" << "SYMBOL" << "NAME" << "DATE" << "OPEN" << "HIGH" << "LOW" << "CLOSE" << "VOLUME" << "OI";

  _delimiterType << tr("Comma") << tr("Semicolon");

  _values.insert(_ParmTypeCSVFile, new DataFile());
  _values.insert(_ParmTypeFormat, new DataString());
  _values.insert(_ParmTypeDateFormat, new DataString());

  DataList *dl = new DataList(_delimiterType.at(0));
  dl->set(_delimiterType);
  _values.insert(_ParmTypeDelimiter, dl);

  QStringList tl;
  tl << "Stock" << "Futures";
  dl = new DataList(tl.at(0));
  dl->set(tl);
  _values.insert(_ParmTypeType, dl);

  _values.insert(_ParmTypeFilenameAsSymbol, new DataBool(FALSE));
  _values.insert(_ParmTypeExchange, new DataString());
}

void CommandCSV::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandCSV::runScript: parse error");
    return;
  }

  // CSV file
  QStringList files = _values.value(_ParmTypeCSVFile)->toList();

  // FORMAT
  QStringList format = _values.value(_ParmTypeFormat)->toString().split(";");

  // DATE_FORMAT
  QString dateFormat = _values.value(_ParmTypeDateFormat)->toString();

  // DELIMITER
  QString delimiter;
  QString s = _values.value(_ParmTypeDelimiter)->toString();
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
  QString type = _values.value(_ParmTypeType)->toString();

  // FILENAME AS SYMBOL
  bool fileNameFlag = _values.value(_ParmTypeFilenameAsSymbol)->toBool();

  // EXCHANGE
  QString exchange = _values.value(_ParmTypeExchange)->toString();

  // away we go
  QHash<QString, Symbol *> symbols;
  int loop = 0;
  for (; loop < files.size(); loop++)
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

      Data *bar = new CurveBar;

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
              bar->set(CurveBar::_DATE, new DataDateTime(dt));
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
                bar->set(CurveBar::_OPEN, new DataDouble(t));
                break;
              case _HIGH:
                bar->set(CurveBar::_HIGH, new DataDouble(t));
                break;
              case _LOW:
                bar->set(CurveBar::_LOW, new DataDouble(t));
                break;
              case _CLOSE:
                bar->set(CurveBar::_CLOSE, new DataDouble(t));
                break;
              case _VOLUME:
                bar->set(CurveBar::_VOLUME, new DataDouble(t));
                break;
              case _OI:
                bar->set(CurveBar::_OI, new DataDouble(t));
                break;
              default:
                break;
            }

            break;
          }
          default:
          {
            delete bar;
            qDeleteAll(symbols);
            Command::error("CommandCSV::runScript: invalid format");
            return;
            break;
          }
        }

        if (flag)
          break;
      }

      if (flag)
      {
        delete bar;
        continue;
      }

      if (texchange.isEmpty())
      {
        qDebug() << "CommandCSV::runScript: invalid EXCHANGE " << texchange;
        delete bar;
        continue;
      }

      if (symbol.isEmpty())
      {
        qDebug() << "CommandCSV::runScript: invalid SYMBOL " << symbol;
        delete bar;
        continue;
      }

      QString key = texchange + ":" + symbol;
      Symbol *bd = symbols.value(key);
      if (! bd)
      {
        bd = new Symbol;
        bd->setExchange(texchange);
        bd->setSymbol(symbol);
        bd->setType(type);
        bd->setName(name);

        symbols.insert(key, bd);
      }

      bd->append(bar);
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

  Command::done(QString());
}
