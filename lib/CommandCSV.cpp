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
#include "QuoteDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFile>

CommandCSV::CommandCSV (QObject *p) : Command (p)
{
  _type = "CSV";

  _formatType << "SYMBOL" << "NAME" << "DATE" << "OPEN" << "HIGH" << "LOW" << "CLOSE" << "VOLUME" << "OI";
}

int CommandCSV::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString file = sg->get("CSV_FILE")->getString();
  QFile f(file);
  if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QStringList tl;
    tl << _type << "::runScript: CSV_FILE error" << file;
    emit signalMessage(tl.join(" "));
    qDebug() << tl;
    return _ERROR;
  }

  QStringList format = sg->get("FORMAT")->getString().split(";");

  QString dateFormat = sg->get("DATE_FORMAT")->getString();

  QString delimiter = sg->get("DELIMITER")->getString();

  QString typ = sg->get("TYPE")->getString();

  QHash<QString, BarData *> symbols;

  while (! f.atEnd())
  {
    QString s = f.readLine();
    s = s.trimmed();
    if (s.isEmpty())
      continue;

    QStringList data = s.split(delimiter);

    if (format.count() != data.count())
    {
      QStringList tl;
      tl << _type << "::runScript: invalid number of fields" << s;
      emit signalMessage(tl.join(" "));
      qDebug() << tl;
      continue;
    }

    QString symbol, name;
    Bar *bar = new Bar;

    int loop = 0;
    int flag = 0;
    for (; loop < format.count(); loop++)
    {
      switch ((FormatType) _formatType.indexOf(format.at(loop)))
      {
        case _SYMBOL:
          symbol = data.at(loop).trimmed();
          break;
        case _NAME:
          name = data.at(loop).trimmed();
          break;
        case _DATE:
        {
          QDateTime dt = QDateTime::fromString(data.at(loop).trimmed(), dateFormat);
          if (! dt.isValid())
          {
            QStringList tl;
            tl << _type << "::runScript: invalid DATE" << data.at(loop);
            emit signalMessage(tl.join(" "));
            qDebug() << tl;
            flag++;
          }
          else
            bar->setDates(dt, dt);
          break;
        }
        case _OPEN:
          if (bar->setOpen(data.at(loop).trimmed()))
          {
            QStringList tl;
            tl << _type << "::runScript: invalid OPEN" << data.at(loop);
            emit signalMessage(tl.join(" "));
            qDebug() << tl;
            flag++;
          }
          break;
        case _HIGH:
          if (bar->setHigh(data.at(loop).trimmed()))
          {
            QStringList tl;
            tl << _type << "::runScript: invalid HIGH" << data.at(loop);
            emit signalMessage(tl.join(" "));
            qDebug() << tl;
            flag++;
          }
          break;
        case _LOW:
          if (bar->setLow(data.at(loop).trimmed()))
          {
            QStringList tl;
            tl << _type << "::runScript: invalid LOW" << data.at(loop);
            emit signalMessage(tl.join(" "));
            qDebug() << tl;
            flag++;
          }
          break;
        case _CLOSE:
          if (bar->setClose(data.at(loop).trimmed()))
          {
            QStringList tl;
            tl << _type << "::runScript: invalid CLOSE" << data.at(loop);
            emit signalMessage(tl.join(" "));
            qDebug() << tl;
            flag++;
          }
          break;
        case _VOLUME:
          if (bar->setVolume(data.at(loop).trimmed()))
          {
            QStringList tl;
            tl << _type << "::runScript: invalid VOLUME" << data.at(loop);
            emit signalMessage(tl.join(" "));
            qDebug() << tl;
            flag++;
          }
          break;
        case _OI:
          if (bar->setOI(data.at(loop).trimmed()))
          {
            QStringList tl;
            tl << _type << "::runScript: invalid OI" << data.at(loop);
            emit signalMessage(tl.join(" "));
            qDebug() << tl;
            flag++;
          }
          break;
        default:
        {
          QStringList tl;
          tl << _type << "::runScript: invalid format";
          emit signalMessage(tl.join(" "));
          qDebug() << tl;
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
      QStringList tl;
      tl << _type << "::runScript: empty SYMBOL" << s;
      emit signalMessage(tl.join(" "));
      qDebug() << tl;
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

  ss = new SettingString(QString(","));
  ss->setKey("DELIMITER");
  sg->set(ss);

  ss = new SettingString(QString("Stock"));
  ss->setKey("TYPE");
  sg->set(ss);

  return sg;
}
