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

#include "VBP.h"
#include "Curve.h"
#include "Globals.h"
#include "VBPWidget.h"
#include "InputType.h"

#include <QtDebug>
#include <QHash>

VBP::VBP ()
{
  _type = "INDICATOR";
  _plugin = "VBP";
}

int VBP::command (Command *command)
{
  // PARMS:
  // INPUT_CLOSE
  // INPUT_VOLUME
  // NAME
  // COLOR_UP
  // COLOR_DOWN

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  Curve *ivol = i->line(command->parm("INPUT_VOLUME"));
  if (! ivol)
  {
    qDebug() << _plugin << "::command: invalid INPUT_VOLUME" << command->parm("INPUT_VOLUME");
    return 1;
  }

  QColor upColor("#00FF00");
  QString s = command->parm("COLOR_UP");
  if (! s.isEmpty())
  {
    upColor.setNamedColor(s);
    if (! upColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_UP" << command->parm("COLOR_UP");
      return 1;
    }
  }

  QColor downColor("#FF0000");
  s = command->parm("COLOR_DOWN");
  if (! s.isEmpty())
  {
    downColor.setNamedColor(s);
    if (! downColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_DOWN" << command->parm("COLOR_DOWN");
      return 1;
    }
  }

  QList<Curve *> list;
  list << iclose << ivol;
  line = getVBP(list, upColor, downColor);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * VBP::getVBP (QList<Curve *> &list, QColor upColor, QColor downColor)
{
  if (list.count() != 2)
    return 0;
  
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;
  
  // get close high/low
  int loop = 0;
  double high = 0;
  double low = 0;
  Curve *close = list.at(0);
  Curve *vol = list.at(1);
  if (close->highLow(high, low))
    return 0;

  // get the price block range
  double range = (high - low) / 12;
  QHash<int, Setting *> hash;
  double bottom = low;
  double top = bottom + range;
  loop = 0;
  for (; loop < 12; loop++)
  {
    Setting *set = new Setting;
    set->setData("HIGH", top);
    set->setData("LOW", bottom);
    set->setData("UP", 0);
    set->setData("DOWN", 0);
    hash.insert(loop, set);

    bottom = top + 0.01;
    top = bottom + range;
  }

  loop = 1;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *cb = close->bar(keys.at(loop));
    if (! cb)
      continue;

    CurveBar *ycb = close->bar(keys.at(loop - 1));
    if (! ycb)
      continue;

    CurveBar *vb = vol->bar(keys.at(loop));
    if (! vb)
      continue;

    int flag = 1;
    if (cb->data() < ycb->data())
      flag = 0;

    QHashIterator<int, Setting *> it(hash);
    while (it.hasNext())
    {
      it.next();
      Setting *set = it.value();
      
      if (cb->data() >= set->getDouble("LOW") && cb->data() <= set->getDouble("HIGH"))
      {
        if (flag)
	  set->setData("UP", set->getDouble("UP") + vb->data());
	else
	  set->setData("DOWN", set->getDouble("DOWN") + vb->data());
	break;
      }
    }
  }

  Curve *line = new Curve;
  line->setType("HistogramHorizontal");

  loop = 0;
  int pos = 0;
  for (; loop < 12; loop++)
  {
    Setting *set = hash.value(loop);
    if (! set)
      continue;

    CurveBar *b = new CurveBar;
    b->setData(0, set->getDouble("HIGH"));
    b->setData(1, set->getDouble("LOW"));
    b->setData(2, set->getDouble("UP"));
    b->setData(3, set->getDouble("DOWN"));
    b->setColor(upColor);
    line->setBar(pos++, b);

    b = new CurveBar;
    b->setData(0, set->getDouble("HIGH"));
    b->setData(1, set->getDouble("LOW"));
    b->setData(2, set->getDouble("UP"));
    b->setData(3, set->getDouble("DOWN"));
    b->setColor(downColor);
    line->setBar(pos++, b);
  }

  qDeleteAll(hash);
  
  return line;
}

PluginWidget * VBP::dialog (QWidget *p)
{
  return new VBPWidget(p);
}

void VBP::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=" + _plugin;
  l << "INPUT_CLOSE=Close";
  l << "INPUT_VOLUME=Volume";
  l << "COLOR_UP=#00FF00";
  l << "COLOR_DOWN=#FF0000";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  VBP *o = new VBP;
  return ((Plugin *) o);
}
