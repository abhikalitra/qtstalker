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

#ifndef COPLUGIN_HPP
#define COPLUGIN_HPP

#include "Scaler.h"
#include "BarData.h"
#include "BarDate.h"
#include "Setting.h"
#include <qpixmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qpopupmenu.h>

#define HANDLE_WIDTH 6

class COPlugin : public QObject
{
  Q_OBJECT
  
  signals:
    void signalDraw ();
    void signalRefresh ();
    void message (QString);
    void signalObjectDeleted ();

  public:
    enum Status
    {
      None,
      ClickWait,
      ClickWait2,
      Selected,
      Moving
    };
  
    COPlugin ();
    virtual ~COPlugin ();
    virtual void draw (QPixmap &, Scaler &, int, int, int);
    virtual void newObject (QString &, QString &);
    virtual void addObject (Setting &);
    virtual void saveObjects (QString &);
    virtual void clear ();
    virtual void keyEvent (QKeyEvent *);
    virtual double getHigh ();
    virtual double getLow ();
    virtual void showMenu ();
    virtual void getNameList (QStringList &);
    
    void setData (BarData *);
    QString getHelpFile ();

  public slots:    
    virtual void prefDialog ();
    virtual COPlugin::Status pointerClick (QPoint &, BarDate &, double);
    virtual void pointerMoving (QPixmap &, QPoint &, BarDate &, double y);
    
  protected:
    BarData *data;
    QPopupMenu *menu;
    QString indicator;
    QString name;
    QString helpFile;
};

#endif
