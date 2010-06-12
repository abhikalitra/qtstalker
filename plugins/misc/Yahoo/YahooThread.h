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

#ifndef YAHOO_THREAD_HPP
#define YAHOO_THREAD_HPP

#include <QThread>
#include <QString>
#include <QDateTime>
#include <QtNetwork>
#include <QByteArray>

class YahooThread : public QThread
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalDone ();

  public:
    enum Type
    {
      History,
      Info,
      Exchange
    };

    struct UrlData
    {
      QString url;
      QString ysymbol;
      QString symbol;
      QString exchange;
    };
    
    YahooThread ();
    void downloadHistory (QDateTime sd, QDateTime ed, int adj, QString &ysymbol);
    void downloadInfo (QString &ysymbol);
    void downloadExchange (QString &ysymbol);
    void run ();
    void createHistoryURL ();
    void createInfoURL ();
    void createExchangeURL ();
    void parseHistory (QByteArray &);
    void parseInfo (QByteArray &);
    void parseExchange (QByteArray &);

  public slots:
    void requestFinished (QNetworkReply *);
      
  private:
    QDateTime _sdate;
    QDateTime _edate;
    int _adjustment;
    QNetworkAccessManager *_manager;
    UrlData _data;
    Type _type;
};

#endif
