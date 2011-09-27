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
#include "CommandFactory.h"
#include "Message.h"
#include "MessageDialog.h"
#include "SettingString.h"

#include <QtDebug>
#include <QCoreApplication>
#include <QWidget>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>

QtStalkerScript::QtStalkerScript (QString session, QString command, QString file)
{
  QFileInfo fi(file);

  _script = new Script(this);
  _script->setSession(session);
  _script->setFile(file);
  _script->setCommand(command);
  _script->setName(fi.baseName());

  _pro = new QProcess(this);
  connect(_pro, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(scriptFinished(int, QProcess::ExitStatus)));
  connect(_pro, SIGNAL(error(QProcess::ProcessError)), this, SLOT(scriptError(QProcess::ProcessError)));
  connect(_pro, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));

  // we need to create the parent gui thread and keep it running
  // or else after the first dialog is deleted, the qt gui thread
  // will terminate.
  // we create a hidden window and keep it for the remainder of the
  // script.
  _parent = new QWidget(0, Qt::WindowStaysOnBottomHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
  _parent->setFixedSize(0, 0);
  _parent->show();

  QTimer::singleShot(1, this, SLOT(run()));
}

QtStalkerScript::~QtStalkerScript ()
{
  _pro->terminate();
  _pro->waitForFinished(3000);
}

void QtStalkerScript::run ()
{
  QString s = _script->command() + " " + _script->file();
  _pro->start(s);
  if (! _pro->waitForStarted())
  {
    message("QtStalkerScript::run", "process not started");
    scriptFinished(0, QProcess::NormalExit);
    return;
  }

  CommandFactory fac;

  while (_pro->state() == QProcess::Running)
  {
    _pro->waitForReadyRead(-1);
    QByteArray ba = _pro->readAllStandardOutput();
//    qDebug() << ba;
    if (! ba.count())
      continue;

    Message tsg;
    QString s(ba);
    QStringList l = s.split(";");
    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      QStringList tl = l.at(loop).split("=");
      if (tl.count() != 2)
        continue;

      tsg.insert(tl.at(0).trimmed(), tl.at(1).trimmed());
    }

    Command *com = fac.command(_parent, tsg.value("COMMAND"));
    if (! com)
    {
      message("QtStalkerScript::run", QString("command not found " + tsg.value("COMMAND")));
      _pro->kill();
      return;
    }

    com->setWidgetParent(_parent);

    s = "OK\n";
    if (com->runScript(&tsg, _script))
      s = "ERROR\n";

    // check if we have any return data to the script
    QString ts = com->returnString();
    if (! ts.isEmpty())
      s = ts + "\n";

    message(com->type(), com->message());

    delete com;

    // deal with it
    ba.clear();
    ba.append(s);
    _pro->write(ba);
    _pro->waitForBytesWritten(-1);
  }
}

void QtStalkerScript::message (QString command, QString mess)
{
  if (mess.isEmpty())
    return;

  _messages << command + " : " + mess;
}

void QtStalkerScript::scriptFinished (int, QProcess::ExitStatus)
{
//  qDebug() << "CommandScript::scriptFinished" << code << status;

  if (_messages.count())
  {
    QStringList wt;
    wt << "QtStalkerScript(" + _script->session() + ")" << _script->name();

    MessageDialog dialog(_parent);
    dialog.setWindowTitle(wt.join(" "));
    dialog.setMessage(_messages.join("\n"));
    dialog.exec();
  }

  CommandFactory fac;
  Command *com = fac.command(_parent, "SCRIPT_DONE");
  if (com)
  {
    Message sg;
    sg.insert("SCRIPT", _script->file());
    com->runScript(&sg, _script);
    delete com;
  }

/*
  QStringList l;
  l << QDateTime::currentDateTime().toString(Qt::ISODate);
  l << QObject::tr("Script");
  l << _script->name();
  l << QObject::tr("completed");
qDebug() << l.join(" ");
*/

  QCoreApplication::exit(0);
}

void QtStalkerScript::scriptError (QProcess::ProcessError d)
{
  message("QtStalkerScript::scriptError", QString::number(d));
  _pro->kill();
}

void QtStalkerScript::readFromStderr ()
{
  qDebug() << "QtStalkerScript::readFromStderr:" << _script->name() + ": " + _pro->readAllStandardError();
}
