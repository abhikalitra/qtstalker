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

#ifndef PP_HPP
#define PP_HPP

#include "IndicatorPlugin.h"

class PP : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      R1Color = 10,
      R2Color = 20,
      R3Color = 30,
      S1Color = 40,
      S2Color = 50,
      S3Color = 60,
      R1Label = 70,
      R2Label = 80,
      R3Label = 90,
      S1Label = 100,
      S2Label = 110,
      S3Label = 120,
      R1Show = 130,
      R2Show = 140,
      R3Show = 150,
      S1Show = 160,
      S2Show = 170,
      S3Show = 180
    };

    PP ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
