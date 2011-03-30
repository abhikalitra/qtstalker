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

#ifndef PLUGIN_YAHOO_HISTORY_HPP
#define PLUGIN_YAHOO_HISTORY_HPP

#include <QThread>
#include <QDateTime>
#include <QList>

#include "Setting.h"

class YahooHistory : public QThread
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
//    void signalDone ();
  
  public:
    YahooHistory (QObject *, QList<Setting>);
    void getUrl (QDateTime &, QDateTime &, Setting &, QString &);
    void parse (QByteArray &, Setting &);
    void downloadName (Setting &);

  public slots:
    void stop ();

  protected:
    void run ();

  private:
    int _stopFlag;
    QList<Setting> _symbols;
};

#endif
