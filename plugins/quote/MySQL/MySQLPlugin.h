/*
 *  MySQL plugin for Qtstalker stock charter
 *
 *  Copyright (C) 2003 Marco van Zwetselaar
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

#include "QuotePlugin.h"
#include "ChartDb.h"
#include <qstring.h>
#include <mysql.h>

class MySQLPlugin : public QuotePlugin
{
  Q_OBJECT

  public:
    MySQLPlugin ();
    virtual ~MySQLPlugin ();

    // plugin interface
    void update ();
    Setting* getCreateDetails ();
    void createChart (Setting*);

  public slots:
    void performUpdate();
    
  protected:
    void retrieveSettings();
    void storeSettings();
    void updateSymbol(QString);
    void doQuery (QString, ChartDb&);
    bool openDatabase();
    void closeDatabase();

  private:
    MYSQL mysql;
};

extern "C"
{
  Plugin * create ();
}
