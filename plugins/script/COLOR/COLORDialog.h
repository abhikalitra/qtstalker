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

#ifndef PLUGIN_COLOR_DIALOG_HPP
#define PLUGIN_COLOR_DIALOG_HPP

#include <QTreeWidget>

#include "Setting.h"

class COLORDialog : public QWidget
{
  Q_OBJECT

  public:
    COLORDialog (QWidget *, Setting *);
    void createGeneralPage();
    void loadSettings ();

  public slots:
    void save ();
    void addPattern ();
    void addPattern (QString color, QString in, QString op, QString in2, QString in3);
    void deletePattern ();

  private:
    Setting *_settings;
    QTreeWidget *_plist;
};

#endif
