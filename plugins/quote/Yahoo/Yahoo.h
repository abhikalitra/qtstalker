/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include <qnetworkprotocol.h>

class Yahoo : public QuotePlugin
{
  Q_OBJECT

  public:
    Yahoo ();
    virtual ~Yahoo ();
    void update ();
    QString parseDate (QString);
    void parseHistory ();
    void parseQuote ();
    void loadSettings ();
    void saveSettings ();
    void prefDialog (QWidget *);
    
  public slots:
    void opDone (QNetworkOperation *);
    void dataReady (const QByteArray &, QNetworkOperation *);
    void getFile ();

  private:
    QString data;
    QUrlOperator *op;
    QStringList symbolList;
    QStringList urlList;
    int symbolLoop;
    bool adjustment;
    QDateTime sdate;
    QDateTime edate;
    QString method;
};

extern "C"
{
  QuotePlugin * createQuotePlugin ();
}

