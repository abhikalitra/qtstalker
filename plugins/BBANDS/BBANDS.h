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


#ifndef PLUGIN_BBANDS_H
#define PLUGIN_BBANDS_H

#include <QObject>

#include "Plugin.h"

class BBANDS : public QObject, public Plugin
{
  Q_OBJECT
  Q_INTERFACES(Plugin)

  public:
    int draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *);
    int command (PluginData *);    
    int settings (PluginData *);
    int dialog (PluginData *);
    int run (PluginData *);
    int getBBANDS (QString inKey, int period, double pdev, double ndev, int type,
                   QString ulabel, QString mlabel, QString llabel);
    Curve * getOHLC (QString tstyle, QString key, QString tuc, QString tdc, QString tnc);
};

#endif
