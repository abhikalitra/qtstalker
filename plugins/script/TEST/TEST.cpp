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
  _method << "SET_RULE" << "TEST" << "SET_VOLUME" << "SET_ENTER_COMM" << "SET_EXIT_COMM" << "SET_EQUITY";
  _method << "SET_PRICES" << "RESET" << "SAVE";

  init();
  _buyPrice = 0;
  _sellPrice = 0;
  _closePrice = 0;
}

TEST::~TEST ()
{
  qDeleteAll(_trades);
}

void TEST::init ()
{
  _enterLong = 0;
  _enterLong2 = 0;
  _exitLong = 0;
  _exitLong2 = 0;
  _enterShort = 0;
  _enterShort2 = 0;
  _exitShort = 0;
  _exitShort2 = 0;
  _volume = 0.2;
  _enterCommission = 10;
  _exitCommission = 10;
  _equity = 1000;
  _enterLongOp = Operator::_EQUAL;
  _exitLongOp = Operator::_EQUAL;
  _enterShortOp = Operator::_EQUAL;
  _exitShortOp = Operator::_EQUAL;
  _enterLongValue = 0;
  _exitLongValue = 0;
  _enterShortValue = 0;
  _exitShortValue = 0;
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
    case _SET_PRICES:
      return setPrices(command);
      break;
    case _RESET:
      return reset(command);
      break;
    case _SAVE:
      return save(command);
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

int TEST::setPrices (Command *command)
{
  // PARMS:
  // METHOD (SET_PRICES)
  // BUY
  // SELL
  // CLOSE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::setPrices: no indicator";
    return 1;
  }

  // verify buy prices
  _buyPrice = i->line(command->parm("BUY"));
  if (! _buyPrice)
  {
    qDebug() << _plugin << "::setPrices: BUY not found" << command->parm("BUY");
    return 1;
  }

  // verify sell prices
  _sellPrice = i->line(command->parm("SELL"));
  if (! _sellPrice)
  {
    qDebug() << _plugin << "::setPrices: SELL not found" << command->parm("SELL");
    return 1;
  }

  // verify close prices
  _closePrice = i->line(command->parm("CLOSE"));
  if (! _closePrice)
  {
    qDebug() << _plugin << "::setPrices: CLOSE not found" << command->parm("CLOSE");
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int TEST::reset (Command *command)
{
  // PARMS:
  // METHOD (RESET)

  qDeleteAll(_trades);
  _trades.clear();

  _equity = 1000;
  
  command->setReturnCode("0");

  return 0;
}

int TEST::save (Command *command)
{
  // PARMS:
  // METHOD (SAVE)
  // NAME

  QString name = command->parm("NAME");
  if (name.isEmpty())
  {
    qDebug() << _plugin << "::save: NAME not found" << name;
    return 1;
  }

qDebug() << _plugin << "::save: total trades =" << _trades.count() << _equity;

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
      {
        int rc = updateTrade(pos);
        if (rc)
        {
          exitTrade(pos, rc);
          status = 0;
          break;
        }
        
        CurveBar *bar = _exitLong->bar(keys.at(pos));
        if (bar)
        {
          if (_exitLong2)
          {
            CurveBar *bar2 = _exitLong2->bar(keys.at(pos));
            if (bar2)
            {
              if (oper.test(bar->data(), _exitLongOp, bar2->data()))
              {
                status = 0;
                exitTrade(pos, status);
                continue;
              }
            }
          }
          else
          {
            if (oper.test(bar->data(), _exitLongOp, _exitLongValue))
            {
              status = 0;
              exitTrade(pos, status);
              continue;
            }
          }
        }
        break;
      }
      case -1: // short trade
      {
        int rc = updateTrade(pos);
        if (rc)
        {
          exitTrade(pos, rc);
          status = 0;
        }

        CurveBar *bar = _exitShort->bar(keys.at(pos));
        if (bar)
        {
          if (_exitShort2)
          {
            CurveBar *bar2 = _exitShort2->bar(keys.at(pos));
            if (bar2)
            {
              if (oper.test(bar->data(), _exitShortOp, bar2->data()))
              {
                status = 0;
                exitTrade(pos, status);
                continue;
              }
            }
          }
          else
          {
            if (oper.test(bar->data(), _exitShortOp, _exitShortValue))
            {
              status = 0;
              exitTrade(pos, status);
              continue;
            }
          }
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
                  if (enterTrade(1, pos))
                    continue;
                  status = 1;
                  continue;
                }
              }
            }
            else
            {
              if (oper.test(bar->data(), _enterLongOp, _enterLongValue))
              {
                if (enterTrade(1, pos))
                  continue;
                status = 1;
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
                  if (enterTrade(-1, pos))
                    continue;
                  status = -1;
                  continue;
                }
              }
            }
            else
            {
              if (oper.test(bar->data(), _enterShortOp, _enterShortValue))
              {
                if (enterTrade(-1, pos))
                  continue;
                status = -1;
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
  if (! _buyPrice)
    return 1;
  
  CurveBar *bar = _buyPrice->bar(pos);
  if (! bar)
    return 1;

  int volume = (_equity * _volume) / bar->data();
  if (! volume)
    return 1;
  double value = volume * bar->data();

  Setting *trade = new Setting;
  _trades.append(trade);
  
//  trade->setData("Symbol", _bars->symbol());
  trade->setData("Type", status);
  trade->setData("Enter Date", pos);
  trade->setData("Enter Price", bar->data());
  trade->setData("Price High", bar->data());
  trade->setData("Price Low", bar->data());
  trade->setData("High", bar->data());
  trade->setData("Low", bar->data());
  trade->setData("Volume", volume);
  
  _equity -= value;
  _equity -= _enterCommission;

//  _low = value;
//  _high = value;

  return 0;
}

int TEST::exitTrade (int pos, int signal)
{
  if (! _sellPrice)
    return 1;

  CurveBar *bar = _sellPrice->bar(pos);
  if (! bar)
    return 1;

  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;
  
  trade->setData("Exit Date", pos);
  trade->setData("Exit Price", bar->data());
  trade->setData("Signal", signal);

  double profit = 0;
  if (trade->getInt("Type"))
    profit = trade->getDouble("Volume") * (bar->data() - trade->getDouble("Enter Price")); // long
  else
    profit = trade->getDouble("Volume") * (trade->getDouble("Enter Price") - bar->data()); // short

  _equity += trade->getDouble("Volume") * bar->data();
  _equity -= _exitCommission;
  
  trade->setData("Profit", profit);

qDebug() << trade->data("Type") << trade->data("Enter Date") << trade->data("Enter Price") << trade->data("Volume") << trade->data("Exit Date") << trade->data("Exit Price") << _equity << profit;
  
  return 0;
}

int TEST::updateTrade (int pos)
{
  CurveBar *bar = _closePrice->bar(pos);
  if (! bar)
    return 1;

  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;

  double profit = 0;
  if (trade->getInt("Type"))
    profit = trade->getDouble("Volume") * (bar->data() - trade->getDouble("Enter Price")); // long
  else
    profit = trade->getDouble("Volume") * (trade->getDouble("Enter Price") - bar->data()); // short

  if (profit < trade->getDouble("Low"))
    trade->setData("Low", profit);

  if (profit > trade->getDouble("High"))
    trade->setData("High", profit);

  if (bar->data() > trade->getDouble("Price High"))
    trade->setData("Price High", bar->data());

  if (bar->data() < trade->getDouble("Price Low"))
    trade->setData("Price Low", bar->data());

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
