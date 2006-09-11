/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#ifndef QUOTEPLUGIN_HPP
#define QUOTEPLUGIN_HPP

#include <qstring.h>
#include <qobject.h>
#include <qnetworkprotocol.h>
#include <qurloperator.h>
#include <qtimer.h>

class QuotePlugin : public QObject
{
  Q_OBJECT

  signals:
    void done ();
    void message (QString);
    void statusLogMessage (QString);
    void signalGetFileDone (bool);
    void signalCopyFileDone (QString);
    void signalTimeout ();
    void signalProgMessage (int, int);

  public:
    QuotePlugin ();
    virtual ~QuotePlugin ();
    bool setTFloat (QString &, bool);
    void stripJunk (QString &, QString &);
    void createDirectory (QString &, QString &);
    void getPluginName (QString &);
    void getHelpFile (QString &);
    
    virtual void update ();
    virtual void cancelUpdate ();
    virtual void prefDialog (QWidget *);
    
  public slots:
    void getFile (QString &);
    void copyFile (QString &, QString &);
    void getFileDone (QNetworkOperation *);
    void copyFileDone (QNetworkOperation *);
    void dataReady (const QByteArray &, QNetworkOperation *);
    void slotTimeout ();
    
  protected:
    QString file;
    float tfloat;
    bool saveFlag;
    QString pluginName;
    QString helpFile;
    QUrlOperator *op;
    QString data;
    QTimer *timer;
    int errorLoop;
    int retries;
    int timeout;
};

#endif
