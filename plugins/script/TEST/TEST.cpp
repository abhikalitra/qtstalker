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
#include "Strip.h"
#include "TestSignal.h"
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
  _startEquity = 0;
  _enterLongOp = Operator::_EQUAL;
  _exitLongOp = Operator::_EQUAL;
  _enterShortOp = Operator::_EQUAL;
  _exitShortOp = Operator::_EQUAL;
  _buyPrice = 0;
  _sellPrice = 0;
  _closePrice = 0;
  _date = 0;
  _name.clear();
  _symbol.clear();
  _tag.clear();
  _delay = 0;
  _maxLossStop = 0;
  _trailingStop = 0;
  
  qDeleteAll(_trades);
  _trades.clear();
}

int TEST::command (Command *command)
{
  // PARMS:
  // NAME
  // TAG
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
  // DATE_NAME
  // BUY_NAME
  // SELL_NAME
  // CLOSE_NAME
  // VOLUME
  // ENTER_COMM
  // EXIT_COMM
  // EQUITY
  // DELAY
  // MAX_LOSS_STOP
  // TRAILING_STOP

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

  // verify TAG
  _tag = command->parm("TAG");
  if (_tag.isEmpty())
    _tag = _name;

  // verify SYMBOL
  _symbol = command->parm("SYMBOL");
  if (_symbol.isEmpty())
  {
    qDebug() << _plugin << "::command: SYMBOL not found";
    return 1;
  }

  // verify ENTER_LONG / EXIT_LONG
  QString s = command->parm("ENTER_LONG_NAME");
  if (! s.isEmpty())
  {
    if (setRule(QString("ENTER_LONG"), command))
      return 1;

    s = command->parm("EXIT_LONG_NAME");
    if (s.isEmpty())
    {
      qDebug() << _plugin << "::command: missing EXIT_LONG_NAME";
      return 1;
    }
    
    if (setRule(QString("EXIT_LONG"), command))
      return 1;
  }

  // verify ENTER_SHORT / EXIT_SHORT
  s = command->parm("ENTER_SHORT_NAME");
  if (! s.isEmpty())
  {
    if (setRule(QString("ENTER_SHORT"), command))
      return 1;

    s = command->parm("EXIT_SHORT_NAME");
    if (s.isEmpty())
    {
      qDebug() << _plugin << "::command: missing EXIT_SHORT_NAME";
      return 1;
    }

    if (setRule(QString("EXIT_SHORT"), command))
      return 1;
  }

  // verify DATE_NAME
  _date = i->line(command->parm("DATE_NAME"));
  if (! _date)
  {
    qDebug() << _plugin << "::command: DATE_NAME not found" << command->parm("DATE_NAME");
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
    _volume = 0.02;

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
    _startEquity = t;
  }
  else
  {
    _equity = 10000;
    _startEquity = 10000;
  }

  // verify DELAY
  s = command->parm("DELAY");
  if (! s.isEmpty())
  {
    bool ok;
    s.toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid DELAY" << s;
      return 1;
    }

    _delay = 1;
  }
  else
    _delay = 0;

  // verify MAX_LOSS_STOP
  s = command->parm("MAX_LOSS_STOP");
  if (! s.isEmpty())
  {
    bool ok;
    double t = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid MAX_LOSS_STOP" << s;
      return 1;
    }

    if (t < 0)
    {
      qDebug() << _plugin << "::command: invalid MAX_LOSS_STOP value < 0" << t;
      return 1;
    }

    _maxLossStop = t;
  }
  else
    _maxLossStop = 0;

  // verify TRAILING_STOP
  s = command->parm("TRAILING_STOP");
  if (! s.isEmpty())
  {
    bool ok;
    double t = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid TRAILING_STOP" << s;
      return 1;
    }

    if (t < 0)
    {
      qDebug() << _plugin << "::command: invalid TRAILING_STOP value < 0" << t;
      return 1;
    }

    _trailingStop = t;
  }
  else
    _trailingStop = 0;

  // run test
  if (test())
    return 1;

  if (saveSummary())
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
  Curve *input = i->line(command->parm(s));
  if (! input)
  {
    qDebug() << _plugin << "::setRule:" << s << "not found";
    return 1;
  }
  
  // verify operator
  s = key + "OP";
  Operator oper;
  Operator::Type op = oper.stringToOperator(command->parm(s));
  if (op == -1)
  {
    qDebug() << _plugin << "::setRule: invalid" << s;
    return 1;
  }
  
  // verify name2/value
  // first check if its a name
  s = key + "NAME2";
  Curve *input2 = i->line(command->parm(s));
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

int TEST::test ()
{
  int status = _NONE;

  QList<int> keys;
  _closePrice->keys(keys);
  if (! keys.count())
    return 1;

  Operator oper;
  
  int rc = 0;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    int pos = keys.at(loop);
    switch (status)
    {
      case _LONG:
      {
        rc = updateTrade(pos);
        if (rc)
        {
	  if (_delay)
	  {
	    status = _EXIT_DELAY;
	    continue;
	  }
	  
          exitTrade(pos, rc);
          status = _NONE;
          break;
        }
        
        CurveBar *bar = _exitLong->bar(pos);
        if (bar)
        {
          CurveBar *bar2 = _exitLong2->bar(pos);
          if (bar2)
          {
            if (oper.test(bar->data(), _exitLongOp, bar2->data()))
            {
	      if (_delay)
	      {
	        status = _EXIT_DELAY;
		rc = (int) TestSignal::_EXIT_LONG;
	        continue;
	      }

              status = _NONE;
              exitTrade(pos, (int) TestSignal::_EXIT_LONG);
              continue;
            }
          }
        }
        break;
      }
      case _SHORT:
      {
        rc = updateTrade(pos);
        if (rc)
        {
	  if (_delay)
	  {
	    status = _EXIT_DELAY;
	    continue;
	  }

          exitTrade(pos, rc);
          status = _NONE;
        }

        CurveBar *bar = _exitShort->bar(pos);
        if (bar)
        {
          CurveBar *bar2 = _exitShort2->bar(pos);
          if (bar2)
          {
            if (oper.test(bar->data(), _exitShortOp, bar2->data()))
            {
	      if (_delay)
	      {
	        status = _EXIT_DELAY;
		rc = (int) TestSignal::_EXIT_SHORT;
	        continue;
	      }

              status = _NONE;
              exitTrade(pos, (int) TestSignal::_EXIT_SHORT);
              continue;
            }
          }
        }
        break;
      }
      case _LONG_ENTER_DELAY:
      {
        if (enterTrade(_LONG, pos))
          continue;
        status = _LONG;
	break;
      }
      case _SHORT_ENTER_DELAY:
      {
        if (enterTrade(_SHORT, pos))
          continue;
        status = _SHORT;
	break;
      }
      case _EXIT_DELAY:
      {
        if (exitTrade(pos, rc))
	  continue;
        status = _NONE;
	break;
      }
      default: // no trade
      {
        // if we are on the last bar, dont enter any trades
        if (loop == keys.count() - 1)
          continue;
        
        // check if we enter long trade
        if (_enterLong)
        {
          CurveBar *bar = _enterLong->bar(pos);
          if (bar)
          {
            CurveBar *bar2 = _enterLong2->bar(pos);
            if (bar2)
            {
              if (oper.test(bar->data(), _enterLongOp, bar2->data()))
              {
		if (_delay)
		{
		  status = _LONG_ENTER_DELAY;
		  continue;
		}
		
                if (enterTrade(1, pos))
                  continue;
                status = _LONG;
                continue;
              }
            }
          }
        }

        // check if we enter short trade
        if (_enterShort)
        {
          CurveBar *bar = _enterShort->bar(pos);
          if (bar)
          {
            CurveBar *bar2 = _enterShort2->bar(pos);
            if (bar2)
            {
              if (oper.test(bar->data(), _enterShortOp, bar2->data()))
              {
		if (_delay)
		{
		  status = _SHORT_ENTER_DELAY;
		  continue;
		}

                if (enterTrade(-1, pos))
                  continue;
                status = _SHORT;
                continue;
              }
            }
          }
        }
      }
    }
  }

  // check if this is the last bar and close the trade
  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;
  QString s = trade->data("EXIT_DATE");
  if (s.isEmpty())
    exitTrade(loop - 1, (int) TestSignal::_TEST_END);

  return 0;
}

int TEST::enterTrade (int status, int pos)
{
  CurveBar *bar = _buyPrice->bar(pos);
  if (! bar)
    return 1;

  CurveBar *date = _date->bar(pos);
  if (! date)
    return 1;

  int volume = 1;
  if (_volume != 0)
  {
    volume = (_equity * _volume) / bar->data();
    if (! volume)
      return 1;
  }
  double value = volume * bar->data();

  Setting *trade = new Setting;
  _trades.append(trade);
  
  trade->setData("SYMBOL", _symbol);
  trade->setData("NAME", _name);

  if (status == _LONG)
    trade->setData("TYPE", QString("L"));
  else
    trade->setData("TYPE", QString("S"));

  trade->setData("ENTER_DATE", date->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
  trade->setData("ENTER_PRICE", bar->data());
  trade->setData("VOLUME", volume);
  trade->setData("BARS_HELD", 1);
  trade->setData("ENTER_COMM", _enterCommission);
  trade->setData("DRAWDOWN", 0);
  
  _equity -= value;
  _equity -= _enterCommission;

  return 0;
}

int TEST::exitTrade (int pos, int signal)
{
  CurveBar *bar = _sellPrice->bar(pos);
  if (! bar)
    return 1;

  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;
  
  CurveBar *date = _date->bar(pos);
  if (! date)
    return 1;

  trade->setData("EXIT_DATE", date->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
  trade->setData("EXIT_PRICE", bar->data());
  QString s;
  TestSignal ts;
  ts.signalText((TestSignal::Signal) signal, s);
  trade->setData("SIGNAL", s);
  trade->setData("EXIT_COMM", _exitCommission);

  double profit = 0;
  if (trade->data("TYPE") == "L")
    profit = trade->getDouble("VOLUME") * (bar->data() - trade->getDouble("ENTER_PRICE")); // long
  else
    profit = trade->getDouble("VOLUME") * (trade->getDouble("ENTER_PRICE") - bar->data()); // short
  trade->setData("PROFIT", profit);

  _equity += trade->getDouble("VOLUME") * bar->data();
  _equity -= _exitCommission;
  trade->setData("EQUITY", _equity);
  
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
  if (trade->data("TYPE") == "L")
    profit = trade->getDouble("VOLUME") * (bar->data() - trade->getDouble("ENTER_PRICE")); // long
  else
    profit = trade->getDouble("VOLUME") * (trade->getDouble("ENTER_PRICE") - bar->data()); // short

  double t = trade->getDouble("DRAWDOWN");
  if (profit < t)
    trade->setData("DRAWDOWN", profit);

  trade->setData("BARS_HELD", trade->getInt("BARS_HELD") + 1);

  if (maxLossStop(pos))
    return TestSignal::_MAXIMUM_LOSS_STOP;

  if (trailingStop(pos))
    return TestSignal::_TRAILING_STOP;

  return 0;
}

int TEST::maxLossStop (int pos)
{
  if (! _maxLossStop)
    return 0;
  
  CurveBar *bar = _closePrice->bar(pos);
  if (! bar)
    return 0;

  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 0;

  double risk = (trade->getDouble("VOLUME") * trade->getDouble("ENTER_PRICE")) * _maxLossStop;

  double profit = 0;
  if (trade->data("TYPE") == "L")
    profit = trade->getDouble("VOLUME") * (bar->data() - trade->getDouble("ENTER_PRICE")); // long
  else
    profit = trade->getDouble("VOLUME") * (trade->getDouble("ENTER_PRICE") - bar->data()); // short

  if (profit * 1 >= risk)
    return 1;

  return 0;
}

int TEST::trailingStop (int pos)
{
  if (! _trailingStop)
    return 0;

  CurveBar *bar = _closePrice->bar(pos);
  if (! bar)
    return 0;

  Setting *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 0;

  double risk = bar->data() * _trailingStop;
  
  double trail = trade->getDouble("TRAILING_STOP");
  if (trail == 0)
  {
    if (trade->data("TYPE") == "L")
      trail = trade->getDouble("ENTER_PRICE") - (trade->getDouble("ENTER_PRICE") * _trailingStop);
    else
      trail = trade->getDouble("ENTER_PRICE") + (trade->getDouble("ENTER_PRICE") * _trailingStop);
  }

  if (trade->data("TYPE") == "L")
  {
    double stop = bar->data() - risk;

    if (stop > trail)
    {
      trail = stop;
      trade->setData("TRAILING_STOP", trail);
    }
    
    if (stop < trail)
      return 1;
  }
  else
  {
    double stop = bar->data() + risk;

    if (stop < trail)
    {
      trail = stop;
      trade->setData("TRAILING_STOP", trail);
    }

    if (stop > trail)
      return 1;
  }

  return 0;
}

int TEST::saveSummary ()
{
  int win = 0;
  int loss = 0;
  int conLoss = 0;
  int tConLoss = 0;
  double winTotal = 0;
  double lossTotal = 0;
  double profit = 0;
  double drawDown = 0;
  double tDrawDown = 0;
  int maxBars = 0;
  int minBars = 99999999;
  int totalBars = 0;
  double commissions = 0;
  double equity = 0;
  int loop = 0;
  Setting report;
  for (; loop < _trades.count(); loop++)
  {
    Setting *trade = _trades.at(loop);

    if (trade->getDouble("PROFIT") < 0)
    {
      loss++;
      lossTotal += trade->getDouble("PROFIT");

      tConLoss++;
      if (tConLoss > conLoss)
	conLoss = tConLoss;

      tDrawDown += trade->getDouble("PROFIT");
      if (tDrawDown < drawDown)
	drawDown = tDrawDown;
    }
    else
    {
      win++;
      winTotal += trade->getDouble("PROFIT");

      tConLoss = 0;
      tDrawDown = 0;
    }

    profit += trade->getDouble("PROFIT");

    int bars = trade->getInt("BARS_HELD");
    totalBars += bars;
    if (bars < minBars)
      minBars = bars;
    if (bars > maxBars)
      maxBars = bars;

    commissions += trade->getDouble("ENTER_COMM");
    commissions += trade->getDouble("EXIT_COMM");

    equity = trade->getDouble("EQUITY");
  }

  Strip strip;
  QString s;
  double t = 0;
  t = (double) ((equity - _startEquity) / _startEquity) * 100;
  strip.strip(t, 2, s);
  report.setData("EQUITY_GAIN", s);

  report.setData("EQUITY", equity);
  report.setData("TOTAL_PROFIT", profit);

  t = (double) (((double) win / (double) _trades.count()) * 100);
  strip.strip(t, 2, s);
  report.setData("PROFITABLE_TRADES", s);

  report.setData("TOTAL_TRADES", _trades.count());
  report.setData("WIN_TRADES", win);
  report.setData("LOSE_TRADES", loss);
  report.setData("MAX_DRAWDOWN", drawDown);
  report.setData("MAX_LOSS", conLoss);

  t = (double) (winTotal / (lossTotal * -1));
  strip.strip(t, 2, s);
  report.setData("AVG_PROFIT_LOSS", s);

  report.setData("TOTAL_WIN_TRADES", winTotal);
  report.setData("TOTAL_LOSE_TRADES", lossTotal);

  t = (int) totalBars / _trades.count();
  report.setData("AVG_BARS_HELD", t);

  report.setData("MIN_BARS_HELD", minBars);
  report.setData("MAX_BARS_HELD", maxBars);
  report.setData("TOTAL_COMMISSIONS", commissions);

  // save summary
  report.setData("NAME", _name);
  report.setData("SYMBOL", _symbol);
  report.setData("TAG", _tag);

  // keep track of the best performer (highest equity) and save it
  if (report.getDouble("EQUITY") > _bestReport.getDouble("EQUITY"))
  {
    _bestReport = report;

    TestDataBase db;
    db.transaction();
    if (db.saveSummary(report, _trades))
      return 1;
    db.commit();
  }

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
