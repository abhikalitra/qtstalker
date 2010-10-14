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

#include "ScannerItem.h"

#include <QDebug>

ScannerItem::ScannerItem ()
{
  _barLength = 0;
  _dateRange = 0;
}

void ScannerItem::setName (QString d)
{
  _name = d;
}

QString & ScannerItem::name ()
{
  return _name;
}

void ScannerItem::setGroup (Group &d)
{
  _group = d;
}

Group & ScannerItem::group ()
{
  return _group;
}

void ScannerItem::setIndicator (QString d)
{
  _indicator = d;
}

QString & ScannerItem::indicator ()
{
  return _indicator;
}

void ScannerItem::setBarLength (int d)
{
  _barLength = d;
}

int ScannerItem::barLength ()
{
  return _barLength;
}

void ScannerItem::setDateRange (int d)
{
  _dateRange = d;
}

int ScannerItem::dateRange ()
{
  return _dateRange;
}

void ScannerItem::setGroupName (QString d)
{
  _groupName = d;
}

QString & ScannerItem::groupName ()
{
  return _groupName;
}

void ScannerItem::setSettings (Setting d)
{
  _settings = d;
}

Setting & ScannerItem::settings ()
{
  return _settings;
}

QStringList & ScannerItem::plots ()
{
  return _plots;
}

void ScannerItem::setPlots (QStringList d)
{
  _plots = d;
}

void ScannerItem::setPlotNames (QStringList d)
{
  _plotNames = d;
}

QStringList & ScannerItem::plotNames ()
{
  return _plotNames;
}

