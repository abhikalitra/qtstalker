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

#ifndef CONFIGURE_DIALOG_HPP
#define CONFIGURE_DIALOG_HPP

#include <QTabWidget>
#include <QHash>
#include <QComboBox>

#include "ColorButton.h"
#include "FontButton.h"
#include "Dialog.h"

class ConfigureDialog : public Dialog
{
  Q_OBJECT

  public:
    ConfigureDialog (QWidget *);
    void createGUI ();
    void createGeneralPage ();

  public slots:
    void done ();
    void buttonStatus ();
    void backgroundChanged ();
    void appFontChanged ();
    void plotFontChanged ();
    void tabPositionChanged ();

  private:
    QTabWidget *_tabs;
    ColorButton *_background;
    FontButton *_appFont;
    FontButton *_plotFont;
    int _modified;
    QHash<QString, int> _flags;
    QComboBox *_tabPosition;
};

#endif
