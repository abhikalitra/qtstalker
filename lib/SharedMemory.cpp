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

#include "SharedMemory.h"

#include <QtDebug>
#include <QBuffer>
#include <QDataStream>

SharedMemory::SharedMemory ()
{
}

int SharedMemory::setData (QSharedMemory &sm, QString d)
{
  if (sm.isAttached())
  {
    if (! sm.detach())
    {
      qDebug() << "SharedMemory::setData:" << sm.errorString();
      return 1;
    }
  }

  QBuffer buffer;
  buffer.open(QBuffer::ReadWrite);
  QDataStream out(&buffer);
  out << d;
  int size = buffer.size();

  if (! sm.create(size))
  {
    qDebug() << "SharedMemory::setData:" << sm.size() << buffer.size() << sm.errorString();
    return 1;
  }

  sm.lock();
  char *to = (char*) sm.data();
  const char *from = buffer.data().data();
  memcpy(to, from, qMin(sm.size(), size));
  sm.unlock();

  return 0;
}

int SharedMemory::data (QSharedMemory &sm, QString &s)
{
  if (! sm.attach())
  {
    qDebug() << "SharedMemory::data:" << sm.key() << sm.size() << sm.errorString();
    return 1;
  }

  s.clear();
  QBuffer buffer;
  QDataStream in(&buffer);
  sm.lock();
  buffer.setData((char*) sm.constData(), sm.size());
  buffer.open(QBuffer::ReadOnly);
  in >> s;
  sm.unlock();
  sm.detach();

  return 0;
}
