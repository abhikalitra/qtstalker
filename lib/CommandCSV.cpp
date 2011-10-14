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
#include "DataDouble.h"
#include "DataDateTime.h"
#include "VerifyDataInput.h"

#include <QtDebug>
#include <QFile>

CommandCSV::CommandCSV (QObject *p) : Command (p)
{
  _name = "CSV";
  _type = _THREAD;

  _formatType << "EXCHANGE" << "SYMBOL" << "NAME" << "DATE" << "OPEN" << "HIGH" << "LOW" << "CLOSE" << "VOLUME" << "OI";

  _delimiterType << tr("Comma") << tr("Semicolon");
}

int CommandCSV::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // CSV file
  QStringList files;
  QString s = sg->value("CSV_FILE");
  if (vdi.toFile(script, s, files))
  {
    qDebug() << "CommandCSV::runScript: invalid CSV_FILE " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // FORMAT
  QString ts;
  s = sg->value("FORMAT");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandCSV::runScript: invalid FORMAT " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  QStringList format = ts.split(",");

  // DATE_FORMAT
  QString dateFormat;
  s = sg->value("DATE_FORMAT");
  if (vdi.toString(script, s, dateFormat))
  {
    qDebug() << "CommandCSV::runScript: invalid DATE_FORMAT " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // DELIMITER
  QString delimiter;
  s = sg->value("DELIMITER");
  if (vdi.toList(script, s, delimiter))
  {
    qDebug() << "CommandCSV::runScript: invalid DELIMITER " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  switch ((DelimiterType) _delimiterType.indexOf(delimiter))
  {
    case _COMMA:
      delimiter = ",";
      break;
    case _SEMICOLON:
      delimiter = ";";
      break;
    default:
      qDebug() << "CommandCSV::runScript: invalid DELIMITER " << s;
      emit signalResume((void *) this);
      return _ERROR;
      break;
  }

  // TYPE
  QString type;
  s = sg->value("TYPE");
  if (vdi.toList(script, s, type))
  {
    qDebug() << "CommandCSV::runScript: invalid TYPE " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // FILENAME AS SYMBOL
  bool fileNameFlag = FALSE;
  s = sg->value("FILENAME_AS_SYMBOL");
  if (vdi.toBool(script, s, fileNameFlag))
  {
    qDebug() << "CommandCSV::runScript: FILENAME_AS_SYMBOL " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // EXCHANGE
  QString exchange;
  s = sg->value("EXCHANGE");
  vdi.toString(script, s, exchange);

  QHash<QString, Symbol *> symbols;
  int loop = 0;
  for (; loop < files.count(); loop++)
  {
    QFile f(files.at(loop));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug() << "CommandCSV::runScript: CSV_FILE error " << files.at(loop);
      emit signalResume((void *) this);
      return _ERROR;
    }

    QString fileNameSymbol;
    if (fileNameFlag)
    {
      QFileInfo fi(files.at(loop));
      fileNameSymbol = fi.baseName();
    }

    while (! f.atEnd())
    {
      s = f.readLine();
      s = s.trimmed();
      if (s.isEmpty())
        continue;

      QStringList data = s.split(delimiter);

      if (format.count() != data.count())
      {
        qDebug() << "CommandCSV::runScript: invalid number of fields " << s;
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
            qDebug() << "CommandCSV::runScript: invalid format";
            delete bar;
            qDeleteAll(symbols);
            emit signalResume((void *) this);
            return _ERROR;
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

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
