/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#ifndef SETTING_HPP
#define SETTING_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <qdict.h>

typedef struct
{
  QString data;
  QString key;
  QStringList list;
  int type;

} SettingItem;

class Setting
{
  public:
    enum Type
    {
      None,
      Integer,
      Float,
      Bool,
      Text,
      Color,
      Date,
      Symbol,
      FileList,
      Composite, // not in use
      List,
      MAType,
      InputField,
      LineType
    };

    Setting ();
    ~Setting ();
    void set (QString, QString, Setting::Type);
    QString getData (QString);
    float getFloat (QString);
    int getInt (QString);
    void setData (QString, QString);
    Setting::Type getType (QString);
    QStringList getList (QString);
    void setList (QString, QStringList);
    QStringList getKeyList ();
    QStringList getStringList ();
    void remove (QString);
    QString getString ();
    SettingItem * getItem (QString);
    void parse (QString);
    void parse (QStringList);
    void clear ();
    int count ();

  protected:
    QDict<SettingItem> dict;
};

#endif
