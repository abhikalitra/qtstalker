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

#include "CommandDialogScript.h"
#include "Script.h"
#include "GlobalMutex.h"
#include "GlobalData.h"
#include "TypeThreadMessage.h"

#include <QtDebug>
#include <QUuid>

CommandDialogScript::CommandDialogScript ()
{
  _name = "DIALOG_SCRIPT";

  Data td(TypeData::_FILE);
  Entity::set(QString("FILE"), td);

  td = Data(QString("perl"));
  Entity::set(QString("COMMAND"), td);
}

QString CommandDialogScript::run (CommandParse &, void *scr)
{
  Script *script = (Script *) scr;
  
  Data file, command;
  Entity::toData(QString("FILE"), file);
  Entity::toData(QString("COMMAND"), command);
  
  Entity dialog;
  dialog.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG_SCRIPT));
  dialog.set(QString("FILE"), file);
  dialog.set(QString("COMMAND"), command);
  
  QString id = QUuid::createUuid().toString();
  
  // create new mutex
  QMutex *mutex = new QMutex;
  mutex->lock();
  
  g_mutex.lock();
  g_mutexList.insert(id, mutex);
  g_mutex.unlock();
  
  // put data into global area
  g_dataMutex.lock();
  g_dataList.insert(id, dialog);
  g_dataMutex.unlock();

  // emit the message signal
  script->threadMessage(id);

  // pause thread until main app releases the mutex
  // remove the mutex
//qDebug() << "CommandDialog::run: thread paused" << id;  
  mutex->lock();
//qDebug() << "CommandDialog::run: thread resumed" << id;  
  mutex->unlock();
  g_mutex.lock();
  delete mutex;
  g_mutexList.remove(id);
  g_mutex.unlock();
  
  // remove data from global area
  g_dataMutex.lock();
  dialog = g_dataList.value(id);
  g_dataList.remove(id);
  g_dataMutex.unlock();
  
  // check if dialog was cancelled
  if (! dialog.dkeyCount())
    return _returnCode;
  
  dialog.toData(QString("FILE"), file);
  dialog.toData(QString("COMMAND"), command);
  set(QString("FILE"), file);
  set(QString("COMMAND"), command);

  _returnCode = "OK";
  return _returnCode;
}
