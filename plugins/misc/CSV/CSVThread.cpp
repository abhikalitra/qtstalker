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
#include "DBPluginFactory.h"
#include "DBPlugin.h"
#include "Config.h"
#include "Indicator.h"

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDateTime>

CSVThread::CSVThread (Setting *rule)
{
  _rule = rule;
}

void CSVThread::run ()
{
  if (! _rule)
  {
    emit signalMessage(QString("Rule missing"));
    return;
  }

  QString k = "Plugin";
  QString type;
  _rule->getData(k, type);
  if (type.isEmpty())
  {
    emit signalMessage(QString("Type missing"));
    return;
  }

  k = "Delimeter";
  QString delimeter;
  _rule->getData(k, delimeter);
  if (delimeter.isEmpty())
  {
    emit signalMessage(QString("Delimeter missing"));
    return;
  }

  k = "File";
  QString file;
  _rule->getData(k, file);
  if (file.isEmpty())
  {
    emit signalMessage(QString("File missing"));
    return;
  }

  k = "FileSymbol";
  QString d;
  _rule->getData(k, d);
  if (d.isEmpty())
  {
    emit signalMessage(QString("Use filename as symbol missing"));
    return;
  }
  int fileSymbol = d.toInt();

  k = "Rule";
  _rule->getData(k, d);
  if (d.isEmpty())
  {
    emit signalMessage(QString("Rule missing"));
    return;
  }

  QStringList fieldList = d.split(",");
  if (! fieldList.count())
  {
    emit signalMessage(QString("Rule empty"));
    return;
  }

  QFile f(file);
  if (! f.open(QIODevice::ReadOnly))
  {
    emit signalMessage(QString("Error opening CSV file"));
    return;
  }
  
  QTextStream stream(&f);

  QStringList fields;
  fields << "Exchange" << "Symbol" << "Open" << "High" << "Low" << "Close";
  fields << "Volume" << "OI" << "Ignore" << "Name";

  int lineCount = 0;
  QFileInfo fi(f);
  QString fName = fi.fileName();

  DBPluginFactory fac;

  Config config;
  QString pluginPath;
  config.getData(Config::DBPluginPath, pluginPath);
  
  while (stream.atEnd() == 0)
  {
    QString ts = stream.readLine();
    ts = ts.trimmed();

    QStringList pl = ts.split(delimeter);
    lineCount++;
    
    if (pl.count() != fieldList.count())
    {
      QString s = fName + tr(": Line# ") + QString::number(lineCount);
      s.append(tr("Number of fields in file != rule format"));
      emit signalMessage(s);
      continue;
    }

    QDateTime date;
    QString exchange, symbol, name, open, high, low, close, volume, oi;
    int fieldLoop;
    bool flag = FALSE;
    for (fieldLoop = 0; fieldLoop < (int) fieldList.count(); fieldLoop++)
    {
      switch ((Field) fields.indexOf(fieldList[fieldLoop]))
      {
        case Exchange:
        {
          exchange = pl[fieldLoop];
          break;
        }
        case Symbol:
        {
          symbol = pl[fieldLoop];
	  break;
	}
        case Name:
	{
          name = pl[fieldLoop];
	  break;
	}
        case Open:
        {
          bool ok;
          pl[fieldLoop].toDouble(&ok);
          if (! ok)
          {
            QString s = fName + tr(": Line# ") + QString::number(lineCount);
            s.append(tr("Bad open:") + pl[fieldLoop]);
            emit signalMessage(s);
            flag = TRUE;
            break;
          }
            
          open = pl[fieldLoop];
          break;
        }
        case High:
        {
          bool ok;
          pl[fieldLoop].toDouble(&ok);
          if (! ok)
          {
            QString s = fName + tr(": Line# ") + QString::number(lineCount);
            s.append(tr("Bad high:") + pl[fieldLoop]);
            emit signalMessage(s);
            flag = TRUE;
            break;
          }

          high = pl[fieldLoop];
          break;
        }
        case Low:
        {
          bool ok;
          pl[fieldLoop].toDouble(&ok);
          if (! ok)
          {
            QString s = fName + tr(": Line# ") + QString::number(lineCount);
            s.append(tr("Bad low:") + pl[fieldLoop]);
            emit signalMessage(s);
            flag = TRUE;
            break;
          }

          low = pl[fieldLoop];
          break;
        }
        case Close:
        {
          bool ok;
          pl[fieldLoop].toDouble(&ok);
          if (! ok)
          {
            QString s = fName + tr(": Line# ") + QString::number(lineCount);
            s.append(tr("Bad close:") + pl[fieldLoop]);
            emit signalMessage(s);
            flag = TRUE;
            break;
          }

          close = pl[fieldLoop];
          break;
        }
        case Volume:
        {
          bool ok;
          pl[fieldLoop].toDouble(&ok);
          if (! ok)
          {
            QString s = fName + tr(": Line# ") + QString::number(lineCount);
            s.append(tr("Bad volume:") + pl[fieldLoop]);
            emit signalMessage(s);
            flag = TRUE;
            break;
          }

          volume = pl[fieldLoop];
          break;
        }
        case OI:
        {
          bool ok;
          pl[fieldLoop].toDouble(&ok);
          if (! ok)
          {
            QString s = fName + tr(": Line# ") + QString::number(lineCount);
            s.append(tr("Bad oi:") + pl[fieldLoop]);
            emit signalMessage(s);
            flag = TRUE;
            break;
          }

          oi = pl[fieldLoop];
          break;
        }
        default:
        {
          if (fieldList[fieldLoop].contains("Date"))
          {
            QStringList l = fieldList[fieldLoop].split(":");
            date = QDateTime::fromString(pl[fieldLoop], l[1]);
            if (! date.isValid())
            {
              QString s = fName + tr(": Line# ") + QString::number(lineCount);
              s.append(tr("Bad date:") + pl[fieldLoop]);
              emit signalMessage(s);
              flag = TRUE;
              break;
            }

            break;
          }

          if (fieldList[fieldLoop].contains("Time"))
          {
            QStringList l = fieldList[fieldLoop].split(":");
            QTime time = QTime::fromString(pl[fieldLoop], l[1]);
            if (! time.isValid())
            {
              QString s = fName + tr(": Line# ") + QString::number(lineCount);
              s.append(tr("Bad time:") + pl[fieldLoop]);
              emit signalMessage(s);
              flag = TRUE;
              break;
            }

            date.setTime(time);
            break;
          }

          break;
        }
      }
    }
      
    if (flag)
      continue;

    if (! date.isValid())
      continue;

    if (symbol.isEmpty())
    {
      if (fileSymbol)
        symbol = fName;
      else
        continue;
    }

    if (exchange.isEmpty())
      continue;
    
    DBPlugin *plug = fac.plugin(pluginPath, type);
    if (! plug)
      continue;

    QStringList l;
    l << "QUOTE" << type << "SET_QUOTE" << exchange << symbol << "yyyy-MM-ddTHH:mm:ss";
    l << date.toString(Qt::ISODate) << open << high << low << close << volume;
    if (type == "Futures")
      l << oi;

    Indicator ind;
    plug->scriptCommand(l, ind);
  }

  f.close();

  // format = QUOTE,PLUGIN,SAVE_QUOTES
  DBPlugin *plug = fac.plugin(pluginPath, type);
  if (plug)
  {
    Indicator ind;
    QStringList l;
    l << "QUOTE" << type << "SAVE_QUOTES";
    plug->scriptCommand(l, ind);
  }
  
  emit signalMessage(tr("Done"));
}

