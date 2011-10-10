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

#include "TestSignal.h"

#include <QDebug>
#include <QObject>

TestSignal::TestSignal ()
{
}

void TestSignal::signalText (TestSignal::Signal sig, QString &text)
{
  text.clear();

  switch (sig)
  {
    case _NONE:
      text = QObject::tr("None");
      break;
    case _EXIT_LONG:
      text = QObject::tr("Exit Long");
      break;
    case _EXIT_SHORT:
      text = QObject::tr("Exit Short");
      break;
    case _TEST_END:
      text = QObject::tr("Test End");
      break;
    case _MAXIMUM_LOSS_STOP:
      text = QObject::tr("Maximum Loss");
      break;
    case _PROFIT_TARGET_STOP:
      text = QObject::tr("Profit Target");
      break;
    case _TRAILING_STOP:
      text = QObject::tr("Trailing");
      break;
    case _BARS_STOP:
      text = QObject::tr("Bars");
      break;
    default:
      break;
  }
}
