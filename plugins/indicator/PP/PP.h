/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "IndicatorPlugin.h"

class PP : public IndicatorPlugin
{
  public:
    PP ();
    virtual ~PP ();
    void calculate ();
    void loadIndicatorSettings (QString);
    void saveIndicatorSettings (QString);
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    Setting getIndicatorSettings ();
    void setIndicatorSettings (Setting);
    int getMinBars ();
    
  private:
    QColor resColor;
    QColor supColor;
    PlotLine::LineType resLineType;
    PlotLine::LineType supLineType;
    QString resLabel;
    QString resLabel2;
    QString resLabel3;
    QString supLabel;
    QString supLabel2;
    QString supLabel3;
    QString label;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}
