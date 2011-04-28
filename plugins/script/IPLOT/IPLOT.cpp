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

#include "IPLOT.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

IPLOT::IPLOT ()
{
  _plugin = "IPLOT";
  _type = "INDICATOR";
}

int IPLOT::command (Command *command)
{
  // PARMS:
  // NAME
  // STYLE
  // USE_STYLE
  // USE_COLOR
  // COLOR
  // Z
  // PEN

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify name
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::command: NAME not found" << name;
    return 1;
  }

  // verify style
  int flag = command->parm("USE_STYLE").toInt();
  if (flag)
  {
    QString s = command->parm("STYLE");
    if (! s.isEmpty())
    {
      QStringList l = line->list();
      int style = l.indexOf(s);
      if (style == -1)
      {
        qDebug() << _plugin << "::command: invalid STYLE" << s;
        return 1;
      }
      line->setType(command->parm("STYLE"));
    }
  }

  // verify color
  flag = command->parm("USE_COLOR").toInt();
  if (flag)
  {
    QString s = command->parm("COLOR");
    if (! s.isEmpty())
    {
      QColor color(s);
      if (! color.isValid())
      {
        qDebug() << _plugin << "::command: invalid COLOR" << s;
        return 1;
      }
      line->setAllColor(color);
    }
  }
  
  // verify Z
  QString s = command->parm("Z");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid Z value" << s;
    return 1;
  }
  bool ok;
  int z = s.toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid Z value" << s;
    return 1;
  }
  if (z < -1)
  {
    qDebug() << _plugin << "::command: invalid z value must be -1 or greater" << z;
    return 1;
  }
  line->setZ(z);

  // verify PEN
  s = command->parm("PEN");
  if (s.isEmpty())
    s = "1";
  int pen = s.toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PEN value" << s;
    return 1;
  }
  if (pen < 1)
  {
    qDebug() << _plugin << "::command: invalid pen value must be 1 or greater" << pen;
    return 1;
  }
  line->setPen(pen);

  command->setReturnCode("0");

  return 0;
}

void IPLOT::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME" << "USE_COLOR" << "COLOR" << "USE_STYLE" << "STYLE" << "Z" << "PEN";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME", QString("Close"));
  set->setData("NAME:TYPE", QString("TEXT"));
  set->setData("USE_COLOR", 1);
  set->setData("USE_COLOR:TYPE", QString("CHECK"));
  set->setData("COLOR", QString("#FF0000"));
  set->setData("COLOR:TYPE", QString("COLOR"));
  set->setData("USE_STYLE", 1);
  set->setData("USE_STYLE:TYPE", QString("CHECK"));
  set->setData("STYLE", QString("Line"));
  set->setData("STYLE:TYPE", QString("LIST"));
  QSettings settings(g_globalSettings);
  set->setData("STYLE:LIST", settings.value("curve_plugins").toStringList().join(","));
  set->setData("Z", 0);
  set->setData("Z:TYPE", QString("INTEGER"));
  set->setData("PEN", 1);
  set->setData("PEN:TYPE", QString("INTEGER"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  IPLOT *o = new IPLOT;
  return ((Plugin *) o);
}
