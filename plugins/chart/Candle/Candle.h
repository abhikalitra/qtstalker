/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#ifndef CANDLE_HPP
#define CANDLE_HPP

#include "ChartPlugin.h"
#include <qcolor.h>

class Candle : public ChartPlugin
{
  Q_OBJECT

  public:
    Candle ();
    virtual ~Candle ();
    void drawChart (int startX, int startIndex, int pixelspace);
    void drawCandles (int startX, int startIndex, int pixelspace);
    void drawQSCandles (int startX, int startIndex, int pixelspace);
    void drawVolumeCandles (int startX, int startIndex, int pixelspace);
    QColor volumeColor (int index);
    void prefDialog ();
    void loadSettings ();
    void saveSettings ();

  protected:
    QString style;
    bool expandCandles;
    
    // candle vars
    QColor candleColor;
    
    // qs vars
    QColor qsNeutralColor;
    QColor qsUpColor;
    QColor qsDownColor;

    // volume vars
    QColor c0, c1, c2, c3, c4, c5;  // persistant values holding color prefs
    double vr1, vr2, vr3, vr4, vr5; // persistant values holding trigger points
    int vma;                        // persistant value holding ma period for volume
    int minCandleRadius;
    int fixedCandleRadius;
    int maxCandleGap;
};

extern "C"
{
  Plugin * create ();
};

#endif
