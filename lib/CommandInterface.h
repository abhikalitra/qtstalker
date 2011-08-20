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

#ifndef COMMAND_INTERFACE_HPP
#define COMMAND_INTERFACE_HPP

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtDBus>

class CommandInterface: public QDBusAbstractInterface
{
  Q_OBJECT

  public:
    static inline const char *staticInterfaceName()
    { return "com.qtstalker.message"; }

  public:
    CommandInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);
    ~CommandInterface();

  public Q_SLOTS: // METHODS
  Q_SIGNALS: // SIGNALS
    void action (const QString &nickname, const QString &text);
    void message (const QString &nickname, const QString &text);
};

namespace com {
  namespace qtstalker {
    typedef ::CommandInterface message;
  }
}

#endif
