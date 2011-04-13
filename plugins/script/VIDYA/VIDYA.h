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

#ifndef PLUGIN_VIDYA_HPP
#define PLUGIN_VIDYA_HPP

#include "Plugin.h"
#include "Curve.h"

class VIDYA : public Plugin
{
  Q_OBJECT

  public:
    VIDYA ();
    int command (Command *);
    int calculate (BarData *, Indicator *, Setting *);
    void defaults (Setting *);
    QWidget * dialog (QWidget *, Setting *);
    Curve * getVIDYA (QList<Curve *> &, int period, int vperiod);
    Curve * getCMO (QList<Curve *> &, int period);
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
