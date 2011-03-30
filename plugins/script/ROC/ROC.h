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

#ifndef PLUGIN_ROC_HPP
#define PLUGIN_ROC_HPP

#include "Plugin.h"

class ROC : public Plugin
{
  Q_OBJECT

  public:
    enum Method
    {
      _ROCN,
      _ROCP,
      _ROCR,
      _ROCR100
    };

    enum Parm
    {
      _COLOR = 10,
      _STYLE = 20,
      _LABEL = 30,
      _PERIOD = 40,
      _SMOOTHING = 50,
      _SMOOTHING_TYPE = 60,
      _INPUT = 70,
      _METHOD = 80
    };

    ROC ();
    int command (Command *);
    int calculate (BarData *, Indicator *);
    void defaults (Setting *);
    void dialog (QWidget *, Indicator *);
    QStringList method ();

  protected:
    QStringList _method;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
