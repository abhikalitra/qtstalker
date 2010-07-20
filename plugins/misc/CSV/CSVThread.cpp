/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "CSVThread.h"
#include "Indicator.h"
#include "QuoteServerRequest.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

CSVThread::CSVThread (QObject *p) : QThread (p)
{
  _rule = 0;
  _fileSymbol = 0;
  _lineCount = 0;
  _stopFlag = 0;
  _removeSuffix = 0;

  _fields << "Exchange" << "Symbol" << "Open" << "High" << "Low" << "Close";
  _fields << "Volume" << "OI" << "Ignore" << "Name";
}

void CSVThread::setRule (CSVRule *rule)
{
  _rule = rule;
}

void CSVThread::run ()
{
  // verify rule parms
  if (verifyRule())
  {
    quit();
    return;
  }

  // open csv file
  QFile f(_file);
  if (! f.open(QIODevice::ReadOnly))
  {
    emit signalMessage(QString("Error opening CSV file"));
    quit();
    return;
  }

  QTextStream stream(&f);

  _lineCount = 0;
//  int records = 0;
  _stopFlag = 0;
  
  QFileInfo fi(f);
  _fileName = fi.fileName();

  // try to remove the file suffix
  if (_removeSuffix)
  {
    QString s = fi.completeSuffix();
    if (! s.isEmpty())
    {
      s.prepend("."); // add the dot because Qt does not include it
      _fileName = _fileName.remove(s, Qt::CaseSensitive);
    }
  }

  QHash<QString, QString> commands;
  QStringList nameCommands;

  while (stream.atEnd() == 0)
  {
    if (_stopFlag)
    {
      f.close();
      emit signalMessage(QString("*** " + tr("Import cancelled") + " ***"));
      quit();
      return;
    }
    
    QString ts = stream.readLine().trimmed();

    QStringList pl = ts.split(_delimeter);

    _lineCount++;

    // verify and populate the bar fields
    CSVBar bar;
    if (verifyCSVBar(pl, bar))
      continue;

    // verify date
    if (! bar.date.isValid())
    {
      QString s = _fileName + tr(": Line ") + QString::number(_lineCount) + tr(": bad date");
      emit signalMessage(s);
      continue;
    }

    // verify we have a valid symbol
    if (bar.symbol.isEmpty())
    {
      if (_fileSymbol)
        bar.symbol = _fileName;
      else
      {
        QString s = _fileName + tr(": Line ") + QString::number(_lineCount) + tr(": symbol missing");
        emit signalMessage(s);
        continue;
      }
    }

    // verify we have an exchange
    if (bar.exchange.isEmpty())
    {
      bar.exchange = _rule->exchange;
      if (bar.exchange.isEmpty())
      {
        QString s = _fileName + tr(": Line ") + QString::number(_lineCount) + tr(": exchange missing");
        emit signalMessage(s);
        continue;
      }
    }

    QString key = bar.exchange + ":" + bar.symbol;
    QString command = commands.value(key);
    if (command.isEmpty())
    {
      // create new command entry
      command = "Quotes,Set";
      command.append("," + _type);
      command.append("," + bar.exchange);
      command.append("," + bar.symbol);
      command.append(",yyyyMMddHHmmss");

      if (! bar.name.isEmpty())
      {
        QStringList tl;
        tl << "Details" << "S" << bar.exchange << bar.symbol << "Name" << bar.name;
        nameCommands.append(tl.join(",") + "\n");
      }
    }

    command.append("," + bar.date.toString("yyyyMMddHHmmss"));
    command.append("," + bar.open);
    command.append("," + bar.high);
    command.append("," + bar.low);
    command.append("," + bar.close);
    command.append("," + bar.volume);
    if (_type == "F")
      command.append("," + bar.oi);

    commands.insert(key, command);
  }

  f.close();

  // save the quotes
  QHashIterator<QString, QString> it(commands);
  while (it.hasNext())
  {
    it.next();

    QuoteServerRequest qsr;
    QString s = it.value() + "\n";
    if (qsr.run(s))
      emit signalMessage(tr("ERROR ") + it.key() + tr(" quotes ignored"));
  }

  // save any names
  int loop = 0;
  for (; loop < nameCommands.count(); loop++)
  {
    QuoteServerRequest qsr;
    if (qsr.run(nameCommands[loop]))
      emit signalMessage(tr("ERROR saving name, ignored"));
  }

  // send the done message along with some stats
//  emit signalMessage(tr("Quotes imported: ") + QString::number(records));
  emit signalMessage(tr("Quotes imported"));

  quit();
}

void CSVThread::stop ()
{
  _stopFlag = 1;
}

int CSVThread::verifyRule ()
{
  if (! _rule)
  {
    emit signalMessage(QString("Rule missing"));
    return 1;
  }

  _type = _rule->type;
  if (_type.isEmpty())
  {
    emit signalMessage(QString("Type missing"));
    return 1;
  }

  _delimeter = _rule->delimeter;
  if (_delimeter.isEmpty())
  {
    emit signalMessage(QString("Delimeter missing"));
    return 1;
  }

  _file = _rule->file;
  if (_file.isEmpty())
  {
    emit signalMessage(QString("File missing"));
    return 1;
  }

  _fileSymbol = _rule->fileSymbol;

  _removeSuffix = _rule->removeSuffix;

  QString d = _rule->rule;
  if (d.isEmpty())
  {
    emit signalMessage(QString("Rule missing"));
    return 1;
  }

  _fieldList = d.split(",");
  if (! _fieldList.count())
  {
    emit signalMessage(QString("Rule empty"));
    return 1;
  }

  return 0;
}

int CSVThread::verifyCSVBar (QStringList &pl, CSVBar &bar)
{
  if (pl.count() != _fieldList.count())
  {
    QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
    s.append(tr(": number of fields in file != rule format"));
    emit signalMessage(s);
    return 1;
  }

  int fieldLoop = 0;
  for (; fieldLoop < (int) _fieldList.count(); fieldLoop++)
  {
    switch ((Field) _fields.indexOf(_fieldList[fieldLoop]))
    {
      case Ignore:
        break;
      case Exchange:
        bar.exchange = pl[fieldLoop];
        break;
      case Symbol:
        bar.symbol = pl[fieldLoop];
        break;
      case Name:
        bar.name = pl[fieldLoop];
        break;
      case Open:
      {
        bool ok;
        pl[fieldLoop].toDouble(&ok);
        if (! ok)
        {
          QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
          s.append(tr(": bad open: ") + pl[fieldLoop]);
          emit signalMessage(s);
          return 1;
          break;
        }
        
        bar.open = pl[fieldLoop];
        break;
      }
      case High:
      {
        bool ok;
        pl[fieldLoop].toDouble(&ok);
        if (! ok)
        {
          QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
          s.append(tr(": bad high: ") + pl[fieldLoop]);
          emit signalMessage(s);
          return 1;
          break;
        }

        bar.high = pl[fieldLoop];
        break;
      }
      case Low:
      {
        bool ok;
        pl[fieldLoop].toDouble(&ok);
        if (! ok)
        {
          QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
          s.append(tr(": bad low: ") + pl[fieldLoop]);
          emit signalMessage(s);
          return 1;
          break;
        }

        bar.low = pl[fieldLoop];
        break;
      }
      case Close:
      {
        bool ok;
        pl[fieldLoop].toDouble(&ok);
        if (! ok)
        {
          QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
          s.append(tr(": bad close: ") + pl[fieldLoop]);
          emit signalMessage(s);
          return 1;
          break;
        }

        bar.close = pl[fieldLoop];
        break;
      }
      case Volume:
      {
        bool ok;
        pl[fieldLoop].toDouble(&ok);
        if (! ok)
        {
          QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
          s.append(tr(": bad volume: ") + pl[fieldLoop]);
          emit signalMessage(s);
          return 1;
          break;
        }

        bar.volume = pl[fieldLoop];
        break;
      }
      case OI:
      {
        bool ok;
        pl[fieldLoop].toDouble(&ok);
        if (! ok)
        {
          QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
          s.append(tr(": bad oi: ") + pl[fieldLoop]);
          emit signalMessage(s);
          return 1;
          break;
        }

        bar.oi = pl[fieldLoop];
        break;
      }
      default:
      {
        if (_fieldList[fieldLoop].contains("Date"))
        {
          QStringList l = _fieldList[fieldLoop].split("=");
          bar.date = QDateTime::fromString(pl[fieldLoop], l[1]);
          if (! bar.date.isValid())
          {
            QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
            s.append(tr(": bad date: ") + pl[fieldLoop]);
            emit signalMessage(s);
            return 1;
            break;
          }

          break;
        }

        if (_fieldList[fieldLoop].contains("Time"))
        {
          QStringList l = _fieldList[fieldLoop].split("=");
          QTime time = QTime::fromString(pl[fieldLoop], l[1]);
          if (! time.isValid())
          {
            QString s = _fileName + tr(": Line ") + QString::number(_lineCount);
            s.append(tr(": bad time: ") + pl[fieldLoop]);
            emit signalMessage(s);
            return 1;
            break;
          }

          bar.date.setTime(time);
          break;
        }

        break;
      }
    }
  }

  return 0;
}

