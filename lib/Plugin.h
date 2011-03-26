/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include <QObject>
#include <QWidget>

#include "Command.h"
#include "Indicator.h"
#include "Setting.h"
#include "BarData.h"

class Plugin : public QObject
{
  Q_OBJECT

  signals:
    void signalResume ();
    void signalKill ();
  
  public:
    enum Type
    {
      _DIALOG,
      _THREAD,
      _INDICATOR
    };
    
    Plugin ();
    virtual ~Plugin ();
    virtual int command (Command *);
    virtual int calculate (BarData *, Indicator *);
    virtual void defaults (Setting *);
    virtual void dialog (QWidget *, Indicator *);
    
    int type ();
    void setParent (QWidget *);

  protected:
    int _type;
    QString _plugin;
    QWidget *_parent;
};

#endif
