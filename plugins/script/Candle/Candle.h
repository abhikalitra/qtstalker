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

#ifndef PLUGIN_CANDLE_HPP
#define PLUGIN_CANDLE_HPP

#include <QStringList>
#include <QHash>
#include <qwt_plot.h>

#include "Plugin.h"
#include "CandleDraw.h"
#include "Setting.h"

class Candle : public Plugin
{
  Q_OBJECT

  public:
    Candle ();
    ~Candle ();
    int setCurve (Curve *);
    int request (Setting *, Setting *);
    void setParent (void *);
    int clear ();
    int info (Setting *, Setting *);
    int highLowRange (Setting *, Setting *);

  private:
    QwtPlot *_plot;
    QHash<QString, CandleDraw *> _items;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
