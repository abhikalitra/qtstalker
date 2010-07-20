/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#ifndef QS_DETAIL_HPP
#define QS_DETAIL_HPP

#include <QStringList>

class QSDetail
{
  public:
    enum Type
    {
      _Name,
      _Sector,
      _FuturesCode,
      _FuturesExpMonth,
      _FuturesExpYear,
      _T1,
      _T2,
      _T3,
      _T4,
      _T5,
      _T6,
      _T7,
      _T8,
      _T9,
      _T10,
      _T11,
      _T12,
      _T13,
      _T14,
      _T15,
      _D1,
      _D2,
      _D3,
      _D4,
      _D5,
      _D6,
      _D7,
      _D8,
      _D9,
      _D10,
      _D11,
      _D12,
      _D13,
      _D14,
      _D15
    };
    
    QSDetail ();
    void setTable (QString &);
    QString & table ();
    int addKey (QString &);
    void addData (QString &);
    void string (QString &);
    void keyString (QString &);
    int keyCount ();
    QStringList & keyList ();
    QStringList & dataList ();
    int isNumber (QString &);

  private:
    QString _table;
    QStringList _typeList;
    QStringList _keys;
    QStringList _data;
};

#endif
