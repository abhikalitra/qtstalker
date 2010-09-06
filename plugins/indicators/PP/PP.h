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
      R1Color,
      R2Color,
      R3Color,
      S1Color,
      S2Color,
      S3Color,
      R1Label,
      R2Label,
      R3Label,
      S1Label,
      S2Label,
      S3Label,
      R1Show,
      R2Show,
      R3Show,
      S1Show,
      S2Show,
      S3Show
    };

    PP ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
