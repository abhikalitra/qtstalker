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

#ifndef INDICATOR_EDIT_DIALOG_HPP
#define INDICATOR_EDIT_DIALOG_HPP

#include <QStringList>
#include <QTabWidget>

#include "Dialog.h"

class IndicatorEditDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone ();
    void signalSave ();

  public:
    enum Status
    {
      _NONE,
      _DONE,
      _APPLY
    };
    
    IndicatorEditDialog (QWidget *, QString);
    void createGUI ();
    void addTab (QString, int);
    void newDialog ();
    void createDefaults ();

  public slots:
    void done ();
    void addIndicator ();
    void addIndicator2 (QString);
    void deleteIndicator ();
    void newDialog2 (QString);
    void  apply ();
    int  applySave ();

  private:
    QTabWidget *_tabs;
    QString _name;
    int _status;
    int _newFlag;
};

#endif
