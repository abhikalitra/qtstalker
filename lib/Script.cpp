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

#include "Script.h"
#include "ChartObjectData.h"

#include <QDebug>
#include <QProcess>

Script::Script (QObject *p) : QObject (p)
{
  clear();
}

Script::~Script ()
{
  clear();
}

void Script::clear ()
{
  qDeleteAll(_data);
  _data.clear();

  _session.clear();
  _name.clear();
  _file.clear();
  _pid = -1;
}

void Script::setData (QString key, Data *d)
{
  Data *sg = _data.value(key);
  if (sg)
    delete sg;

  _data.insert(key, d);
}

Data * Script::data (QString d)
{
  return _data.value(d);
}

void Script::setSession (QString d)
{
  _session = d;
}

QString Script::session ()
{
  return _session;
}

void Script::setName (QString d)
{
  _name = d;
}

QString Script::name ()
{
  return _name;
}

void Script::setFile (QString d)
{
  _file = d;
}

QString & Script::file ()
{
  return _file;
}

int Script::run ()
{
qDebug() << "Script::run" << _session << _file;

  QStringList args;
  args << _session << _file;

  bool ok = QProcess::startDetached("QtStalkerScript", args, QString(), &_pid);
  if (! ok)
  {
    qDebug() << "Script::run: error starting process";
    return 1;
  }

  return 0;
}

int Script::runWait ()
{
qDebug() << "Script::runWait" << _session << _file;

  QStringList args;
  args << _session << _file;

  int rc = QProcess::execute("QtStalkerScript", args);
  if (rc)
  {
    qDebug() << "Script::runWait: error";
    return rc;
  }

  return 0;
}

int Script::kill ()
{
  if (_pid < 0)
    return 1;

  QStringList args;
  args << QString::number(_pid);

  bool ok = QProcess::startDetached("kill", args);
  if (! ok)
  {
    qDebug() << "Script::kill: error starting process";
    return 1;
  }

  return 0;
}

qint64 Script::pid ()
{
  return _pid;
}

int Script::count ()
{
  return _data.count();
}

QList<QString> Script::dataKeys ()
{
  return _data.keys();
}

int Script::nextROID ()
{
  int low = 0;
  QHashIterator<QString, Data *> it(_data);
  while (it.hasNext())
  {
    it.next();
    Data *dg = it.value();
    if (dg->type() != "CHART_OBJECT")
      continue;

    int t = dg->getInteger(ChartObjectData::_ID);
    if (t < low)
      low = t;
  }

  low--;

  return low;
}
