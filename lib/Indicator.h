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

#ifndef INDICATOR_HPP
#define INDICATOR_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include "PlotLine.h"

class Indicator
{
  public:
    Indicator ();
    ~Indicator ();
    int getLines ();
    void addLine (PlotLine *);
    PlotLine * getLine (int);
    int getTabRow ();
    void setTabRow (int);
    void clearLines ();
    void setEnable (bool);
    bool getEnable ();
    void setName (QString &);
    QString getName ();
    void setFile (QString &);
    QString getFile ();
    void copy (Indicator *);
    void setType (QString &);
    QString getType ();
    void setDateFlag (bool);
    bool getDateFlag ();
    void setLogScale (bool);
    bool getLogScale ();

  private:
    QPtrList<PlotLine> lines;
    bool enable;
    QString name;
    QString file;
    QString type;
    int tabRow;
    bool dateFlag;
    bool logScale;
};

#endif

