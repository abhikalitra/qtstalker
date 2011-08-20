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
#include "Globals.h"
#include "CommandFactory.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
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
  qDeleteAll(_curves);
  _curves.clear();

  qDeleteAll(_chartObjects);
  _chartObjects.clear();

  qDeleteAll(_settingGroups);
  _settingGroups.clear();

  _runOrder.clear();
  _session.clear();
  _name.clear();
  _file.clear();
  _pid = -1;
}

void Script::setCurve (QString key, Curve *d)
{
  Curve *c = _curves.value(key);
  if (c)
    delete c;

  _curves.insert(key, d);
}

Curve * Script::curve (QString key)
{
  return _curves.value(key);
}

void Script::setCurves (QHash<QString, Curve *> l)
{
  qDeleteAll(_curves);
  _curves.clear();
  _curves = l;
}

QHash<QString, Curve *>  Script::curves ()
{
  return _curves;
}

void Script::setSettingGroup (SettingGroup *d)
{
  SettingGroup *sg = _settingGroups.value(d->stepName());
  if (sg)
    delete sg;

  _runOrder << d->stepName();

  _settingGroups.insert(d->stepName(), d);
}

SettingGroup * Script::settingGroup (QString d)
{
  return _settingGroups.value(d);
}

Setting * Script::setting (QString k)
{
  QStringList l = k.split(":");
  if (l.count() != 2)
    return 0;

  SettingGroup *sg = _settingGroups.value(l.at(0));
  if (! sg)
    return 0;

  return sg->get(l.at(1));
}

int Script::chartObjectCount ()
{
  return _chartObjects.count();
}

void Script::setChartObject (QString key, ChartObject *d)
{
  ChartObject *c = _chartObjects.value(key);
  if (c)
    delete c;

  _chartObjects.insert(key, d);
}

QHash<QString, ChartObject *>  Script::chartObjects ()
{
  return _chartObjects;
}

void Script::setRunOrder (QStringList d)
{
  _runOrder = d;
}

QStringList Script::runOrder ()
{
  return _runOrder;
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

int Script::loadScript ()
{
  QFile f(_file);
  if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Script::load: file error" << _file;
    return 1;
  }

  while (! f.atEnd())
  {
    QString s = f.readLine();
    s = s.trimmed();

    if (s != "{")
      continue;

    // start new command structure
    QStringList tl;
    while (! f.atEnd())
    {
      s = f.readLine();
      s = s.trimmed();
      if (s.isEmpty())
	continue;

      if (s == "}")
        break;

      tl << s;
    }

    SettingGroup tsg;
    if (tsg.parse(tl))
    {
      qDebug() << "Script::load: parse error" << tl;
      return 1;
    }

    if (tsg.stepName().isEmpty())
    {
      qDebug() << "Script::load: step name missing" << tl;
      return 1;
    }

    if (tsg.command().isEmpty())
    {
      qDebug() << "Script::load: command missing" << tl;
      return 1;
    }

    CommandFactory fac;
    Command *com = fac.command(this, tsg.command());
    if (! com)
    {
      qDebug() << "Script::load: no command" << tl;
      return 1;
    }

    SettingGroup *sg = com->settings();
    sg->setStepName(tsg.stepName());

    tl = tsg.keys();
    int loop = 0;
    for (; loop < tl.count(); loop++)
    {
      Setting *set = tsg.get(tl.at(loop));
      Setting *set2 = sg->get(tl.at(loop));
      if (! set2)
	continue;

      if (set2->fromString(set->toString()))
      {
        qDebug() << "Script::load: parse error" << tl.at(loop) << set->toString();
	return 1;
      }
    }

    setSettingGroup(sg);

    delete com;
  }

  return 0;
}

int Script::saveScript ()
{
  QFile f(_file);
  if (! f.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug() << "Script::save: file error" << _file;
    return 1;
  }

  QTextStream out(&f);

  int loop = 0;
  for (; loop < _runOrder.count(); loop++)
  {
    SettingGroup *sg = _settingGroups.value(_runOrder.at(loop));
    if (! sg)
      continue;

    out << "{\n";
    out << "  STEP=" << sg->stepName() << "\n";
    out << "  COMMAND=" << sg->command() << "\n";

    QStringList l = sg->keys();
    int loop2 = 0;
    for (; loop2 < l.count(); loop2++)
    {
      Setting *set = sg->get(l.at(loop2));
      out << "  " << set->key() << "=" << set->toString() << "\n";
    }

    out << "}\n\n";
  }

  return 0;
}

void Script::removeSettingGroup (QString d)
{
  SettingGroup *sg = settingGroup(d);
  if (! sg)
    return;

  _runOrder.removeAll(d);

  delete sg;

  _settingGroups.remove(d);
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
qDebug() << "Script::run" << g_session << _file;

  QStringList args;
  args << g_session << _file;

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
qDebug() << "Script::runWait" << g_session << _file;

  QStringList args;
  args << g_session << _file;

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

void Script::setCurrentStep (QString d)
{
  _currentStep = d;
}

QString Script::currentStep ()
{
  return _currentStep;
}

