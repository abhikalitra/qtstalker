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
#include "IPLOTWidget.h"

#include <QtDebug>

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

  // verify color
  s = command->parm("COLOR");
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
  
  // verify Z
  s = command->parm("Z");
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

PluginWidget * IPLOT::dialog (QWidget *p)
{
  return new IPLOTWidget(p);
}

void IPLOT::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=Close";
  l << "COLOR=#FF0000";
  l << "STYLE=Line";
  l << "Z=0";
  l << "PEN=1";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  IPLOT *o = new IPLOT;
  return ((Plugin *) o);
}
