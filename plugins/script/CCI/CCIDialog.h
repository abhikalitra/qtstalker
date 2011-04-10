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

#ifndef PLUGIN_CCI_DIALOG_HPP
#define PLUGIN_CCI_DIALOG_HPP

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <QSpinBox>
#include <QLineEdit>

#include "ColorButton.h"
#include "Setting.h"

class CCIDialog : public QWidget
{
  Q_OBJECT

  public:
    CCIDialog (QWidget *, Setting *);
    void createGeneralPage ();
    void createRefPage ();
    void createRef2Page ();

  public slots:
    void save ();

  private:
    Setting *_settings;
    QTabWidget *_tabs;
    QSpinBox *_period;
    ColorButton *_color;
    ColorButton *_refColor;
    ColorButton *_ref2Color;
    QComboBox *_style;
    QDoubleSpinBox *_ref;
    QDoubleSpinBox *_ref2;
    QSpinBox *_z;
    QLineEdit *_output;
};

#endif