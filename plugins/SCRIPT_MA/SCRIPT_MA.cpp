/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include <QtGui>

#include "SCRIPT_MA.h"
#include "Global.h"
#include "MA.h"
#include "MAType.h"
#include "BarType.h"


DialogUser *
SCRIPT_MA::dialog (QWidget *p, Entity *settings)
{
  DialogUser *dialog = new DialogUser(p);
  dialog->setGUI(settings);
  return dialog;
}

int
SCRIPT_MA::run (Entity *settings, QList<Curve *> &, QList<Marker *> &)
{
  if (! g_symbol)
    return 0;
  
  Setting *input = settings->get(QString("input"));
  if (! input)
    return 0;
  
  Setting *label = settings->get(QString("label"));
  if (! label)
    return 0;
  
  Setting *period = settings->get(QString("period"));
  if (! period)
    return 0;
  
  Setting *type = settings->get(QString("type"));
  if (! type)
    return 0;

  MA ma;
  if (! ma.getMA(input->toString(), label->toString(), type->toInt(), period->toInt()))
    return 0;

//qDebug() << "SCRIPT_MA::run:" << input->toString() << label->toString();
  
  return 1;
}

Entity *
SCRIPT_MA::settings ()
{
  Entity *command = new Entity;
  int order = 0;
  BarType bt;
  
  // plugin
  Setting *set = new Setting;
  set->setString(QString("SCRIPT_MA"), QString(), -1);
  command->set(QString("plugin"), set);
  
  // entity type
  set = new Setting;
  set->setString(QString("script"), QString(), -1);
  command->set(QString("type"), set);

  // input
  set = new Setting;
  set->setString(bt.indexToString(BarType::_CLOSE), tr("Input"), order++);
  command->set(QString("input"), set);
  
  // color
  set = new Setting;
  set->setColor(QColor(Qt::yellow), tr("Color"), order++);
  command->set(QString("color"), set);
  
  // label
  set = new Setting;
  set->setString(QString("Plot 1"), tr("Label"), order++);
  command->set(QString("label"), set);
  
  // type
  MAType mat;
  set = new Setting;
  set->setCombo(mat.list(), mat.indexToString(MAType::_EMA), tr("Type"), order++);
  command->set(QString("type"), set);
  
  // period
  set = new Setting;
  set->setInt(10, tr("Period"), order++, 0, 999999);
  command->set(QString("period"), set);

  return command;
}


// do not remove
Q_EXPORT_PLUGIN2(script_ma, SCRIPT_MA);
