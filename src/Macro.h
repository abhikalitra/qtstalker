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

#ifndef MACRO_HPP
#define MACRO_HPP

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qevent.h>

class Macro : public QObject
{
  Q_OBJECT
  
  public:
  
    enum MacroZone
    {
      ChartPage,
      GroupPage,
      IndicatorPage,
      PortfolioPage,
      TestPage,
      ScannerPage,
      MacroPage,
      Menubar,
      ChartToolbar,
      SidePanel
    };
    
    Macro(QString);
    ~Macro();
    void load ();
    void save ();
    void record ();
    void stop ();
    QKeyEvent * getKey ();
    int getCount ();
    void setKey (int);
    int getZone ();
    
  public slots:
    void recordKey (int, int, int, int, QString);
    
  private:
    QString path;
    QStringList list;
    bool flag;
    int zone;
    int state;
    int key;
    int ascii;
    QString text;
};

#endif
