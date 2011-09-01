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

#include "CandleType.h"

#include <QtDebug>

CandleType::CandleType ()
{
  _method << "2CROWS";
  _method << "3BLACKCROWS";
  _method << "3INSIDE";
  _method << "3LINESTRIKE";
  _method << "3OUTSIDE";
  _method << "3STARSINSOUTH";
  _method << "3WHITESOLDIERS";
  _method << "ABANDONEDBABY";
  _method << "ADVANCEBLOCK";
  _method << "BELTHOLD";
  _method << "BREAKAWAY";
  _method << "CLOSINGMARUBOZU";
  _method << "CONCEALBABYSWALL";
  _method << "COUNTERATTACK";
  _method << "DARKCLOUDCOVER";
  _method << "DOJI";
  _method << "DOJISTAR";
  _method << "DRAGONFLYDOJI";
  _method << "ENGULFING";
  _method << "EVENINGDOJISTAR";
  _method << "EVENINGSTAR";
  _method << "GAPSIDESIDEWHITE";
  _method << "GRAVESTONEDOJI";
  _method << "HAMMER";
  _method << "HANGINGMAN";
  _method << "HARAMI";
  _method << "HARAMICROSS";
  _method << "HIGHWAVE";
  _method << "HIKKAKE";
  _method << "HIKKAKEMOD";
  _method << "HOMINGPIGEON";
  _method << "IDENTICAL3CROWS";
  _method << "INNECK";
  _method << "INVERTEDHAMMER";
  _method << "KICKING";
  _method << "KICKINGBYLENGTH";
  _method << "LADDERBOTTOM";
  _method << "LONGLEGGEDDOJI";
  _method << "LONGLINE";
  _method << "MARUBOZU";
  _method << "MATCHINGLOW";
  _method << "MATHOLD";
  _method << "MORNINGDOJISTAR";
  _method << "MORNINGSTAR";
  _method << "ONNECK";
  _method << "PIERCING";
  _method << "RICKSHAWMAN";
  _method << "RISEFALL3METHODS";
  _method << "SEPARATINGLINES";
  _method << "SHOOTINGSTAR";
  _method << "SHORTLINE";
  _method << "SPINNINGTOP";
  _method << "STALLEDPATTERN";
  _method << "STICKSANDWICH";
  _method << "TAKURI";
  _method << "TASUKIGAP";
  _method << "THRUSTING";
  _method << "TRISTAR";
  _method << "UNIQUE3RIVER";
  _method << "UPSIDEGAP2CROWS";
  _method << "XSIDEGAP3METHODS";
}

int CandleType::fromString (QString s)
{
  return _method.indexOf(s);
}

QStringList CandleType::list ()
{
  return _method;
}
