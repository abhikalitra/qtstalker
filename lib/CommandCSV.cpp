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
#include "SettingString.h"
#include "SettingFile.h"
#include "SettingBool.h"
#include "SettingDouble.h"
#include "SettingDateTime.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"

#include <QtDebug>
#include <QFile>

CommandCSV::CommandCSV (QObject *p) : Command (p)
{
  _type = "CSV";

  _formatType << "EXCHANGE" << "SYMBOL" << "NAME" << "DATE" << "OPEN" << "HIGH" << "LOW" << "CLOSE" << "VOLUME" << "OI";

  _delimiterType << tr("Comma") << tr("Semicolon");
}

int CommandCSV::runScript (Message *sg, Script *script)
{
  // CSV file
  VerifyDataInput vdi;
  QString s = sg->value("CSV_FILE");
  Setting *csv = vdi.setting(SettingFactory::_FILE, script, s);
  if (! csv)
  {
    _message << "invalid CSV_FILE " + s;
    return _ERROR;
  }
  QStringList files = csv->toList();

  // FORMAT
  s = sg->value("FORMAT");
  Setting *formatSetting = vdi.setting(SettingFactory::_STRING, script, s);
  if (! formatSetting)
  {
    _message << "invalid FORMAT " + s;
    return _ERROR;
  }
  QStringList format = formatSetting->toString().split(",");

  // DATE_FORMAT
  s = sg->value("DATE_FORMAT");
  Setting *dateFormatSetting = vdi.setting(SettingFactory::_STRING, script, s);
  if (! dateFormatSetting)
  {
    _message << "invalid DATE_FORMAT " + s;
    return _ERROR;
  }

  QString dateFormat = dateFormatSetting->toString();
  if (dateFormat.isEmpty())
  {
    _message << "invalid DATE_FORMAT " + dateFormat;
    return _ERROR;
  }

  // DELIMITER
  QString delimiter;
  s = sg->value("DELIMITER");
  Setting *delimiterSetting = vdi.setting(SettingFactory::_LIST, script, s);
  if (! delimiterSetting)
  {
    _message << "invalid DELIMITER " + s;
    return _ERROR;
  }

  switch ((DelimiterType) _delimiterType.indexOf(delimiterSetting->toString()))
  {
    case _COMMA:
      delimiter = ",";
      break;
    case _SEMICOLON:
      delimiter = ";";
      break;
    default:
      _message << "invalid DELIMITER " + s;
      return _ERROR;
      break;
  }

  // TYPE
  s = sg->value("TYPE");
  Setting *typeSetting = vdi.setting(SettingFactory::_LIST, script, s);
  if (! typeSetting)
  {
    _message << "invalid TYPE " + s;
    return _ERROR;
  }

  // FILENAME AS SYMBOL
  s = sg->value("FILENAME_AS_SYMBOL");
  Setting *fileNameFlag = vdi.setting(SettingFactory::_BOOL, script, s);
  if (! fileNameFlag)
  {
    _message << "FILENAME_AS_SYMBOL " + s;
    return _ERROR;
  }

  // EXCHANGE
  QString exchange;
  s = sg->value("EXCHANGE");
  Setting *exchangeSetting = vdi.setting(SettingFactory::_STRING, script, s);
  if (exchangeSetting)
    exchange = exchangeSetting->toString();

  QHash<QString, Data *> symbols;
  int loop = 0;
  for (; loop < files.count(); loop++)
  {
    QFile f(files.at(loop));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      _message << "CSV_FILE error " + files.at(loop);
      return _ERROR;
    }

    QString fileNameSymbol;
    if (fileNameFlag->toBool())
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
        _message << "invalid number of fields " + s;
        continue;
      }

      QString symbol, name;
      if (fileNameFlag->toBool())
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
              _message << "invalid DATE " + data.at(loop2);
              flag++;
            }
            else
              bar->set(CurveBar::_DATE, new SettingDateTime(dt));
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
              _message << "invalid " + format.at(loop2) + " " + data.at(loop2);
              flag++;
              break;
            }

            switch ((FormatType) _formatType.indexOf(format.at(loop2)))
            {
              case _OPEN:
                bar->set(CurveBar::_OPEN, new SettingDouble(t));
                break;
              case _HIGH:
                bar->set(CurveBar::_HIGH, new SettingDouble(t));
                break;
              case _LOW:
                bar->set(CurveBar::_LOW, new SettingDouble(t));
                break;
              case _CLOSE:
                bar->set(CurveBar::_CLOSE, new SettingDouble(t));
                break;
              case _VOLUME:
                bar->set(CurveBar::_VOLUME, new SettingDouble(t));
                break;
              case _OI:
                bar->set(CurveBar::_OI, new SettingDouble(t));
                break;
              default:
                break;
            }

            break;
          }
          default:
          {
            _message << "invalid format";
            delete bar;
            qDeleteAll(symbols);
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
        _message << "invalid EXCHANGE " + texchange;
        delete bar;
        continue;
      }

      if (symbol.isEmpty())
      {
        _message << "invalid SYMBOL " + symbol;
        delete bar;
        continue;
      }

      QString key = texchange + ":" + symbol;
      Data *bd = symbols.value(key);
      if (! bd)
      {
        bd = new Symbol;
        bd->set(Symbol::_EXCHANGE, new SettingString(texchange));
        bd->set(Symbol::_SYMBOL, new SettingString(symbol));
        bd->set(Symbol::_TYPE, new SettingString(typeSetting->toString()));
        bd->set(Symbol::_NAME, new SettingString(name));

        symbols.insert(key, bd);
      }

      bd->append(bar);
    }

    f.close();
  }

  QuoteDataBase db;
  QHashIterator<QString, Data *> it(symbols);
  while (it.hasNext())
  {
    it.next();
    Data *bd = it.value();

    db.transaction();
    db.setBars(bd);
    db.commit();
  }

  qDeleteAll(symbols);

  return _OK;
}
