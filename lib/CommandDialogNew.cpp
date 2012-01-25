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

#include "CommandDialogNew.h"
#include "Script.h"
#include "GlobalMutex.h"
#include "GlobalData.h"
#include "TypeThreadMessage.h"

#include <QtDebug>
#include <QUuid>

CommandDialogNew::CommandDialogNew ()
{
  _name = "DIALOG_NEW";

  Data td(TypeData::_LIST);
  Entity::set(QString("LIST"), td);

  td = Data(QString("New Item"));
  Entity::set(QString("NAME"), td);
}

QString CommandDialogNew::run (CommandParse &, void *scr)
{
  Script *script = (Script *) scr;
  
  Data list, name;
  Entity::toData(QString("LIST"), list);
  Entity::toData(QString("NAME"), name);
  
  Entity dialog;
  dialog.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG_NEW));
  dialog.set(QString("LIST"), list);
  dialog.set(QString("NAME"), name);
  
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
  
  dialog.toData(QString("NAME"), name);
  set(QString("NAME"), name);

  _returnCode = "OK";
  return _returnCode;
}