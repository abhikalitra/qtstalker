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

#ifndef BARS_DIALOG_DIALOG_HPP
#define BARS_DIALOG_DIALOG_HPP

//#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QTabWidget>

#include "Command.h"
#include "ColorButton.h"
#include "LineEdit.h"

class BARSDialog : public QDialog
{
  Q_OBJECT

  public:
    BARSDialog (Command *);
    void createGUI ();
    void createBARSPage();
    void createMA1Page();
    void createMA2Page();
    void createMA3Page();
    void loadSettings ();
    void saveSettings ();
    void setDialogSettings ();
    void dialogSettings (QString &);

  public slots:
    void done ();
    void cancel ();
    void help ();

  private:
    QTabWidget *_tabs;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QString _helpFile;
    Command *_command;
    ColorButton *_upColor;
    ColorButton *_downColor;
    ColorButton *_neutralColor;
    ColorButton *_ma1Color;
    ColorButton *_ma2Color;
    ColorButton *_ma3Color;
    QComboBox *_ma1Type;
    QComboBox *_ma2Type;
    QComboBox *_ma3Type;
    QComboBox *_ma1PlotStyle;
    QComboBox *_ma2PlotStyle;
    QComboBox *_ma3PlotStyle;
    QSpinBox *_ma1Period;
    QSpinBox *_ma2Period;
    QSpinBox *_ma3Period;
    LineEdit *_barsLabel;
    LineEdit *_ma1Label;
    LineEdit *_ma2Label;
    LineEdit *_ma3Label;
    QString _name;
};

#endif
