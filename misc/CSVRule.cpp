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

#include "CSVRule.h"


CSVRule::CSVRule ()
{
  useFileName = FALSE;
}

CSVRule::~CSVRule ()
{
}

void CSVRule::setName (QString &d)
{
  name = d;
}

void CSVRule::getName (QString &d)
{
  d = name;
}

void CSVRule::setFormat (QString &d)
{
  format = d;
}

void CSVRule::getFormat (QString &d)
{
  d = format;
}

void CSVRule::setDelimiter (QString &d)
{
  delimiter = d;
}

void CSVRule::getDelimiter (QString &d)
{
  d = delimiter;
}

void CSVRule::setFileNameSymbol (bool d)
{
  useFileName = d;
}

bool CSVRule::getFileNameSymbol ()
{
  return useFileName;
}

void CSVRule::setDateFormat (QString &d)
{
  dateFormat = d;
}

void CSVRule::getDateFormat (QString &d)
{
  d = dateFormat;
}

void CSVRule::setInterval (QString &d)
{
  interval = d;
}

void CSVRule::getInterval (QString &d)
{
  d = interval;
}

void CSVRule::setFileList (QStringList &d)
{
  fileList = d;
}

void CSVRule::getFileList (QStringList &d)
{
  d = fileList;
}

void CSVRule::getFieldList (QStringList &tl)
{
  tl.clear();
  tl.append("Symbol");
  tl.append("Date");
  tl.append("Open");
  tl.append("High");
  tl.append("Low");
  tl.append("Close");
  tl.append("Volume");
  tl.append("OI");
  tl.append("Ignore");
}

void CSVRule::getBarFormat (QString &d)
{
  QStringList l = format.split(",");
  l.removeAll("Symbol");
  l.removeAll("Date");
  l.removeAll("Ignore");
  d = l.join(",");
}


