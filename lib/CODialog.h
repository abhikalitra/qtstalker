/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef CODIALOG_HPP
#define CODIALOG_HPP

#include <QDialog>
#include <QString>
#include <QDialogButtonBox>
#include <QWidget>
#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>


#include "COSettings.h"
#include "ColorButton.h"
#include "FontButton.h"



class CODialog : public QDialog
{
  Q_OBJECT

  public:
    CODialog (QWidget *, COSettings *);
    void createArrowPage ();
    void createHorizontalLinePage ();
    void createTextPage ();
    void createFiboLinePage ();
    void createTrendLinePage ();
    void createVerticalLinePage ();

  public slots:
    void slotModified ();
    void slotSave ();
    void cancelPressed ();
    
  private:
    int modified;
    QTabWidget *tabs;
    QDialogButtonBox *buttonBox;
    COSettings *co;
    ColorButton *colorButton;
    QDoubleSpinBox *dspin;
    QDoubleSpinBox *dspin2;
    QDoubleSpinBox *dspin3;
    QDoubleSpinBox *dspin4;
    QDoubleSpinBox *dspin5;
    QDoubleSpinBox *dspin6;
    QDoubleSpinBox *dspin7;
    QDoubleSpinBox *dspin8;
    QCheckBox *checkDefault;
    QCheckBox *check;
    QCheckBox *check2;
    QLineEdit *text;
    FontButton *fontButton;
    QComboBox *combo;
    QDateTimeEdit *date;
};

#endif

