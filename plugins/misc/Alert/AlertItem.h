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

#ifndef ALERT_ITEM_HPP
#define ALERT_ITEM_HPP

#include <QStringList>
#include <QHash>
#include <QMetaType>
#include <QDateTime>

#include "Setting.h"

class AlertItem
{
  public:
    enum Status
    {
      _Waiting,
      _Notify,
      _Hit
    };
    
    AlertItem ();
    void setSymbol (QString);
    QString & symbol ();
    void setSettings (Setting);
    Setting & settings ();
    void setIndicator (QString);
    QString & indicator ();
    void setMail (int);
    int mail ();
    void setSound (int);
    int sound ();
    void setPopup (int);
    int popup ();
    void setPlots (QStringList);
    QStringList & plots ();
    void setId (int);
    int id ();
    void setStatus (AlertItem::Status);
    AlertItem::Status status ();
    QString statusToString (AlertItem::Status);
    void setBarLength (int);
    int barLength ();
    void setDateRange (int);
    int dateRange ();
    void setBars (int);
    int bars ();
    void setLastUpdate (QDateTime);
    QDateTime lastUpdate ();
    void setHitDate (QDateTime);
    QDateTime hitDate ();

  private:
    QString _symbol;
    Setting _settings;
    QString _indicator;
    int _mail;
    int _sound;
    int _popup;
    QStringList _plots;
    int _id;
    Status _status;
    QStringList _statusList;
    int _barLength;
    int _dateRange;
    int _bars;
    QDateTime _lastDate;
    QDateTime _hitDate;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(AlertItem)

#endif
