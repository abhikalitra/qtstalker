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

#include "BARS.h"
#include "Curve.h"
#include "Globals.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>

BARS::BARS ()
{
//  _method << "BARS" << "CANDLES" << "Open" << "High" << "Low" << "Close" << "Volume" << "OI" << "CURRENT";
  _method << "BARS" << "CANDLES" << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
}

int BARS::command (Command &command)
{
  // BARS,<METHOD>
  //  0      1

  if (command.count() < 2)
  {
    qDebug() << "BARS::command: invalid parm count" << command.count();
    return 1;
  }

  switch ((Method) _method.indexOf(command.parm(1)))
  {
    case _BARS:
      return getBars(command);
      break;
    case _CANDLES:
      return getCandles(command);
      break;
    case _OPEN:
    case _HIGH:
    case _LOW:
    case _CLOSE:
    case _VOLUME:
    case _OI:
      return getBarField(command);
      break;
//    case _CURRENT:
//      return getCurrentBars(command);
//      break;
    default:
      break;
  }

  return 0;
}

int BARS::getBars (Command &command)
{
  // BARS,<METHOD>,<NAME>
  //  0      1       2

  if (command.count() != 3)
  {
    qDebug() << "BARS::getBars: invalid parm count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "BARS::getBars: no indicator";
    return 1;
  }

  if (g_barData->count() < 1)
  {
    qDebug() << "BARS::getBars: no bars";
    return 1;
  }

  QString name = command.parm(2);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "BARS::getBars: duplicate name" << name;
    return 1;
  }

  line = new Curve(Curve::OHLC);

  int size = g_barData->count();

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *b = g_barData->bar(loop);
    if (! b)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, b->open());
    bar->setData(1, b->high());
    bar->setData(2, b->low());
    bar->setData(3, b->close());
    
    line->setBar(loop, bar);
  }

  line->setLabel(name);
  i->setLine(name, line);

  command.setReturnData("0");

  return 0;
}

int BARS::getCandles (Command &command)
{
  // BARS,<METHOD>,<NAME>
  //  0      1       2

  if (command.count() != 3)
  {
    qDebug() << "BARS::getCandles: invalid parm count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "BARS::getCandles: no indicator";
    return 1;
  }

  if (g_barData->count() < 1)
  {
    qDebug() << "BARS::getCandles: no bars";
    return 1;
  }

  QString name = command.parm(2);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "BARS::getCandles: duplicate name" << name;
    return 1;
  }

  line = new Curve(Curve::Candle);

  int size = g_barData->count();

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *b = g_barData->bar(loop);
    if (! b)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, b->open());
    bar->setData(1, b->high());
    bar->setData(2, b->low());
    bar->setData(3, b->close());

    line->setBar(loop, bar);
  }

  line->setLabel(name);
  i->setLine(name, line);

  command.setReturnData("0");

  return 0;
}

int BARS::getBarField (Command &command)
{
  // BARS,<METHOD>,<NAME>
  //  0      1       2

  if (command.count() != 3)
  {
    qDebug() << "BARS::getBarField: invalid parm count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "BARS::getBarField: no indicator";
    return 1;
  }

  if (g_barData->count() < 1)
  {
    qDebug() << "BARS::getBarField: no bars";
    return 1;
  }

  QString name = command.parm(2);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "BARS::getBarField: duplicate name" << name;
    return 1;
  }

  line = g_barData->input(g_barData->inputType(command.parm(1)));
  if (! line)
  {
    qDebug() << "BARS::getBarField: no input returned";
    return 1;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command.setReturnData("0");

  return 0;
}

/*
int BARS::getCurrentBars (Command &command)
{
  // BARS,<METHOD>
  //  0      1

  MessageClient mc(this);
  QLocalSocket *socket = new QLocalSocket(this);
  if (mc.newConnection(socket, "QtStalkerMessageServer" + g_session))
  {
    qDebug() << "BARS::getCurrentBars:" << socket->errorString();
    delete socket;
    return 1;
  }

  QString message;
  Message mess;
  mess.message(Message::_BARS, message);
  
  mc.sendMessage(socket, message);
  
  mc.receiveMessage(socket, message);
  if (message.isEmpty())
  {
    qDebug() << "BARS::getCurrentBars: empty bars message";
    delete socket;
    return 1;
  }

  delete socket;

  g_barData->clear();
  if (g_barData->setStringSettings(message))
  {
    qDebug() << "BARS::getCurrentBars: bars message fields error";
    return 1;
  }

  QStringList l;
  l << "QUOTE" << "GET" << g_barData->exchange() << g_barData->symbol() << QString::number(g_barData->barLength());
  l << "0" << "0" << QString::number(g_barData->range());
  
  Command tcommand(l.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(tcommand.plugin());
  if (! plug)
  {
    qDebug() << "BARS::getCurrentBars: no plugin";
    return 1;
  }

  plug->command(tcommand);

  delete plug;

  command.setReturnData("0");

qDebug() << "BARS::getCurrentBars:" << g_barData->count();

  return 0;
}
*/

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  BARS *o = new BARS;
  return ((ScriptPlugin *) o);
}
