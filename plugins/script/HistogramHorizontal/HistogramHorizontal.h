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

#ifndef PLUGIN_HISTOGRAM_HORIZONTAL_HPP
#define PLUGIN_HISTOGRAM_HORIZONTAL_HPP

#include <QStringList>
#include <QHash>
#include <qwt_plot.h>

#include "Plugin.h"
#include "HistogramHorizontalDraw.h"
#include "Setting.h"

class HistogramHorizontal : public Plugin
{
  Q_OBJECT

  public:
    HistogramHorizontal ();
    ~HistogramHorizontal ();
    int setCurve (Curve *);
    int request (Setting *, Setting *);
    void setParent (void *);
    int clear ();
    int highLowRange (Setting *, Setting *);

  private:
    QwtPlot *_plot;
    QHash<QString, HistogramHorizontalDraw *> _items;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif