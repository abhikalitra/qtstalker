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

#include "YahooThread.h"
#include "YahooParseQuote.h"

#include <QDebug>

YahooThread::YahooThread (QObject *p) : QThread (p)
{
  _urlPos = 0;
  _stopFlag = 0;

  _manager = new QNetworkAccessManager(this);
  connect(_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(requestFinished(QNetworkReply *)));
}

void YahooThread::setParms (QList<YahooUrlData> &urls)
{
  _urls = urls;
}

void YahooThread::stop ()
{
  _stopFlag = 1;
}

void YahooThread::run ()
{
  _urlPos = 0;
  _stopFlag = 0;
  YahooUrlData data = _urls.at(_urlPos);

  _manager->get(QNetworkRequest(QUrl(data.url)));
  
  exec();
}

void YahooThread::requestFinished (QNetworkReply *reply)
{
  QByteArray ba = reply->readAll();
  reply->deleteLater();

  YahooUrlData data = _urls.at(_urlPos);

  YahooParseQuote pq;
  connect(&pq, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  
  switch ((UrlType) data.type)
  {
    case UrlTypeHistory:
      pq.history(ba, data);
      break;
    case UrlTypeDetails:
      pq.details(ba, data);
      break;
    default:
      break;
  }

  if (_stopFlag)
  {
    emit signalMessage(QString("\n*** " + tr("Download cancelled") + " ***"));
    quit();
  }

  _urlPos++;
  if (_urlPos == _urls.count())
    quit();
  else
  {
    YahooUrlData data = _urls.at(_urlPos);
    _manager->get(QNetworkRequest(QUrl(data.url)));
  }
}

