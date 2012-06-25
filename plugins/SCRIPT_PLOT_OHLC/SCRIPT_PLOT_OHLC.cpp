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

#include "SCRIPT_PLOT_OHLC.h"
#include "CurveOHLCType.h"
#include "Global.h"
#include "BarType.h"
#include "OHLCFunction.h"


DialogUser *
SCRIPT_PLOT_OHLC::dialog (QWidget *p, Entity *settings)
{
  DialogUser *dialog = new DialogUser(p);
  dialog->setGUI(settings);
  return dialog;
}

int
SCRIPT_PLOT_OHLC::run (Entity *settings, QList<Curve *> &output, QList<Marker *> &)
{
  if (! g_symbol)
    return 0;
  
  Setting *color = settings->get(QString("color"));
  if (! color)
    return 0;
  
  Setting *label = settings->get(QString("label"));
  if (! label)
    return 0;
  
  Setting *style = settings->get(QString("style"));
  if (! style)
    return 0;
  
  OHLCFunction f;
  Curve *ohlc = f.getOHLC(style->toInt(), label->toString(), color->toColor(), color->toColor(), color->toColor());
  if (! ohlc)
    return 0;
  
  output << ohlc;
  
  return 1;
}

Entity *
SCRIPT_PLOT_OHLC::settings ()
{
  Entity *command = new Entity;
  int order = 0;
  BarType bt;
  
  // plugin
  Setting *set = new Setting;
  set->setString(QString("SCRIPT_PLOT_OHLC"), QString(), -1);
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
  set->setColor(QColor(Qt::blue), tr("Color"), order++);
  command->set(QString("color"), set);
  
  // label
  set = new Setting;
  set->setString(QString("OHLC"), tr("Label"), order++);
  command->set(QString("label"), set);
  
  // style
  CurveOHLCType ct;
  set = new Setting;
  set->setCombo(ct.list(), ct.indexToString(CurveOHLCType::_OHLC), tr("Style"), order++);
  command->set(QString("style"), set);

  return command;
}


// do not remove
Q_EXPORT_PLUGIN2(script_plot_ohlc, SCRIPT_PLOT_OHLC);
