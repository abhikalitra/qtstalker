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

#include "../../../src/QuotePlugin.h"
#include "../../../src/FuturesData.h"
#include <qstring.h>
#include <qnetworkprotocol.h>
#include <qurloperator.h>

class CBOT : public QuotePlugin
{
  Q_OBJECT

  public:
    CBOT ();
    virtual ~CBOT ();
    void update ();
    QString translateFraction (QString);
    QString translateFraction2 (QString);
    QString translateFraction3 (QString);
    void cancelUpdate ();
    void parse();

  public slots:
    void getFile ();
    void opDone (QNetworkOperation *);

  private:
    QUrlOperator *op;
    FuturesData *fd;
    QString data;
    QStringList urlList;
    int symbolLoop;
};

extern "C"
{
  Plugin * create ();
}
