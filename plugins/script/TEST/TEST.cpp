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

#include "TEST.h"
#include "Globals.h"

#include <QtDebug>

TEST::TEST ()
{
  _plugin = "TEST";
  _enterLong = 0;
  _enterLong2 = 0;
  _exitLong = 0;
  _exitLong2 = 0;
  _enterShort = 0;
  _enterShort2 = 0;
  _exitShort = 0;
  _exitShort2 = 0;
  _volume = 0.1;
  _enterCommission = 10;
  _exitCommission = 10;
  _equity = 10000;
  _enterLongOp = Operator::_EQUAL;
  _exitLongOp = Operator::_EQUAL;
  _enterShortOp = Operator::_EQUAL;
  _exitShortOp = Operator::_EQUAL;
  _enterLongValue = 0;
  _exitLongValue = 0;
  _enterShortValue = 0;
  _exitShortValue = 0;
  
  _method << "SET_RULE" << "TEST" << "SET_VOLUME" << "SET_ENTER_COMM" << "SET_EXIT_COMM" << "SET_EQUITY";
}

int TEST::command (Command *command)
{
  // PARMS:
  // METHOD

  switch ((Method) _method.indexOf(command->parm("METHOD")))
  {
    case _SET_RULE:
      return setRule(command);
      break;
    case _TEST:
      return test(command);
      break;
    case _SET_VOLUME:
      return setVolume(command);
      break;
    case _SET_ENTER_COMM:
      return setEnterComm(command);
      break;
    case _SET_EXIT_COMM:
      return setExitComm(command);
      break;
    case _SET_EQUITY:
      return setEquity(command);
      break;
    default:
      break;
  }

  return 0;
}

int TEST::setRule (Command *command)
{
  // PARMS:
  // METHOD (SET_RULE)
  // TYPE
  // NAME
  // OP
  // NAME2

  // verify indicator
  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::setRule: no indicator";
    return 1;
  }

  // verify type
  QStringList typeList;
  typeList << "ENTER_LONG" << "EXIT_LONG" << "ENTER_SHORT" << "EXIT_SHORT";
  int type = typeList.indexOf(command->parm("TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::setRule: invalid TYPE" << command->parm("TYPE");
    return 1;
  }

  // verify name
  Curve *input = i->line(command->parm("NAME"));
  if (! input)
  {
    qDebug() << _plugin << "::setRule: NAME not found" << command->parm("NAME");
    return 1;
  }
  
  // verify operator
  Operator oper;
  Operator::Type op = oper.stringToOperator(command->parm("OP"));
  if (op == -1)
  {
    qDebug() << _plugin << "::setRule: invalid OP" << command->parm("OP");
    return 1;
  }
  
  // verify name2/value
  // first check if its a name
  double value = 0;
  Curve *input2 = i->line(command->parm("NAME2"));
  if (! input2)
  {
    // check if its a value
    bool ok;
    double t = command->parm("NAME2").toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::setRule: invalid NAME2" << command->parm("NAME2");
      return 1;
    }

    value = t;
  }

  switch (type)
  {
    case 0: // ENTER_LONG
      _enterLong = input;
      _enterLongOp = op;
      if (input2)
        _enterLong2 = input2;
      else
        _enterLongValue = value;
      break;
    case 1: // EXIT_LONG
      _exitLong = input;
      _exitLongOp = op;
      if (input2)
        _exitLong2 = input2;
      else
        _exitLongValue = value;
      break;
    case 2: // ENTER_SHORT
      _enterShort = input;
      _enterShortOp = op;
      if (input2)
        _enterShort2 = input2;
      else
        _enterShortValue = value;
      break;
    case 3: // EXIT_SHORT
      _exitShort = input;
      _exitShortOp = op;
      if (input2)
        _exitShort2 = input2;
      else
        _exitShortValue = value;
      break;
    default:
      break;
  }
  
  command->setReturnCode("0");

  return 0;
}

int TEST::setVolume (Command *command)
{
  // PARMS:
  // METHOD (SET_VOLUME)
  // VOLUME

  bool ok;
  double t = command->parm("VOLUME").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::setVolume: invalid VOLUME" << command->parm("VOLUME");
    return 1;
  }

  if (t < 0 || t > 1)
  {
    qDebug() << _plugin << "::setVolume: invalid VOLUME, value must be between 0 and 1" << t;
    return 1;
  }

  _volume = t;

  command->setReturnCode("0");

  return 0;
}

int TEST::setEnterComm (Command *command)
{
  // PARMS:
  // METHOD (SET_ENTER_COMM)
  // COMM

  bool ok;
  double t = command->parm("COMM").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::setEnterComm: invalid COMM" << command->parm("COMM");
    return 1;
  }

  if (t < 0)
  {
    qDebug() << _plugin << "::setEnterComm: invalid enter COMM value < 0" << t;
    return 1;
  }

  _enterCommission = t;

  command->setReturnCode("0");

  return 0;
}

int TEST::setExitComm (Command *command)
{
  // PARMS:
  // METHOD (SET_EXIT_COMM)
  // COMM

  bool ok;
  double t = command->parm("COMM").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::setExitComm: invalid COMM" << command->parm("COMM");
    return 1;
  }

  if (t < 0)
  {
    qDebug() << _plugin << "::setExitComm: invalid exit COMM value < 0" << t;
    return 1;
  }

  _exitCommission = t;

  command->setReturnCode("0");

  return 0;
}

int TEST::setEquity (Command *command)
{
  // PARMS:
  // METHOD (SET_EQUITY)
  // EQUITY

  bool ok;
  double t = command->parm("EQUITY").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::setEquity: invalid EQUITY" << command->parm("EQUITY");
    return 1;
  }

  if (t < 0)
  {
    qDebug() << _plugin << "::setEquity: invalid EQUITY value < 0" << t;
    return 1;
  }

  _equity = t;

  command->setReturnCode("0");

  return 0;
}


int TEST::test (Command *command)
{
  // PARMS:
  // METHOD (TEST)

  int status = 0;

  QList<int> keys;
  if (_enterLong)
    _enterLong->keys(keys);
  if (_enterShort)
    _enterShort->keys(keys);

  if (! keys.count())
    return 1;

  Operator oper;
  
  int pos = keys.at(0);
  for (; pos < keys.count(); pos++)
  {
    switch (status)
    {
      case 1: // long trade
      case -1: // short trade
      {
        int rc = updateTrade(pos);
        if (rc)
        {
          exitTrade(pos, rc);
          status = 0;
        }
        break;
      }
      default:  // no trade
      {
        // check if we enter long trade
        if (_enterLong)
        {
          CurveBar *bar = _enterLong->bar(keys.at(pos));
          if (bar)
          {
            if (_enterLong2)
            {
              CurveBar *bar2 = _enterLong2->bar(keys.at(pos));
              if (bar2)
              {
                if (oper.test(bar->data(), _enterLongOp, bar2->data()))
                {
                  status = 1;
                  enterTrade(status, pos);
                  continue;
                }
              }
            }
            else
            {
              if (oper.test(bar->data(), _enterLongOp, _enterLongValue))
              {
                status = 1;
                enterTrade(status, pos);
                continue;
              }
            }
          }
        }

        // check if we enter short trade
        if (_enterShort)
        {
          CurveBar *bar = _enterShort->bar(keys.at(pos));
          if (bar)
          {
            if (_enterShort2)
            {
              CurveBar *bar2 = _enterShort2->bar(keys.at(pos));
              if (bar2)
              {
                if (oper.test(bar->data(), _enterShortOp, bar2->data()))
                {
                  status = -1;
                  enterTrade(status, pos);
                  continue;
                }
              }
            }
            else
            {
              if (oper.test(bar->data(), _enterShortOp, _enterShortValue))
              {
                status = -1;
                enterTrade(status, pos);
                continue;
              }
            }
          }
        }
      }
    }
  }

  command->setReturnCode("0");

  return 0;
}

int TEST::enterTrade (int status, int pos)
{
  Bar *bar = _bars->bar(pos);
  if (! bar)
    return 1;

  Setting *trade = new Setting;
  _trades.append(trade);
  
  trade->setData("Symbol", _bars->symbol());
  trade->setData("Type", status);
  trade->setData("Enter Date", bar->date());
  trade->setData("Price", bar->close());
  trade->setData("Price High", bar->close());
  trade->setData("Price Low", bar->close());
  trade->setData("High", bar->close());
  trade->setData("Low", bar->close());

  int volume = _equity * _volume;
  trade->setData("Volume", volume);
  
  double value = volume * bar->close();
  _equity -= value;
  _equity -= _enterCommission;

//  _low = value;
//  _high = value;

  return 0;
}

int TEST::exitTrade (int pos, int signal)
{
  Bar *bar = _bars->bar(pos);
  if (! bar)
    return 1;

  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;
  
  trade->setData("Exit Date", bar->date());
  trade->setData("Exit Price", bar->close());
  trade->setData("Signal", signal);

  double profit = 0;
  if (! trade->getInt("Type"))
    profit = trade->getDouble("Volume") * (trade->getDouble("Exit Price") - trade->getDouble("Enter Price"));
  else
    profit = trade->getDouble("Volume") * (trade->getDouble("Enter Price") - trade->getDouble("Exit Price"));

  _equity += trade->getDouble("Volume") * trade->getDouble("Enter Price");
  _equity += profit;
  _equity -= _exitCommission;

  return 0;
}

int TEST::updateTrade (int pos)
{
  Bar *bar = _bars->bar(pos);
  if (! bar)
    return 1;

  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;

  double profit = 0;
  if (! trade->getInt("Type"))
    profit = trade->getDouble("Volume") * (bar->close() - trade->getDouble("Enter Price"));
  else
    profit = trade->getDouble("Volume") * (trade->getDouble("Enter Price") - bar->close());

  if (profit < trade->getDouble("Low"))
    trade->setData("Low", profit);

  if (profit > trade->getDouble("High"))
    trade->setData("High", profit);

  if (bar->close() > trade->getDouble("Price High"))
    trade->setData("Price High", bar->close());

  if (bar->close() < trade->getDouble("Price Low"))
    trade->setData("Price Low", bar->close());

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  TEST *o = new TEST;
  return ((ScriptPlugin *) o);
}
