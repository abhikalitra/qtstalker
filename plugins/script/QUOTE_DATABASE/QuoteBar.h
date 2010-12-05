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

#ifndef QUOTE_BAR_HPP
#define QUOTE_BAR_HPP

#include <QStringList>
#include <QDateTime>

class QuoteBar
{
  public:
    enum BarLength
    {
      Minute1,
      Minute5,
      Minute10,
      Minute15,
      Minute30,
      Minute60,
      DailyBar,
      WeeklyBar,
      MonthlyBar
    };

    QuoteBar ();
    void setDateRange (QDateTime &, QuoteBar::BarLength);
    void setDate (QDateTime &);
    int setDate (QString);
    QDateTime & startDate ();
    QDateTime & endDate ();
    int setOpen (QString);
    QString & open ();
    int setHigh (QString);
    QString & high ();
    int setLow (QString);
    QString & low ();
    int setClose (QString);
    QString & close ();
    int setVolume (QString);
    QString & volume ();
    int setOI (QString);
    QString & oi ();
    void rangeKey (QString &);
    void string (QString &);
    void lengthList (QStringList &);
    int verify ();
    void setExchange (QString);
    QString & exchange ();
    void setSymbol (QString);
    QString & symbol ();
    void setName (QString);
    QString & name ();
    void setType (QString);
    QString & type ();

  private:
    QString _open;
    QString _high;
    QString _low;
    QString _close;
    QString _volume;
    QString _oi;
    QDateTime _startDate;
    QDateTime _endDate;
    BarLength _length;
    QString _exchange;
    QString _symbol;
    QString _name;
    QString _type;
};

#endif
