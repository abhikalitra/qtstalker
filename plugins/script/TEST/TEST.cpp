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
#include "TestDataBase.h"

#include <QtDebug>

TEST::TEST ()
{
  _plugin = "TEST";
  
  init();
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
  _volume = 0;
  _enterCommission = 0;
  _exitCommission = 0;
  _equity = 0;
  _enterLongOp = Operator::_EQUAL;
  _exitLongOp = Operator::_EQUAL;
  _enterShortOp = Operator::_EQUAL;
  _exitShortOp = Operator::_EQUAL;
  _buyPrice = 0;
  _sellPrice = 0;
  _closePrice = 0;
  _name.clear();
  _version.clear();
  _symbol.clear();
  qDeleteAll(_trades);
}

int TEST::command (Command *command)
{
  // PARMS:
  // NAME
  // VERSION
  // SYMBOL
  // ENTER_LONG_NAME
  // ENTER_LONG_OP
  // ENTER_LONG_NAME2
  // EXIT_LONG_NAME
  // EXIT_LONG_OP
  // EXIT_LONG_NAME2
  // ENTER_SHORT_NAME
  // ENTER_SHORT_OP
  // ENTER_SHORT_NAME2
  // EXIT_SHORT_NAME
  // EXIT_SHORT_OP
  // EXIT_SHORT_NAME2
  // BUY_NAME
  // SELL_NAME
  // CLOSE_NAME
  // VOLUME
  // ENTER_COMM
  // EXIT_COMM
  // EQUITY

  init();

  // verify indicator
  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify NAME
  _name = command->parm("NAME");
  if (_name.isEmpty())
  {
    qDebug() << _plugin << "::command: NAME not found";
    return 1;
  }

  // verify VERSION
  _version = command->parm("VERSION");
  if (_version.isEmpty())
  {
    qDebug() << _plugin << "::command: VERSION not found";
    return 1;
  }

  // verify SYMBOL
  _symbol = command->parm("SYMBOL");
  if (_symbol.isEmpty())
  {
    qDebug() << _plugin << "::command: SYMBOL not found";
    return 1;
  }

  // verify ENTER_LONG / EXIT_LONG
  QString s = command->parm("ENTER_LONG");
  if (! s.isEmpty())
  {
    if (setRule(QString("ENTER_LONG"), command))
      return 1;

    s = command->parm("EXIT_LONG");
    if (s.isEmpty())
    {
      qDebug() << _plugin << "::command: missing EXIT_LONG";
      return 1;
    }
    
    if (setRule(QString("EXIT_LONG"), command))
      return 1;
  }

  // verify ENTER_SHORT / EXIT_SHORT
  s = command->parm("ENTER_SHORT");
  if (! s.isEmpty())
  {
    if (setRule(QString("ENTER_SHORT"), command))
      return 1;

    s = command->parm("EXIT_SHORT");
    if (s.isEmpty())
    {
      qDebug() << _plugin << "::command: missing EXIT_SHORT";
      return 1;
    }

    if (setRule(QString("EXIT_SHORT"), command))
      return 1;
  }

  // verify BUY_NAME
  _buyPrice = i->line(command->parm("BUY_NAME"));
  if (! _buyPrice)
  {
    qDebug() << _plugin << "::command: BUY_NAME not found" << command->parm("BUY_NAME");
    return 1;
  }

  // verify SELL_NAME
  _sellPrice = i->line(command->parm("SELL_NAME"));
  if (! _sellPrice)
  {
    qDebug() << _plugin << "::command: SELL_NAME not found" << command->parm("SELL_NAME");
    return 1;
  }

  // verify CLOSE_NAME
  _closePrice = i->line(command->parm("CLOSE_NAME"));
  if (! _closePrice)
  {
    qDebug() << _plugin << "::command: CLOSE_NAME not found" << command->parm("CLOSE_NAME");
    return 1;
  }

  // verify VOLUME
  s = command->parm("VOLUME");
  if (! s.isEmpty())
  {
    bool ok;
    double t = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid VOLUME" << s;
      return 1;
    }

    if (t < 0 || t > 1)
    {
      qDebug() << _plugin << "::command: invalid VOLUME, value must be between 0 and 1" << t;
      return 1;
    }

    _volume = t;
  }
  else
    _volume = 0.2;

  // verify ENTER_COMM
  s = command->parm("ENTER_COMM");
  if (! s.isEmpty())
  {
    bool ok;
    double t = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid ENTER_COMM" << s;
      return 1;
    }

    if (t < 0)
    {
      qDebug() << _plugin << "::command: invalid ENTER_COMM value < 0" << t;
      return 1;
    }

    _enterCommission = t;
  }
  else
    _enterCommission = 10;

  // verify EXIT_COMM
  s = command->parm("EXIT_COMM");
  if (! s.isEmpty())
  {
    bool ok;
    double t = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid EXIT_COMM" << s;
      return 1;
    }

    if (t < 0)
    {
      qDebug() << _plugin << "::command: invalid EXIT_COMM value < 0" << t;
      return 1;
    }

    _exitCommission = t;
  }
  else
    _exitCommission = 10;

  // verify EQUITY
  s = command->parm("EQUITY");
  if (! s.isEmpty())
  {
    bool ok;
    double t = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid EQUITY" << s;
      return 1;
    }

    if (t < 0)
    {
      qDebug() << _plugin << "::command: invalid EQUITY value < 0" << t;
      return 1;
    }

    _equity = t;
  }
  else
    _equity = 1000;

  if (test())
    return 1;

  if (save())
    return 1;

  command->setReturnCode("0");

  return 0;
}

int TEST::setRule (QString method, Command *command)
{
  // PARMS:
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
  int type = typeList.indexOf(method);
  if (type == -1)
  {
    qDebug() << _plugin << "::setRule: invalid TYPE" << method;
    return 1;
  }

  QString key = typeList.at(type) + "_";

  // verify name
  QString s = key + "NAME";
  Curve *input = i->line(s);
  if (! input)
  {
    qDebug() << _plugin << "::setRule:" << s << "not found";
    return 1;
  }
  
  // verify operator
  s = key + "OP";
  Operator oper;
  Operator::Type op = oper.stringToOperator(s);
  if (op == -1)
  {
    qDebug() << _plugin << "::setRule: invalid" << s;
    return 1;
  }
  
  // verify name2/value
  // first check if its a name
  s = key + "NAME2";
  Curve *input2 = i->line(s);
  if (! input2)
  {
    qDebug() << _plugin << "::setRule:" << s << "not found";
    return 1;
  }

  switch (type)
  {
    case 0: // ENTER_LONG
      _enterLong = input;
      _enterLongOp = op;
      _enterLong2 = input2;
      break;
    case 1: // EXIT_LONG
      _exitLong = input;
      _exitLongOp = op;
      _exitLong2 = input2;
      break;
    case 2: // ENTER_SHORT
      _enterShort = input;
      _enterShortOp = op;
      _enterShort2 = input2;
      break;
    case 3: // EXIT_SHORT
      _exitShort = input;
      _exitShortOp = op;
      _exitShort2 = input2;
      break;
    default:
      break;
  }
  
  return 0;
}

int TEST::save ()
{
  Setting parms;
  parms.setData("NAME", _name);
  parms.setData("VERSION", _version);

  TestDataBase db;
  return db.saveTrades(parms, _trades);
}

int TEST::test ()
{
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
        }

        // check if we enter short trade
        if (_enterShort)
        {
          CurveBar *bar = _enterShort->bar(keys.at(pos));
          if (bar)
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
        }
      }
    }
  }

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
  
  trade->setData("SYMBOL", _symbol);
  trade->setData("TYPE", status);
  trade->setData("ENTER_DATE", pos);
  trade->setData("ENTER_PRICE", bar->data());
  trade->setData("PRICE_HIGH", bar->data());
  trade->setData("PRICE_LOW", bar->data());
  trade->setData("HIGH", bar->data());
  trade->setData("LOW", bar->data());
  trade->setData("VOLUME", volume);
  
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
  
  trade->setData("EXIT_DATE", pos);
  trade->setData("EXIT_PRICE", bar->data());
  trade->setData("SIGNAL", signal);

  double profit = 0;
  if (trade->getInt("TYPE"))
    profit = trade->getDouble("VOLUME") * (bar->data() - trade->getDouble("ENTER_PRICE")); // long
  else
    profit = trade->getDouble("VOLUME") * (trade->getDouble("ENTER_PRICE") - bar->data()); // short

  _equity += trade->getDouble("VOLUME") * bar->data();
  _equity -= _exitCommission;
  
  trade->setData("PROFIT", profit);

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
  if (trade->getInt("TYPE"))
    profit = trade->getDouble("VOLUME") * (bar->data() - trade->getDouble("ENTER_PRICE")); // long
  else
    profit = trade->getDouble("VOLUME") * (trade->getDouble("ENTER_PRICE") - bar->data()); // short

  if (profit < trade->getDouble("LOW"))
    trade->setData("LOW", profit);

  if (profit > trade->getDouble("HIGH"))
    trade->setData("HIGH", profit);

  if (bar->data() > trade->getDouble("PRICE_HIGH"))
    trade->setData("PRICE_HIGH", bar->data());

  if (bar->data() < trade->getDouble("PRICE_LOW"))
    trade->setData("PRICE_LOW", bar->data());

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
