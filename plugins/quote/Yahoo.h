/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "QuotePlugin.h"
#include <qstring.h>
#include <qurloperator.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qnetworkprotocol.h>

class Yahoo : public QuotePlugin
{
  Q_OBJECT

  public:
    Yahoo ();
    virtual ~Yahoo ();
    void download ();
    QString parseDate (QString);
    void parse ();

  public slots:
    void getFile ();
    void opDone (QNetworkOperation *);
    void dataReady (const QByteArray &, QNetworkOperation *);

  private:
    QString data;
    QUrlOperator *op;
    QDateTime sdate;
    QDateTime edate;
    QStringList symbols;
    int symbolLoop;
};

extern "C"
{
  Plugin * create ();
}

