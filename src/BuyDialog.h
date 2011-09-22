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

#ifndef BUY_DIALOG_HPP
#define BUY_DIALOG_HPP

#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QDateTimeEdit>

#include "ColorButton.h"
#include "Dialog.h"
#include "Data.h"

class BuyDialog : public Dialog
{
  Q_OBJECT

  public:
    BuyDialog (QWidget *, Data *);
    void createGUI ();

  public slots:
    void done ();

  private:
    Data *_settings;
    QTabWidget *_tabs;
    ColorButton *_color;
    QDoubleSpinBox *_price;
    QSpinBox *_z;
    QDateTimeEdit *_date;
};

#endif
