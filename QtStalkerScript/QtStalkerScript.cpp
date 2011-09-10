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

#include <QtDebug>
#include <QCoreApplication>
#include <QWidget>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>

QtStalkerScript::QtStalkerScript (QString session, QString command, QString file)
{
  _dummyFlag = 0;
  _stop = 0;

  QFileInfo fi(file);

  _script = new Script(this);
  _script->setSession(session);
  _script->setFile(file);
  _script->setCommand(command);
  _script->setName(fi.baseName());

  QTimer::singleShot(1, this, SLOT(run()));
}

QtStalkerScript::~QtStalkerScript ()
{
}

void QtStalkerScript::run ()
{
  QProcess pro;
  connect(&pro, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(scriptFinished(int, QProcess::ExitStatus)));

  QString s = _script->command() + " " + _script->file();
  pro.start(s);
  if (! pro.waitForStarted())
  {
    message("QtStalkerScript::run", "process not started");
    done();
    return;
  }

  _stop = 0;
  CommandFactory fac;

  while (1)
  {
    if (_stop)
      break;

    pro.waitForReadyRead(-1);
    QByteArray ba = pro.readAllStandardOutput();
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

    Command *com = fac.command(this, tsg.value("COMMAND"));
    if (! com)
    {
      message("QtStalkerScript::run", QString("command not found " + tsg.value("COMMAND")));
      pro.kill();
      done();
      return;
    }

    Data *sg = com->settings();

    // merge settings and verify them
    QList<QString> dkeys = sg->dataKeys();
    QList<QString> nkeys = tsg.keys();
    for (loop = 0; loop < nkeys.count(); loop++)
    {
      if (dkeys.indexOf(nkeys.at(loop)) == -1)
      {
        if (nkeys.at(loop) == "COMMAND")
          continue;

        message("QtStalkerScript::run", QString("invalid setting " + nkeys.at(loop) + " " + tsg.value(nkeys.at(loop))));
        pro.kill();
        done();
        delete sg;
        return;
      }

      sg->set(nkeys.at(loop), QVariant(tsg.value(nkeys.at(loop))));
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

    s = "OK\n";
    if (com->runScript(sg, _script))
      s = "ERROR\n";

    // check if we have any return data to the script
    QString ts = com->returnString();
    if (! ts.isEmpty())
      s = ts + "\n";

    message(com->type(), com->message());

    delete com;
    delete sg;

    // deal with it
    ba.clear();
    ba.append(s);
    pro.write(ba);
    pro.waitForBytesWritten(-1);
  }

  pro.kill();

  done();
}

void QtStalkerScript::done ()
{
  if (_messages.count())
  {
    _messages.prepend("Script: " + _script->file());

    QStringList wt;
    wt << "QtStalkerScript(" + _script->session() + ")" << "Message";

    MessageDialog dialog(0);
    dialog.setWindowTitle(wt.join(" "));
    dialog.setMessage(_messages.join("\n"));
    dialog.exec();
  }

  CommandFactory fac;
  Command *com = fac.command(this, "SCRIPT_DONE");
  if (com)
  {
    Data *sg = com->settings();
    sg->set("SCRIPT", QVariant(_script->file()));
    com->runScript(sg, _script);
    delete com;
    delete sg;
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

void QtStalkerScript::message (QString command, QString mess)
{
  if (mess.isEmpty())
    return;

  _messages << command + ":" + mess;
}

void QtStalkerScript::scriptFinished (int, QProcess::ExitStatus)
{
//  qDebug() << "CommandScript::scriptFinished" << code << status;
  _stop = 1;
}
