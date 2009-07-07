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

#include "TesterRule.h"

#include <QObject>



TesterRule::TesterRule ()
{
}

void TesterRule::setName (QString &d)
{
  name = d;
}

void TesterRule::getName (QString &d)
{
  d = name;
}

void TesterRule::setEnterLong (Indicator &d)
{
  enterLong = d;
}

void TesterRule::getEnterLong (Indicator &d)
{
  d = enterLong;
}

void TesterRule::setExitLong (Indicator &d)
{
  exitLong = d;
}

void TesterRule::getExitLong (Indicator &d)
{
  d = exitLong;
}

void TesterRule::setEnterShort (Indicator &d)
{
  enterShort = d;
}

void TesterRule::getEnterShort (Indicator &d)
{
  d = enterShort;
}

void TesterRule::setExitShort (Indicator &d)
{
  exitShort = d;
}

void TesterRule::getExitShort (Indicator &d)
{
  d = exitShort;
}

void TesterRule::setCustomLongStop (Indicator &d)
{
  customLongStop = d;
}

void TesterRule::getCustomLongStop (Indicator &d)
{
  d = customLongStop;
}

void TesterRule::setCustomShortStop (Indicator &d)
{
  customShortStop = d;
}

void TesterRule::getCustomShortStop (Indicator &d)
{
  d = customShortStop;
}




bool TesterRule::getMaxLossCheck ()
{
  return maxLossCheck;
}

void TesterRule::setMaxLossCheck (bool d)
{
  maxLossCheck = d;
}

bool TesterRule::getMaxLossLong ()
{
  return maxLossLong;
}

void TesterRule::setMaxLossLong (bool d)
{
  maxLossLong = d;
}

bool TesterRule::getMaxLossShort ()
{
  return maxLossShort;
}

void TesterRule::setMaxLossShort (bool d)
{
  maxLossShort = d;
}

double TesterRule::getMaxLoss ()
{
  return maxLoss;
}

void TesterRule::setMaxLoss (double d)
{
  maxLoss = d;
}

bool TesterRule::getProfitCheck ()
{
  return profitCheck;
}

void TesterRule::setProfitCheck (bool d)
{
  profitCheck = d;
}

bool TesterRule::getProfitLong ()
{
  return profitLong;
}

void TesterRule::setProfitLong (bool d)
{
  profitLong = d;
}

bool TesterRule::getProfitShort ()
{
  return profitShort;
}

void TesterRule::setProfitShort (bool d)
{
  profitShort = d;
}

double TesterRule::getProfit ()
{
  return profit;
}

void TesterRule::setProfit (double d)
{
  profit = d;
}

bool TesterRule::getTrailingCheck ()
{
  return trailingCheck;
}

void TesterRule::setTrailingCheck (bool d)
{
  trailingCheck = d;
}

bool TesterRule::getTrailingLong ()
{
  return trailingLong;
}

void TesterRule::setTrailingLong (bool d)
{
  trailingLong = d;
}

bool TesterRule::getTrailingShort ()
{
  return trailingShort;
}

void TesterRule::setTrailingShort (bool d)
{
  trailingShort = d;
}

double TesterRule::getTrailing ()
{
  return trailing;
}

void TesterRule::setTrailing (double d)
{
  trailing = d;
}

void TesterRule::setCustomLongCheck (bool d)
{
  customLongCheck = d;
}

bool TesterRule::getCustomLongCheck ()
{
  return customLongCheck;
}

void TesterRule::setCustomShortCheck (bool d)
{
  customShortCheck = d;
}

bool TesterRule::getCustomShortCheck ()
{
  return customShortCheck;
}


