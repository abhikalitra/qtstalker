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

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <stdlib.h>
#include <qstring.h>
#include <qlist.h>
#include <qmemarray.h>
#include <qobject.h>
#include <qcolor.h>
#include "Setting.h"
#include "PlotLine.h"

#define COPYRIGHT "(C) 2001, 2002 Stefan Stratigakos"

class Plugin : public Setting, public QObject
{
  public:
    Plugin ();
    virtual ~Plugin ();

    // indicator plugin interface
    virtual void setIndicatorInput (QList<Setting> *);
    virtual int getIndicatorLines ();
    virtual PlotLine * getIndicatorLine (int);
    virtual QMemArray<int> getAlerts ();
    virtual QList<QColor> getColorBars (QString, QString, QString);
    virtual void calculate ();

    // quote plugin interface
    virtual void update ();
    virtual void cancelUpdate ();
    virtual Setting * getCreateDetails ();
    virtual void createChart (Setting *);
    virtual bool getCreateFlag ();

    // base plugin functions
    QString getPluginType ();
    QString getPluginName ();
    QString getVersion ();
    QString getAbout ();
    void setDataPath (QString);

  protected:
    QString pluginType;
    QString pluginName;
    QString dataPath;
    QString about;
    float version;
};

extern "C"
{
  Plugin * create ();
}

#endif


