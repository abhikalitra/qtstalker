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

#include "CSVRule.h"

CSVRule::CSVRule ()
{
  _fileSymbol = 0;
  _removeSuffix = 0;
}

void CSVRule::setName (QString d)
{
  _name = d;
}

QString & CSVRule::name ()
{
  return _name;
}

void CSVRule::setExchange (QString d)
{
  _exchange = d;
}

QString & CSVRule::exchange ()
{
  return _exchange;
}

void CSVRule::setType (QString d)
{
  _type = d;
}

QString & CSVRule::type ()
{
  return _type;
}

void CSVRule::setDelimiter (QString d)
{
  _delimiter = d;
}

QString & CSVRule::delimiter ()
{
  return _delimiter;
}

void CSVRule::setFile (QString d)
{
  _file = d;
}

QString & CSVRule::file ()
{
  return _file;
}

void CSVRule::setRule (QString d)
{
  _rule = d;
}

QString & CSVRule::rule ()
{
  return _rule;
}

void CSVRule::setFileSymbol (int d)
{
  _fileSymbol = d;
}

int CSVRule::fileSymbol ()
{
  return _fileSymbol;
}

void CSVRule::setRemoveSuffix (int d)
{
  _removeSuffix = d;
}

int CSVRule::removeSuffix ()
{
  return _removeSuffix;
}

