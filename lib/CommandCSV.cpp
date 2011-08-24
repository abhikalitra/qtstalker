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
#include "SettingString.h"
#include "SettingList.h"
#include "SettingBool.h"
#include "QuoteDataBase.h"
#include "BarData.h"
#include "Script.h"

#include <QtDebug>
#include <QFile>

CommandCSV::CommandCSV (QObject *p) : Command (p)
{
  _type = "CSV";

  _formatType << "SYMBOL" << "NAME" << "DATE" << "OPEN" << "HIGH" << "LOW" << "CLOSE" << "VOLUME" << "OI";

  _delimiterType << tr("Comma") << tr("Semicolon");
}

int CommandCSV::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QStringList files;
  QString key = sg->get("CSV_FILE")->getString();
  Setting *set = script->setting(key);
  if (set)
    files = set->getFile();
  else
    files << key;

  QStringList format = sg->get("FORMAT")->getString().split(",");

  QString dateFormat = sg->get("DATE_FORMAT")->getString();

  QString delimiter;
  QString s = sg->get("DELIMITER")->getString();
  switch ((DelimiterType) _delimiterType.indexOf(s))
  {
    case _COMMA:
      delimiter = ",";
      break;
    case _SEMICOLON:
      delimiter = ";";
      break;
    default:
      qDebug() << _type << "::runScript: invalid DELIMITER" << s;
      return _ERROR;
      break;
  }

  QString typ = sg->get("TYPE")->getString();

  bool fileNameFlag = sg->get("FILENAME_AS_SYMBOL")->getBool();

  QHash<QString, BarData *> symbols;

  int loop = 0;
  for (; loop < files.count(); loop++)
  {
    QFile f(files.at(loop));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug() << _type << "::runScript: CSV_FILE error" << files.at(loop);
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
        qDebug() << _type << "::runScript: invalid number of fields" << s;
        continue;
      }

      QString symbol, name;
      if (fileNameFlag)
      {
        symbol = fileNameSymbol;
        name = fileNameSymbol;
      }

      Bar *bar = new Bar;

      int loop2 = 0;
      int flag = 0;
      for (; loop2 < format.count(); loop2++)
      {
        switch ((FormatType) _formatType.indexOf(format.at(loop2)))
        {
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
              qDebug() << _type << "::runScript: invalid DATE" << data.at(loop2);
              flag++;
            }
            else
              bar->setDates(dt, dt);
            break;
          }
          case _OPEN:
            if (bar->setOpen(data.at(loop2).trimmed()))
            {
              qDebug() << _type << "::runScript: invalid OPEN" << data.at(loop2);
              flag++;
            }
            break;
          case _HIGH:
            if (bar->setHigh(data.at(loop2).trimmed()))
            {
              qDebug() << _type << "::runScript: invalid HIGH" << data.at(loop2);
              flag++;
            }
            break;
          case _LOW:
            if (bar->setLow(data.at(loop2).trimmed()))
            {
              qDebug() << _type << "::runScript: invalid LOW" << data.at(loop2);
              flag++;
            }
            break;
          case _CLOSE:
            if (bar->setClose(data.at(loop2).trimmed()))
            {
              qDebug() << _type << "::runScript: invalid CLOSE" << data.at(loop2);
              flag++;
            }
            break;
          case _VOLUME:
            if (bar->setVolume(data.at(loop2).trimmed()))
            {
              qDebug() << _type << "::runScript: invalid VOLUME" << data.at(loop2);
              flag++;
            }
            break;
          case _OI:
            if (bar->setOI(data.at(loop2).trimmed()))
            {
              qDebug() << _type << "::runScript: invalid OI" << data.at(loop2);
              flag++;
            }
            break;
          default:
          {
            qDebug() << _type << "::runScript: invalid format";
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

      if (symbol.isEmpty())
      {
        qDebug() << _type << "::runScript: empty SYMBOL" << s;
        delete bar;
        continue;
      }

      BarData *bd = symbols.value(symbol);
      if (! bd)
      {
        bd = new BarData;
        bd->setKey(symbol);
        bd->setType(typ);
        if (! name.isEmpty())
          bd->setName(name);

        symbols.insert(symbol, bd);
      }

      bd->append(bar);
    }

    f.close();
  }

  QuoteDataBase db;
  QHashIterator<QString, BarData *> it(symbols);
  while (it.hasNext())
  {
    it.next();
    BarData *bd = it.value();

    db.transaction();
    db.setBars(bd);
    db.commit();
  }

  qDeleteAll(symbols);

  return _OK;
}

SettingGroup * CommandCSV::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(QString());
  ss->setKey("CSV_FILE");
  sg->set(ss);

  ss = new SettingString(QString());
  ss->setKey("FORMAT");
  sg->set(ss);

  ss = new SettingString(QString());
  ss->setKey("DATE_FORMAT");
  sg->set(ss);

  SettingList *sl = new SettingList(_delimiterType, tr("Comma"));
  sl->setKey("DELIMITER");
  sg->set(sl);

  ss = new SettingString(QString("Stock"));
  ss->setKey("TYPE");
  sg->set(ss);

  SettingBool *sb = new SettingBool(FALSE);
  sb->setKey("FILENAME_AS_SYMBOL");
  sg->set(sb);

  return sg;
}
