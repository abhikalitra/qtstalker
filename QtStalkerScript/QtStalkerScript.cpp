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

#include "QtStalkerScript.h"
#include "Globals.h"
#include "CommandFactory.h"

#include <QtDebug>
#include <QCoreApplication>
#include <QStringList>
#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QFileInfo>

QtStalkerScript::QtStalkerScript (QString session, QString file)
{
  QFileInfo fi(file);

  _dummyFlag = 0;
  _script = new Script(this);
  _script->setSession(session);
  _script->setFile(file);
  _script->setName(fi.baseName());

  QTimer::singleShot(1, this, SLOT(run()));
}

QtStalkerScript::~QtStalkerScript ()
{
}

void QtStalkerScript::run ()
{
  if (_script->loadScript())
  {
    qDebug() << "QtStalkerScript::run: script file error";
    done();
    return;
  }

  QStringList runOrder = _script->runOrder();

  CommandFactory fac;
  int loop = 0;
  for (; loop < runOrder.count(); loop++)
  {
    SettingGroup *sg = _script->settingGroup(runOrder.at(loop));
    if (! sg)
    {
      qDebug() << "QtStalkerScript::run: settingGroup missing" << runOrder.at(loop);
      done();
      return;
    }

    Command *com = fac.command(this, sg->command());
    if (! com)
    {
      qDebug() << "QtStalkerScript::run: command missing" << sg->command();
      delete com;
      done();
      return;
    }

    if (com->isDialog())
    {
      // we need to create the parent gui thread and keep it running
      // or else after the first dialog is deleted, the qt gui thread
      // will terminate.
      // we create a hidden window and keep it for the remainder of the
      // script.
      if (! _dummyFlag)
      {
        QWidget *w = new QWidget(0,
                                 Qt::WindowStaysOnBottomHint
                                 | Qt::FramelessWindowHint
                                 | Qt::X11BypassWindowManagerHint);
        w->setFixedSize(0, 0);
        w->show();
        _dummyFlag++;
      }
    }

    _script->setCurrentStep(runOrder.at(loop));

    if (com->runScript(_script))
    {
      qDebug() << "QtStalkerScript::run: command error" << runOrder.at(loop);
      delete com;
      done();
      return;
    }

    delete com;
  }

  done();
}

void QtStalkerScript::done ()
{
  // setup exit script
  Script tscript(this);
  tscript.setSession(_script->session());
  tscript.setFile(_script->file());

  CommandFactory fac;
  Command *com = fac.command(this, "SCRIPT_DONE");
  if (com)
  {
    SettingGroup *sg = com->settings();
    sg->setStepName("XXX123");
    Setting *v = sg->get("SCRIPT");
    v->setString(_script->file());

    tscript.setSettingGroup(sg);
  }

  tscript.setCurrentStep("XXX123");

  com->runScript(&tscript);
  delete com;

  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << QObject::tr("Script");
  l << _script->name();
  l << QObject::tr("completed");
qDebug() << l.join(" ");

  QCoreApplication::exit(0);
}
