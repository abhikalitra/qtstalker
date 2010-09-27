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
    void setExchange (QString);
    QString & exchange ();
    void setSymbol (QString);
    QString & symbol ();
    void setSettings (QString);
    QString & settings ();
    void setIndicator (QString);
    QString & indicator ();
    void setMail (int);
    int mail ();
    void setSound (int);
    int sound ();
    void setPopup (int);
    int popup ();
    void setEnable (QString, int);
    int enable (QString);
    QString enableString ();
    void setEnableString (QString);
    void setOp (QString, int);
    int op (QString);
    QString opString ();
    void setOpString (QString);
    void setValue (QString, double);
    double value (QString);
    QString valueString ();
    void setValueString (QString);
    void setId (int);
    int id ();
    void setStatus (AlertItem::Status);
    AlertItem::Status status ();
    QString statusToString (AlertItem::Status);
    void setBarLength (int);
    int barLength ();
    void setDateRange (int);
    int dateRange ();
    int count ();
    void keys (QStringList &);

  private:
    QString _exchange;
    QString _symbol;
    QString _settings;
    QString _indicator;
    int _mail;
    int _sound;
    int _popup;
    QHash<QString, int> _enable;
    QHash<QString, int> _op;
    QHash<QString, double> _value;
    int _id;
    Status _status;
    QStringList _statusList;
    int _barLength;
    int _dateRange;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(AlertItem)

#endif
