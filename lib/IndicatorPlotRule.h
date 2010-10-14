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

// ******************************************************
// ******************************************************

#ifndef INDICATOR_PLOT_RULE_HPP
#define INDICATOR_PLOT_RULE_HPP

#include <QString>

class IndicatorPlotRule
{
  public:
    IndicatorPlotRule ();
    void setEnable (int);
    int enable ();
    void setOp (int);
    int op ();
    void setName (QString);
    QString & name ();
    void setValue (QString);
    QString & value ();
    
  private:
    int _enable;
    int _op;
    QString _name;
    QString _value;
};

#endif
