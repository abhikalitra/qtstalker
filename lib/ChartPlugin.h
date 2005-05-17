/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#ifndef CHARTPLUGIN_HPP
#define CHARTPLUGIN_HPP

#include <qobject.h>
#include <qpixmap.h>
#include "BarData.h"
#include "Scaler.h"

class ChartPlugin : public QObject
{
  Q_OBJECT

  signals:
    void draw();
      
  public:
    ChartPlugin ();
    virtual ~ChartPlugin ();
    int getMinPixelspace ();
    int getStartX ();
    bool getIndicatorFlag ();
    QString getPluginName ();
    QString getHelpFile ();
    int getPixelspace ();
    void setPixelspace (int);

    virtual void drawChart (QPixmap &, Scaler &, int, int, int);
    virtual void saveSettings ();
    virtual void prefDialog (QWidget *);
    virtual void savePixelspace ();
    virtual void setChartInput (BarData *);
    
  protected:
    BarData *data;
    int minPixelspace;
    int startX;
    bool indicatorFlag;
    bool saveFlag;
    QString pluginName;
    QString helpFile;
    int currentPixelspace;
};

#endif
