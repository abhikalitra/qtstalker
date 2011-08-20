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

#ifndef COMMAND_APAPTOR_HPP
#define COMMAND_ADAPTOR_HPP

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

class CommandAdaptor: public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "com.qtstalker.message")
  Q_CLASSINFO("D-Bus Introspection", ""
  "  <interface name=\"com.qtstalker.message\" >\n"
  "    <signal name=\"message\" >\n"
  "      <arg direction=\"out\" type=\"s\" name=\"nickname\" />\n"
  "      <arg direction=\"out\" type=\"s\" name=\"text\" />\n"
  "    </signal>\n"
  "    <signal name=\"action\" >\n"
  "      <arg direction=\"out\" type=\"s\" name=\"nickname\" />\n"
  "      <arg direction=\"out\" type=\"s\" name=\"text\" />\n"
  "    </signal>\n"
  "  </interface>\n"
         "")

  public:
     CommandAdaptor (QObject *parent);
     virtual ~CommandAdaptor();

  public: // PROPERTIES
  public Q_SLOTS: // METHODS
  Q_SIGNALS: // SIGNALS
    void action (const QString &nickname, const QString &text);
    void message (const QString &nickname, const QString &text);
};

#endif
