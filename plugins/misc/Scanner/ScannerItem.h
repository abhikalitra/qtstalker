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

#ifndef SCANNER_ITEM_HPP
#define SCANNER_ITEM_HPP

#include <QStringList>
#include <QMetaType>

#include "Group.h"
#include "Setting.h"

class ScannerItem
{
  public:
    ScannerItem ();
    void setName (QString);
    QString & name ();
    void setGroup (Group &);
    Group & group ();
    void setIndicator (QString);
    QString & indicator ();
    void setBarLength (int);
    int barLength ();
    void setDateRange (int);
    int dateRange ();
    void setGroupName (QString);
    QString & groupName ();
    void setSettings (Setting);
    Setting & settings ();
    void setPlots (QStringList);
    QStringList & plots ();

  private:
    QString _name;
    Group _group;
    Setting _settings;
    QString _indicator;
    int _barLength;
    int _dateRange;
    QString _groupName;
    QStringList _plots;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(ScannerItem)

#endif
