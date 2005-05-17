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

#include "QuotePlugin.h"
#include "Config.h"
#include <qdir.h>

QuotePlugin::QuotePlugin ()
{
  saveFlag = FALSE;
  op = 0;
  errorLoop = 0;
  retries = 1;
  timeout = 15;
  
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

QuotePlugin::~QuotePlugin ()
{
  if (op)
  {
    op->stop();
    delete op;
  }

  delete timer;
}

QString QuotePlugin::stripJunk (QString &d)
{
  QString s = d.stripWhiteSpace();

  while (1)
  {
    int p = s.find('"', 0, TRUE);
    if (p == -1)
      break;
    else
      s.remove(p, 1);
  }

  return s;
}

bool QuotePlugin::setTFloat (QString &d, bool flag)
{
  QString s = d;
  
  while (s.contains("A"))
    s = s.remove(s.find("A", 0, TRUE), 1);
  
  while (s.contains("B"))
    s = s.remove(s.find("B", 0, TRUE), 1);

  while (s.contains("K"))
  {
    s = s.remove(s.find("K", 0, TRUE), 1);
    s.append("000");
  }

  if (flag)
  {
    while (s.contains(","))
      s = s.replace(s.find(",", 0, TRUE), 1, ".");
  }
  else
  {
    while (s.contains(","))
      s = s.remove(s.find(",", 0, TRUE), 1);
  }

  bool ok;
  tfloat = s.toFloat(&ok);
  if (! ok)
    return TRUE;
  else
    return FALSE;
}

QString QuotePlugin::createDirectory (QString &d)
{
  Config config;
  QString path = config.getData(Config::DataPath);
  
  QStringList l = QStringList::split("/", d, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    path.append("/");
    path.append(l[loop]);
    QDir dir(path);
    if (! dir.exists(path, TRUE))
    {
      if (! dir.mkdir(path, TRUE))
        return QString::null;
    }
  }

  return path;
}

QString QuotePlugin::getPluginName ()
{
  return pluginName;
}

QString QuotePlugin::getHelpFile ()
{
  return helpFile;
}

void QuotePlugin::getFile (QString &url)
{
  if (op)
  {
    op->stop();
    delete op;
  }

  data.truncate(0);
  
  timer->start(timeout * 1000, TRUE);
  
  op = new QUrlOperator(url);
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(getFileDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
}

void QuotePlugin::copyFile (QString &url, QString &file)
{
  if (op)
  {
    op->stop();
    delete op;
  }
    
  timer->start(timeout * 1000, TRUE);
  
  QDir dir(file);
  dir.remove(file);

  op = new QUrlOperator();
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(copyFileDone(QNetworkOperation *)));
  op->copy(url, file, FALSE, FALSE);
}

void QuotePlugin::getFileDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
  {
    timer->stop();
    emit signalGetFileDone(FALSE);
    return;
  }

  if (o->state() == QNetworkProtocol::StFailed)
  {
    timer->stop();
    emit signalGetFileDone(TRUE);
  }
}

void QuotePlugin::copyFileDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    timer->stop();
    QString s = QObject::tr("Download error: ") + o->protocolDetail();
    emit signalCopyFileDone(s);
    return;
  }
  
  QDir dir(file);
  if (! dir.exists(file, TRUE))
    return;
  
  timer->stop();

  emit signalCopyFileDone(QString());
}

void QuotePlugin::dataReady (const QByteArray &d, QNetworkOperation *)
{
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
    data.append(d[loop]);
}

void QuotePlugin::slotTimeout ()
{
  timer->stop();
  if (op)
    op->stop();
  emit signalTimeout();
}

//**************************************************************************
//*************************** VIRTUAL OVERRIDES ****************************
//**************************************************************************

void QuotePlugin::update ()
{
}

void QuotePlugin::cancelUpdate ()
{
}

void QuotePlugin::prefDialog (QWidget *)
{
}

