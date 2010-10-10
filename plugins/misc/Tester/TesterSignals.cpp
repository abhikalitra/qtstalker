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

#include "TesterSignals.h"

#include <QDebug>
#include <QObject>

TesterSignals::TesterSignals ()
{
}

void TesterSignals::signalText (TesterSignals::Signal sig, QString &text)
{
  text.clear();

  switch (sig)
  {
    case _None:
      text = QObject::tr("None");
      break;
    case _ExitLong:
      text = QObject::tr("Exit Long");
      break;
    case _ExitShort:
      text = QObject::tr("Exit Short");
      break;
    case _TestEnd:
      text = QObject::tr("Test End");
      break;
    case _MaximumLossStop:
      text = QObject::tr("Maximum Loss");
      break;
    case _ProfitTargetStop:
      text = QObject::tr("Profit Target");
      break;
    case _TrailingStop:
      text = QObject::tr("Trailing");
      break;
    case _BarsStop:
      text = QObject::tr("Bars");
      break;
    default:
      break;
  }
}

