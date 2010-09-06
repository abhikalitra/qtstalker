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

#ifndef CHART_OBJECT_TLINE_DIALOG_HPP
#define CHART_OBJECT_TLINE_DIALOG_HPP

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateTimeEdit>

#include "ChartObjectDialog.h"
#include "ColorButton.h"

class ChartObjectTLineDialog : public ChartObjectDialog
{
  Q_OBJECT

  public:
    ChartObjectTLineDialog ();
    void createMainPage ();
    void setSettings (ChartObjectSettings &);

  public slots:
    void done ();

  private:
    ColorButton *_color;
    QDoubleSpinBox *_price;
    QDoubleSpinBox *_price2;
    QCheckBox *_default;
    QCheckBox *_extend;
    QDateTimeEdit *_date;
    QDateTimeEdit *_date2;
};

#endif
