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

#ifndef PLUGIN_CANDLE_PATTERN_DIALOG_HPP
#define PLUGIN_CANDLE_PATTERN_DIALOG_HPP

#include <QDoubleSpinBox>
#include <QTabWidget>
#include <QTreeWidget>
#include <QSpinBox>

#include "ColorButton.h"
#include "Setting.h"

class CandlePatternDialog : public QWidget
{
  Q_OBJECT

  public:
    CandlePatternDialog (QWidget *, Setting *);
    void createGeneralPage ();
    void createPatternPage ();
    void loadSettings ();

  public slots:
    void save ();
    void addPattern ();
    void addPattern (QString, QString);
    void deletePattern ();

  private:
    Setting *_settings;
    QTabWidget *_tabs;
    ColorButton *_color;
    QDoubleSpinBox *_pen;
    QTreeWidget *_plist;
    QSpinBox *_z;
};

#endif
