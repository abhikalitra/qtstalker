#include "IPCMessage.h"

#include <QStringList>

IPCMessage::IPCMessage (QString session, QString command, QString type, QString file)
{
  init();
  _session = session;
  _command = command;
  _type = type;
  _scriptFile = file;
}

IPCMessage::IPCMessage ()
{
  init();
}

void IPCMessage::init ()
{
  _parmCount = 4;
}

QString IPCMessage::toString ()
{
  QStringList l;
  l << _session << _command << _type << _scriptFile;
  return l.join(",");
}

int IPCMessage::fromString (QString d)
{
  QStringList l = d.split(",");
  if (l.count() != _parmCount)
    return 1;

  int pos = 0;
  _session = l.at(pos++);
  _command = l.at(pos++);
  _type = l.at(pos++);
  _scriptFile = l.at(pos++);

  return 0;
}

void IPCMessage::setSession (QString d)
{
  _session = d;
}

QString IPCMessage::session ()
{
  return _session;
}

void IPCMessage::setCommand (QString d)
{
  _command = d;
}

QString IPCMessage::command ()
{
  return _command;
}

void IPCMessage::setType (QString d)
{
  _type = d;
}

QString IPCMessage::type ()
{
  return _type;
}

void IPCMessage::setScriptFile (QString d)
{
  _scriptFile = d;
}

QString IPCMessage::scriptFile ()
{
  return _scriptFile;
}
