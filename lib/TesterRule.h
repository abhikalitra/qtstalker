/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef TESTERRULE_HPP
#define TESTERRULE_HPP


#include "Indicator.h"




class TesterRule
{
  public:
    TesterRule ();
    void getName (QString &);
    void setName (QString &);

    void setEnterLong (Indicator &);
    void getEnterLong (Indicator &);
    void setExitLong (Indicator &);
    void getExitLong (Indicator &);
    void setEnterShort (Indicator &);
    void getEnterShort (Indicator &);
    void setExitShort (Indicator &);
    void getExitShort (Indicator &);

    void setCustomLongStop (Indicator &);
    void getCustomLongStop (Indicator &);
    void setCustomShortStop (Indicator &);
    void getCustomShortStop (Indicator &);
    void setCustomLongCheck (bool);
    bool getCustomLongCheck ();
    void setCustomShortCheck (bool);
    bool getCustomShortCheck ();

    void setMaxLossCheck (bool);
    void setMaxLossLong (bool);
    void setMaxLossShort (bool);
    void setMaxLoss (double);
    bool getMaxLossCheck ();
    bool getMaxLossLong ();
    bool getMaxLossShort ();
    double getMaxLoss ();

    void setProfitCheck (bool);
    void setProfitLong (bool);
    void setProfitShort (bool);
    void setProfit (double);
    bool getProfitCheck ();
    bool getProfitLong ();
    bool getProfitShort ();
    double getProfit ();

    void setTrailingCheck (bool);
    void setTrailingLong (bool);
    void setTrailingShort (bool);
    void setTrailing (double);
    bool getTrailingCheck ();
    bool getTrailingLong ();
    bool getTrailingShort ();
    double getTrailing ();
    
  protected:
    QString name;

    Indicator enterLong;
    Indicator exitLong;
    Indicator enterShort;
    Indicator exitShort;

    Indicator customLongStop;
    Indicator customShortStop;
    bool customLongCheck;
    bool customShortCheck;

    bool maxLossCheck;
    bool maxLossLong;
    bool maxLossShort;
    double maxLoss;

    bool profitCheck;
    bool profitLong;
    bool profitShort;
    double profit;

    bool trailingCheck;
    bool trailingLong;
    bool trailingShort;
    double trailing;
};

#endif
