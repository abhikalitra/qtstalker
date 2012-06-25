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

#include "SCRIPT_PLOT_HISTOGRAM.h"
#include "CurveHistogramType.h"
#include "Global.h"
#include "BarType.h"


DialogUser *
SCRIPT_PLOT_HISTOGRAM::dialog (QWidget *p, Entity *settings)
{
  DialogUser *dialog = new DialogUser(p);
  dialog->setGUI(settings);
  return dialog;
}

int
SCRIPT_PLOT_HISTOGRAM::run (Entity *settings, QList<Curve *> &output, QList<Marker *> &)
{
  if (! g_symbol)
    return 0;
  
  Setting *high = settings->get(QString("high"));
  if (! high)
    return 0;
  
  Setting *low = settings->get(QString("low"));
  if (! low)
    return 0;
  
  Setting *label = settings->get(QString("label"));
  if (! label)
    return 0;
  
  Setting *color = settings->get(QString("color"));
  if (! color)
    return 0;
  
  Setting *style = settings->get(QString("style"));
  if (! style)
    return 0;
  
  Curve *line = new Curve(QString("CurveHistogram"));
  line->setColor(color->toColor());
  line->setLabel(label->toString());
  line->setStyle(style->toInt());
  line->fill(high->toString(), low->toString(), QString(), QString(), color->toColor());
  line->setZ(0);
  output << line;
  
//qDebug() << "SCRIPT_PLOT_HISTOGRAM::run:" << input->toString() << label->toString();

  return 1;
}

Entity *
SCRIPT_PLOT_HISTOGRAM::settings ()
{
  Entity *command = new Entity;
  int order = 0;
  BarType bt;
  
  // plugin
  Setting *set = new Setting;
  set->setString(QString("SCRIPT_PLOT_HISTOGRAM"), QString(), -1);
  command->set(QString("plugin"), set);
  
  // entity type
  set = new Setting;
  set->setString(QString("script"), QString(), -1);
  command->set(QString("type"), set);

  // high
  set = new Setting;
  set->setString(bt.indexToString(BarType::_HIGH), tr("High"), order++);
  command->set(QString("high"), set);

  // low
  set = new Setting;
  set->setString(QString(), tr("Low"), order++);
  command->set(QString("low"), set);
  
  // color
  set = new Setting;
  set->setColor(QColor(Qt::yellow), tr("Color"), order++);
  command->set(QString("color"), set);
  
  // label
  set = new Setting;
  set->setString(QString("Plot 1"), tr("Label"), order++);
  command->set(QString("label"), set);
  
  // style
  CurveHistogramType style;
  set = new Setting;
  set->setCombo(style.list(), style.indexToString(CurveHistogramType::_BAR), tr("Style"), order++);
  command->set(QString("style"), set);

  return command;
}


// do not remove
Q_EXPORT_PLUGIN2(script_plot_histogram, SCRIPT_PLOT_HISTOGRAM);
