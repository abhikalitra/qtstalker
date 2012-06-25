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

#ifndef PLUGIN_BBANDS_DIALOG_HPP
#define PLUGIN_BBANDS_DIALOG_HPP

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QStringList>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

#include "Entity.h"
#include "ColorButton.h"

class BBANDSDialog : public QDialog
{
  Q_OBJECT

  public:
    BBANDSDialog (QWidget *);
    void setGUI (Entity *);

  public slots:
    void done ();
    void cancel ();
    void help ();
    void loadSettings ();
    void saveSettings ();
    void modified ();
    void save ();

  protected:
    Entity *_settings;
    QTabWidget *_tabs;
    QDialogButtonBox *_buttonBox;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QString _helpFile;
    int _modified;
    
    QComboBox *_ohlcStyle;
    ColorButton *_upColor;
    ColorButton *_downColor;
    ColorButton *_neutralColor;
    
    QComboBox *_input;
    QSpinBox *_period;
    QDoubleSpinBox *_devUp;
    QDoubleSpinBox *_devDown;
    QComboBox *_ma;
    
    QComboBox *_upInput;
    QComboBox *_upStyle;
    QSpinBox *_upWidth;
    ColorButton *_upperColor;
    QCheckBox *_upShow;
    
    QComboBox *_midStyle;
    QSpinBox *_midWidth;
    ColorButton *_midColor;
    QCheckBox *_midShow;
    
    QComboBox *_downStyle;
    QSpinBox *_downWidth;
    ColorButton *_lowerColor;
    QCheckBox *_downShow;
};

#endif
