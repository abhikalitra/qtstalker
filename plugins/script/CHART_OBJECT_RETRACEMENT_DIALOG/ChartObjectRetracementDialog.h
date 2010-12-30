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

#ifndef CHART_OBJECT_RETRACEMENT_DIALOG_DIALOG_HPP
#define CHART_OBJECT_RETRACEMENT_DIALOG_DIALOG_HPP

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QTabWidget>

#include "ColorButton.h"
#include "Command.h"
#include "Setting.h"

class ChartObjectRetracementDialog : public QDialog
{
  Q_OBJECT

  public:
    ChartObjectRetracementDialog (Command *);
    void createDialog ();
    void createMainPage ();
    void createLinePage ();
    void loadObject ();
    void loadSettings ();
    void saveSettings ();

  public slots:
    void done ();
    void cancel ();
    void help ();

  private:
    ColorButton *_color;
    QDoubleSpinBox *_high;
    QDoubleSpinBox *_low;
    QDoubleSpinBox *_line1;
    QDoubleSpinBox *_line2;
    QDoubleSpinBox *_line3;
    QDoubleSpinBox *_line4;
    QDoubleSpinBox *_line5;
    QDoubleSpinBox *_line6;
    QCheckBox *_default;
    QCheckBox *_extend;
    QDateTimeEdit *_date;
    QDateTimeEdit *_date2;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QString _helpFile;
    QLabel *_message;
    QTabWidget *_tabs;
    Command *_command;
    Setting _co;
};

#endif
