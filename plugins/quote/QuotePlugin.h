/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#ifndef QUOTEPLUGIN_HPP
#define QUOTEPLUGIN_HPP

#include "Plugin.h"
#include <qstring.h>

class QuotePlugin : public Plugin
{
  Q_OBJECT

  signals:
    void done ();

  public:
    QuotePlugin ();
    virtual ~QuotePlugin ();
    bool setTFloat (QString);
    QString stripJunk (QString);
    int setFutures (QString);
    void ccMonth1 (int);
    void ccMonth2 (int);
    void ccMonth3 (int);
    void setDataPath (QString);

  protected:
    QString file;
    QString dataPath;
    float tfloat;
    QString futureName;
    QString futureSymbol;
    QString cc;
    int ccFlag;
};

#endif
