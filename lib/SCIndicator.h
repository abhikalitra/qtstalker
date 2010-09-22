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

#ifndef SCINDICATOR_HPP
#define SCINDICATOR_HPP

#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "Indicator.h"

class SCIndicator
{
  public:
    enum Method
    {
      NEW, // create a new plotline to be filled later by the script
      GET_INDEX, // pass a single indicator value to the calling script
      GET_INDEX_DATE, // pass a single date value to the calling script
      GET_RANGE, // returns the start and end index values
      PLUGIN, // get a plugin indicator
      SET_INDEX, // pass a single indicator value from script to qtstalker
      SET_COLOR, // set the color of the bar at index pos
      DELETE,  // delete an indicator
      SET_COLOR_ALL, // set all bars to color
      SET_PLOT_STYLE, // set the plot style
      SET_PLOT // plot this indicator
    };
    
    SCIndicator ();
    int calculate (QStringList &, QByteArray &, Indicator &);
    int getIndex (QStringList &, QByteArray &, Indicator &);
    int getIndexDate (QStringList &, QByteArray &);
    int setIndex (QStringList &, QByteArray &, Indicator &);
    int getRange (QStringList &, QByteArray &, Indicator &);
    int setColor (QStringList &, QByteArray &, Indicator &);
    int getPlugin (QStringList &, QByteArray &, Indicator &);
    int getNew (QStringList &, QByteArray &, Indicator &);
    int setDelete (QStringList &, QByteArray &, Indicator &);
    int setColorAll (QStringList &, QByteArray &, Indicator &);
    int setPlotStyle (QStringList &, QByteArray &, Indicator &);
    int setPlot (QStringList &, QByteArray &, Indicator &);

  protected:
    QStringList methodList;
};

#endif
