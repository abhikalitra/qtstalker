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

#ifndef TAB_WIDGET_DIALOG_HPP
#define TAB_WIDGET_DIALOG_HPP

#include <QSpinBox>
#include <QComboBox>
#include <QStringList>

#include "Dialog.h"

class TabWidgetDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalChanged ();
  
  public:
    TabWidgetDialog (QString id);
    void createMainPage ();

  public slots:
    void done ();

  private:
    QStringList _posList;
    QString _id;
    int _ttp;
    int _tnsh;
    int _tnsw;
    int _tewh;
    int _teww;
    QComboBox *_position;
    QSpinBox *_nsh;
    QSpinBox *_nsw;
    QSpinBox *_ewh;
    QSpinBox *_eww;
};

#endif
