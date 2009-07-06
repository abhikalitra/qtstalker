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

#include "ScannerRule.h"


ScannerRule::ScannerRule ()
{
  allSymbols = "0";
}

void ScannerRule::setName (QString &d)
{
  name = d;
}

void ScannerRule::getName (QString &d)
{
  d = name;
}

void ScannerRule::setAllSymbols (QString &d)
{
  allSymbols = d;
}

void ScannerRule::getAllSymbols (QString &d)
{
  d = allSymbols;
}

void ScannerRule::setFileList (QStringList &d)
{
  fileList = d;
}

void ScannerRule::getFileList (QStringList &d)
{
  d = fileList;
}

void ScannerRule::setBarLength (QString &d)
{
  barLength = d;
}

void ScannerRule::getBarLength (QString &d)
{
  d = barLength;
}

void ScannerRule::setBars (QString &d)
{
  bars = d;
}

void ScannerRule::getBars (QString &d)
{
  d = bars;
}

void ScannerRule::setIndicator (Indicator &i)
{
  indicator = i;
}

void ScannerRule::getIndicator (Indicator &i)
{
  i = indicator;
}
