/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include <QtGui>

#include "Script.h"
#include "PluginFactory.h"
#include "Global.h"


DialogUser *
Script::dialog (QWidget *p, Entity *settings)
{
  DialogUser *dialog = new DialogUser(p);
  dialog->setGUI(settings);
  return dialog;
}

int
Script::run (Entity *settings, QList<Curve *> &output, QList<Marker *> &markers)
{
  if (! g_symbol)
    return 0;
  
  Setting *command = settings->get(QString("command"));
  if (! command)
    return 0;
  
  Setting *file = settings->get(QString("file"));
  if (! file)
    return 0;
  
  Setting *type = settings->get(QString("scriptType"));
  if (! type)
    return 0;
  
  if (type->toString() == tr("External"))
  {
    int rc = runExternal(command->toString(), file->toString(), output, markers);
    qDeleteAll(_settings);
    return rc;
  }
  
//  else
//  {
//    if (type->toString() == tr("Internal"))
//      return runInternal(command->toString(), file->toString(), output);
//  }

  return 0;
}

int
Script::runExternal (QString com, QString file, QList<Curve *> &output, QList<Marker *> &markers)
{
  _killFlag = FALSE;
  int rc = _ERROR;
  
  QDir dir;
  if (! dir.exists(file))
  {
    qDebug() << "Script::runExternal: invalid script file" << file;
    return 0;
  }
  
  QProcess proc(0);

  QString command = com + " " + file;

  // start the _process
  proc.start(command, QIODevice::ReadWrite);

  // make sure _process starts error free
  if (! proc.waitForStarted(1000))
  {
    qDebug() << "Script::runExternal: error timed out" << file;
    proc.kill();
    done(_TIMEOUT, file);
    return 0;
  }

//qDebug() << "Script::runExternal: started" << command;
  
  while (proc.state() == QProcess::Running)
  {
    if (_killFlag)
    {
      qDebug() << "Script::runExternal: script terminated";
      rc = _CANCEL;
      break;
    }

    // wait until we have some input from _process
    proc.waitForReadyRead(-1);
    QByteArray ba = proc.readAllStandardOutput();
    qDebug() << file << ba;

    // check for end of script
    QString s(ba);
    s = s.trimmed();
    if (! s.length())
    {
      rc = _OK;
      break;
    }

    // parse command
    CommandParse cp;
    if (! cp.parse(s))
    {
      qDebug() << "Script::runExternal: parse error" << s;
      break;
    }
      
    QString rs = QString("ERROR");

    switch ((CommandParse::Verb) cp.verb())
    {
      case CommandParse::_CREATE:
        rc = verbNew(cp, rs);
        break;
      case CommandParse::_GET:
        rc = verbGet(cp, rs);
        break;
      case CommandParse::_SET:
        rc = verbSet(cp, rs);
        break;
      case CommandParse::_RUN:
        rc = verbRun(cp, output, markers, rs);
        break;
      default:
        break;
    }

    if (! rc)
    {
      qDebug() << "Script::runExternal: error";
      rc = _ERROR;
      break;
    }
    
    QByteArray rba;
    rba.append(rs + "\n");
    proc.write(rba);
    proc.waitForBytesWritten(-1);
  }

  proc.kill();
  
  done(rc, file);
  
  return rc;
}

int
Script::verbNew (CommandParse &cp, QString &rs)
{
  PluginFactory fac;
  PluginScript *plug = fac.loadScript(cp.object());
  if (! plug)
  {
    qDebug() << "Script::verbNew: invalid object" << cp.object();
    return 0;
  }

  Entity *e = plug->settings();
  _settings.insert(cp.value(), e);

  rs = "OK";
  
  return 1;
}

int
Script::verbRun (CommandParse &cp, QList<Curve *> &output, QList<Marker *> &markers, QString &rs)
{
  Entity *e = _settings.value(cp.object());
  if (! e)
  {
    qDebug() << "Script::verbRun: invalid object" << cp.object();
    return 0;
  }

  Setting *pn = e->get(QString("plugin"));
  if (! pn)
  {
    qDebug() << "Script::verbRun: missing plugin";
    return 0;
  }
    
  PluginFactory fac;
  PluginScript *plug = fac.loadScript(pn->toString());
  if (! plug)
  {
    qDebug() << "Script::verbRun: invalid plugin" << pn->toString();
    return 0;
  }

  if (! plug->run(e, output, markers))
    return 0;
  
  rs = "OK";
  
  return 1;
}

int
Script::verbGet (CommandParse &cp, QString &rs)
{
  Entity *e = _settings.value(cp.object());
  if (! e)
  {
    qDebug() << "Script::verbGet: invalid object" << cp.object();
    return 0;
  }

  QString prop = cp.prop(cp.props() - 1);
  Setting *pn = e->get(prop);
  if (! pn)
  {
    qDebug() << "Script::verbGet: invalid property" << prop;
    return 0;
  }

  rs = pn->toString();
  
  return 1;
}

int
Script::verbSet (CommandParse &cp, QString &rs)
{
  Entity *e = _settings.value(cp.object());
  if (! e)
  {
    qDebug() << "Script::verbSet: invalid object" << cp.object();
    return 0;
  }

  QString prop = cp.prop(cp.props() - 1);
  Setting *pn = e->get(prop);
  if (! pn)
  {
    qDebug() << "Script::verbSet: invalid property" << prop;
    return 0;
  }

  if (! pn->fromString(cp.value()))
  {
    qDebug() << "Script::verbSet: invalid value" << cp.value();
    return 0;
  }

  rs = "OK";
  
  return 1;
}

/*
void
ScriptInternal::runInternal (QString com, QString file, QList<Curve *> &output)
{
  int rc = _OK;
  
  ScriptInternalParse qts;
  if (qts.load(_file))
  {
    qDebug() << "ScriptInternal::run: file error" << _file;
    Script::done(_ERROR);
    return;
  }

  int loop = 0;
  for (; loop < qts.commands(); loop++)
  {
//qDebug() << "ScriptInternal::run:" << lines.at(loop);

    QString command;
    qts.command(loop, command);
    
    // parse command
    CommandParse cp;
    if (cp.parse(command, this))
    {
      qDebug() << "ScriptInternal::run: parse error" << command;
      rc = _ERROR;
      break;
    }
    
//qDebug() << "ScriptInternal::run:" << command;

    QString rc = QString("ERROR");
    Plugin *plug = NULL;
    
    if (cp.prop(0) == QString("new"))
    {
      if (! cp.values())
      {
        qDebug() << "ScriptInternal::run: value missing for new command" << command;
        rc = _ERROR;
        break;
      }
      
      PluginFactory fac;
      plug = fac.plugin(cp.object());
      if (! plug)
      {
        qDebug() << "ScriptInternal::run: invalid object" << command;
        rc = _ERROR;
        break;
      }

      setPlugin(cp.value(0), plug);
      
      continue;
    }
    else
    {
      plug = plugin(cp.object());
      if (! plug)
      {
        qDebug() << "ScriptInternal::run: invalid object" << command;
        rc = _ERROR;
        break;
      }
    }

    rc = plug->script(cp, this);
    
    if (rc.contains("ERROR"))
    {
      qDebug() << "ScriptInternal::run: command error" << command;
      rc = _ERROR;
      break;
    }
  }
  
  Script::done(rc);
}
*/

void
Script::done (int rc, QString file)
{
  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << file;

  switch ((ReturnCode) rc)
  {
    case _CANCEL:
      l << tr("cancelled");
      break;
    case _TIMEOUT:
      l << tr("timeout");
      break;
    case _ERROR:
      l << tr("error");
      break;
    default:
      l << tr("completed");
      break;
  }

  qDebug() << l.join(" ");
  
//  emit signalDone(_id);
}

Entity *
Script::settings ()
{
  Entity *command = new Entity;
  int order = 0;
  
  // plugin
  Setting *set = new Setting;
  set->setString(QString("Script"), QString(), -1);
  command->set(QString("plugin"), set);
  
  // entity type
  set = new Setting;
  set->setString(QString("indicator"), QString(), -1);
  command->set(QString("type"), set);
  
  // Script page

  QString tab(tr("Settings"));
  
  // command
  set = new Setting;
  set->setString(QString("perl"), tr("Command"), order++);
  set->setTab(tab);
  command->set(QString("command"), set);
  
  // file
  set = new Setting;
  set->setFile(QStringList(), tr("File"), order++);
  set->setTab(tab);
  command->set(QString("file"), set);
  
  // type
  QStringList l;
  l << tr("External");
  
  set = new Setting;
  set->setCombo(l, l.at(0), tr("Type"), order++);
  set->setTab(tab);
  command->set(QString("scriptType"), set);

  return command;
}


// do not remove
Q_EXPORT_PLUGIN2(script, Script);
