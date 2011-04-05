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

#include "QUOTE_DATABASE.h"
#include "Strip.h"
#include "QuoteDataBase.h"
#include "Globals.h"
#include "DateRange.h"
#include "BarLength.h"

#include <QtDebug>

QUOTE_DATABASE::QUOTE_DATABASE ()
{
  _plugin = "QUOTE_DATABASE";
  _method << "DELETE" << "DUMP" << "EXCHANGE" << "GET" << "RENAME" << "SEARCH" << "SET";
}

QUOTE_DATABASE::~QUOTE_DATABASE ()
{
  clear();
}

int QUOTE_DATABASE::command (Command *command)
{
  int rc = 1;
  switch (_method.indexOf(command->parm("METHOD")))
  {
    case 0:
      rc = remove(command);
      break;
    case 1:
      rc = dump(command);
      break;
    case 2:
      rc = exchange(command);
      break;
    case 3:
      rc = get(command);
      break;
    case 4:
      rc = rename(command);
      break;
    case 5:
      rc = search(command);
      break;
    case 6:
      rc = set(command);
      break;
    default:
      break;
  }

  return rc;
}

int QUOTE_DATABASE::remove (Command *command)
{
  // PARMS:
  // NAME - semicolon delimited EXCHANGE:SYMBOL list

  QStringList l = command->parm("NAME").split(";");

  QuoteDataBase db;
  db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData bd;
    if (bd.setKey(l.at(loop)))
    {
      qDebug() << _plugin << "::remove: invalid EXCHANGE:SYMBOL" << l.at(loop);
      continue;
    }

    if (db.deleteSymbol(&bd))
      qDebug() << _plugin << "::remove: QuoteDataBase error deleting" << l.at(loop);
  }

  db.commit();

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::dump (Command *command)
{
  // PARMS:
  // EXCHANGE
  // SYMBOL
  // DIRECTORY

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::dump: invalid exchange" << s;
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::dump: invalid symbol" << s;
    return 1;
  }
  bd.setSymbol(s);

  bd.setBarLength(BarLength::_NONE);
  bd.setRange(DateRange::_ALL);

  // create base directory if needed
  QFileInfo fi(command->parm("DIRECTORY"));
  QString path = fi.absolutePath();

  QDir dir;
  if (! dir.mkpath(path))
  {
    qDebug() << _plugin << "::dump: error creating directory" << path;
    return 1;
  }

  // create exchange directory
  path.append("/");
  path.append(bd.exchange());
  if (! dir.mkpath(path))
  {
    qDebug() << _plugin << "::dump: error creating directory" << path;
    return 1;
  }

  path.append("/");
  path.append(bd.symbol());

  QFile file(path);
  if (! file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug() << _plugin << "::dump: error opening csv file" << path;
    return 1;
  }
  QTextStream out(&file);

  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << _plugin << "::dump: QuoteDataBase error";
    return 1;
  }

  Strip strip;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    if (! b)
      continue;

    QStringList l;
    l << bd.exchange() << bd.symbol() << bd.name();

    b->dateTimeString(s);
    l << s;

    strip.strip(b->open(), 4, s);
    l << s;

    strip.strip(b->high(), 4, s);
    l << s;

    strip.strip(b->low(), 4, s);
    l << s;

    strip.strip(b->close(), 4, s);
    l << s;

    strip.strip(b->volume(), 4, s);
    l << s;

    strip.strip(b->oi(), 4, s);
    l << s;

    out << l.join(",") << "\n";
  }

  file.close();

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::exchange (Command *command)
{
  // PARMS:
  // EXCHANGE

  QStringList l;
  QuoteDataBase db;
  if (db.getExchange(l))
  {
    qDebug() << _plugin << "::exchange: QuoteDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_EXCHANGES", l.join(","));

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::get (Command *command)
{
  // PARMS:
  // NAME_DATE
  // NAME_OPEN
  // NAME_HIGH
  // NAME_LOW
  // NAME_CLOSE
  // NAME_VOLUME
  // NAME_OI
  // EXCHANGE
  // SYMBOL
  // LENGTH
  // RANGE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::get: no indicator";
    return 1;
  }

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::get: invalid exchange" << s;
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::get: invalid symbol" << s;
    return 1;
  }
  bd.setSymbol(s);

  QStringList l;
  BarLength bl;
  l = bl.list();
  s = command->parm("LENGTH");
  int length = l.indexOf(s);
  if (length == -1)
  {
    if (s != "-1")
    {
      qDebug() << _plugin << "::get: invalid LENGTH" << command->parm("LENGTH");
      return 1;
    }

    bd.setBarLength(g_barData->barLength());
  }
  else
    bd.setBarLength((BarLength::Length) length);

  DateRange dr;
  int range = dr.toType(command->parm("RANGE"));
  if (range == -1)
  {
    if (command->parm("RANGE") != "-1")
    {
      qDebug() << _plugin << "::get: invalid RANGE" << command->parm("RANGE");
      return 1;
    }

    bd.setRange(g_barData->range());
  }
  else
    bd.setRange(range);

  Curve *dline = 0;
  s = command->parm("NAME_DATE");
  if (! s.isEmpty())
  {
    dline = i->line(s);
    if (dline)
    {
      qDebug() << _plugin << "::get: duplicate NAME_DATE" << s;
      return 1;
    }

    dline = new Curve;
    dline->setLabel(s);
    i->setLine(s, dline);
  }

  Curve *oline = 0;
  s = command->parm("NAME_OPEN");
  if (! s.isEmpty())
  {
    oline = i->line(s);
    if (oline)
    {
      qDebug() << _plugin << "::get: duplicate NAME_OPEN" << s;
      return 1;
    }

    oline = new Curve;
    oline->setLabel(s);
    i->setLine(s, oline);
  }

  Curve *hline = 0;
  s = command->parm("NAME_HIGH");
  if (! s.isEmpty())
  {
    hline = i->line(s);
    if (hline)
    {
      qDebug() << _plugin << "::get: duplicate NAME_HIGH" << s;
      return 1;
    }

    hline = new Curve;
    hline->setLabel(s);
    i->setLine(s, hline);
  }

  Curve *lline = 0;
  s = command->parm("NAME_LOW");
  if (! s.isEmpty())
  {
    lline = i->line(s);
    if (lline)
    {
      qDebug() << _plugin << "::get: duplicate NAME_LOW" << s;
      return 1;
    }

    lline = new Curve;
    lline->setLabel(s);
    i->setLine(s, lline);
  }

  Curve *cline = 0;
  s = command->parm("NAME_CLOSE");
  if (! s.isEmpty())
  {
    cline = i->line(s);
    if (cline)
    {
      qDebug() << _plugin << "::get: duplicate NAME_CLOSE" << s;
      return 1;
    }

    cline = new Curve;
    cline->setLabel(s);
    i->setLine(s, cline);
  }

  Curve *vline = 0;
  s = command->parm("NAME_VOLUME");
  if (! s.isEmpty())
  {
    vline = i->line(s);
    if (vline)
    {
      qDebug() << _plugin << "::get: duplicate NAME_VOLUME" << s;
      return 1;
    }

    vline = new Curve;
    vline->setLabel(s);
    i->setLine(s, vline);
  }

  Curve *iline = 0;
  s = command->parm("NAME_OI");
  if (! s.isEmpty())
  {
    iline = i->line(s);
    if (iline)
    {
      qDebug() << _plugin << "::get: duplicate NAME_OI" << s;
      return 1;
    }

    iline = new Curve;
    iline->setLabel(s);
    i->setLine(s, iline);
  }

  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << _plugin << "::get: QuoteDataBase error";
    return 1;
  }

  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    if (! b)
      continue;

    if (dline)
    {
      CurveBar *cb = new CurveBar;
      cb->setDateTime(b->date());
      dline->setBar(loop, cb);
    }

    if (oline)
      oline->setBar(loop, new CurveBar(b->open()));

    if (hline)
      hline->setBar(loop, new CurveBar(b->high()));

    if (lline)
      lline->setBar(loop, new CurveBar(b->low()));

    if (cline)
      cline->setBar(loop, new CurveBar(b->close()));

    if (vline)
      vline->setBar(loop, new CurveBar(b->volume()));

    if (iline)
      iline->setBar(loop, new CurveBar(b->oi()));
  }

  command->setReturnData(_plugin + "_NAME", bd.name());

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::rename (Command *command)
{
  // PARMS:
  // OLD_EXCHANGE
  // OLD_SYMBOL
  // NEW_EXCHANGE
  // NEW_SYMBOL

  BarData osymbol;
  QString s = command->parm("OLD_EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid OLD_EXCHANGE" << command->parm("OLD_EXCHANGE");
    return 1;
  }
  osymbol.setExchange(s);

  s = command->parm("OLD_SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid OLD_SYMBOL" << command->parm("OLD_SYMBOL");
    return 1;
  }
  osymbol.setSymbol(s);

  BarData nsymbol;
  s = command->parm("NEW_EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid NEW_EXCHANGE" << command->parm("NEW_EXCHANGE");
    return 1;
  }
  nsymbol.setExchange(s);

  s = command->parm("NEW_SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid NEW_SYMBOL" << command->parm("NEW_SYMBOL");
    return 1;
  }
  nsymbol.setSymbol(s);

  QuoteDataBase db;
  if (db.rename(&osymbol, &nsymbol))
  {
    qDebug() << _plugin << "::rename: QuoteDataBase error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::search (Command *command)
{
  // PARMS:
  // EXCHANGE
  // SYMBOL

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::search: invalid EXCHANGE" << command->parm("EXCHANGE");
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::search: invalid SYMBOL" << command->parm("SYMBOL");
    return 1;
  }
  bd.setSymbol(s);

  QList<BarData> l;
  QuoteDataBase db;
  if (db.search(&bd, l))
  {
    qDebug() << _plugin << "::search: QuoteDataBase error";
    return 1;
  }

  QStringList tl;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData t = l.at(loop);
    tl << t.key();
  }

  // now delimit each item with a semicolon
  command->setReturnData(_plugin + "_SYMBOLS", tl.join(";"));

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::set (Command *command)
{
  // PARMS:
  // DATE
  // OPEN
  // HIGH
  // LOW
  // CLOSE
  // VOLUME
  // INTEREST
  // EXCHANGE
  // SYMBOL
  // NAME
  // TYPE
  // SAVE
  // DATE_FORMAT

  // verify SAVE command
  QString s = command->parm("SAVE");
  if (s.length())
  {
    save();
    command->setReturnCode("0");
    return 0;
  }

  BarData tbd;
  
  // verify exchange
  s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::set: EXCHANGE missing";
    return 1;
  }
  tbd.setExchange(s);

  // verify symbol
  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::set: SYMBOL missing";
    return 1;
  }
  tbd.setSymbol(s);

  BarData *symbol = _symbols.value(tbd.key());
  if (! symbol)
  {
    symbol = new BarData;
    symbol->setKey(tbd.key());
    _symbols.insert(tbd.key(), symbol);
  }

  // verify date
  QString format = command->parm("DATE_FORMAT");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::set: DATE_FORMAT missing";
    return 1;
  }

  QDateTime dt = QDateTime::fromString(command->parm("DATE"), format);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::set: invalid DATE or DATE_FORMAT" << format << command->parm("DATE");
    return 1;
  }
  
  Bar *bar = new Bar;

  if (bar->setDates(dt, dt))
  {
    qDebug() << _plugin << "::set: invalid DATE" << dt;
    delete bar;
    return 1;
  }

  // verify open
  if (bar->setOpen(command->parm("OPEN")))
  {
    qDebug() << _plugin << "::set: invalid OPEN" << command->parm("OPEN");
    delete bar;
    return 1;
  }

  // verify high
  if (bar->setHigh(command->parm("HIGH")))
  {
    qDebug() << _plugin << "::set: invalid HIGH" << command->parm("HIGH");
    delete bar;
    return 1;
  }

  // verify low
  if (bar->setLow(command->parm("LOW")))
  {
    qDebug() << _plugin << "::set: invalid LOW" << command->parm("LOW");
    delete bar;
    return 1;
  }

  // verify close
  if (bar->setClose(command->parm("CLOSE")))
  {
    qDebug() << _plugin << "::set: invalid CLOSE" << command->parm("CLOSE");
    delete bar;
    return 1;
  }

  // verify volume
  if (bar->setVolume(command->parm("VOLUME")))
  {
    qDebug() << _plugin << "::set: invalid VOLUME" << command->parm("VOLUME");
    delete bar;
    return 1;
  }

  // optional parms
  
  // verify oi
  s = command->parm("INTEREST");
  if (! s.isEmpty())
  {
    if (bar->setOI(s))
      qDebug() << _plugin << "::set: invalid INTEREST" << s;
  }

  // verify name
  s = command->parm("NAME");
  if (! s.isEmpty())
  {
    Strip strip;
    strip.verifyText(s);
    symbol->setName(s);
  }

  // verify type
  s = command->parm("TYPE");
  if (! s.isEmpty())
  {
    Strip strip;
    strip.verifyText(s);
    symbol->setType(s);
  }

  symbol->append(bar);
  
  command->setReturnCode("0");

  return 0;
}

void QUOTE_DATABASE::clear ()
{
  QHashIterator<QString, BarData *> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    delete it.value();
  }

  _symbols.clear();
}

void QUOTE_DATABASE::save ()
{
  QuoteDataBase db;
  QHashIterator<QString, BarData *> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    BarData *bd = it.value();
    
    db.transaction();
    if (db.setBars(bd))
    {
      qDebug() << _plugin << "::save: error saving" << bd->key();
      continue;
    }
    db.commit();
  }

  clear();
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  QUOTE_DATABASE *o = new QUOTE_DATABASE;
  return ((Plugin *) o);
}
