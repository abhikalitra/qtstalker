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
#include "Script.h"
#include "CurveBar.h"

#include <QtDebug>
#include <QFile>

CommandCSV::CommandCSV (QObject *p) : Command (p)
{
  _type = "CSV";

  _formatType << "SYMBOL" << "NAME" << "DATE" << "OPEN" << "HIGH" << "LOW" << "CLOSE" << "VOLUME" << "OI";

  _delimiterType << tr("Comma") << tr("Semicolon");
}

int CommandCSV::runScript (Data *sg, Script *)
{
  QStringList files = sg->get("CSV_FILE").toStringList();
  if (! files.count())
  {
    qDebug() << _type << "::runScript: invalid CSV_FILE" << files;
    return _ERROR;
  }

  QStringList format = sg->get("FORMAT").toString().split(",");
  if (! format.count())
  {
    qDebug() << _type << "::runScript: invalid FORMAT" << format;
    return _ERROR;
  }

  QString dateFormat = sg->get("DATE_FORMAT").toString();
  if (dateFormat.isEmpty())
  {
    qDebug() << _type << "::runScript: invalid DATE_FORMAT" << dateFormat;
    return _ERROR;
  }

  QString delimiter;
  QString s = sg->get("DELIMITER").toString();
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

  QString typ = sg->get("TYPE").toString();
  if (typ.isEmpty())
  {
    qDebug() << _type << "::runScript: invalid TYPE" << typ;
    return _ERROR;
  }

  bool fileNameFlag = sg->get("FILENAME_AS_SYMBOL").toBool();

  QHash<QString, Data *> symbols;

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

      Data *bar = new CurveBar;

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
              bar->set(CurveBar::_DATE, QVariant(dt));
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
              qDebug() << _type << "::runScript: invalid" << format.at(loop2) << data.at(loop2);
              flag++;
              break;
            }

            switch ((FormatType) _formatType.indexOf(format.at(loop2)))
            {
              case _OPEN:
                bar->set(CurveBar::_OPEN, QVariant(t));
                break;
              case _HIGH:
                bar->set(CurveBar::_HIGH, QVariant(t));
                break;
              case _LOW:
                bar->set(CurveBar::_LOW, QVariant(t));
                break;
              case _CLOSE:
                bar->set(CurveBar::_CLOSE, QVariant(t));
                break;
              case _VOLUME:
                bar->set(CurveBar::_VOLUME, QVariant(t));
                break;
              case _OI:
                bar->set(CurveBar::_OI, QVariant(t));
                break;
              default:
                break;
            }

            break;
          }
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

      Data *bd = symbols.value(symbol);
      if (! bd)
      {
        bd = new Symbol;
        bd->set(Symbol::_SYMBOL, QVariant(symbol));
        bd->set(Symbol::_TYPE, QVariant(typ));
        if (! name.isEmpty())
          bd->set(Symbol::_NAME, QVariant(name));

        symbols.insert(symbol, bd);
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

Data * CommandCSV::settings ()
{
  Data *sg = new Data;
  sg->set("CSV_FILE", QVariant(QStringList()));
  sg->set("FORMAT", QVariant(QString()));
  sg->set("DATE_FORMAT", QVariant(QString()));
  sg->set("DELIMITER", QVariant(QString("Comma")));
  sg->set("STOCK", QVariant(QString("Stock")));
  sg->set("FILENAME_AS_SYMBOL", QVariant(FALSE));
  return sg;
}
