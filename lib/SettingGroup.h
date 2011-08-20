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

// *************************************************************************************************
// *************************************************************************************************

#ifndef SETTING_GROUP_HPP
#define SETTING_GROUP_HPP

#include <QStringList>
#include <QHash>

#include "Setting.h"

class SettingGroup
{
  public:
    SettingGroup ();
    ~SettingGroup ();
    void clear ();
    void set (Setting *);
    Setting * get (QString key);
    QStringList keys ();
    bool contains (QString);
    void merge (SettingGroup *);
    QString toString ();
    int fromString (QString);
    void setCommand (QString);
    QString command ();
    void setStepName (QString);
    QString stepName ();
    int parse (QStringList &);

  private:
    QHash<QString, Setting *> _settings;
    QStringList _keys;
    QString _stepName;
    QString _command;
};

#endif
