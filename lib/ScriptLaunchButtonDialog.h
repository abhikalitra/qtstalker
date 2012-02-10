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

#ifndef SCRIPT_LAUNCH_BUTTON_DIALOG_HPP
#define SCRIPT_LAUNCH_BUTTON_DIALOG_HPP

#include <QCheckBox>
#include <QString>
#include <QSpinBox>
#include <QComboBox>

#include "IconButton.h"
#include "Dialog.h"
#include "WidgetLineEdit.h"

class ScriptLaunchButtonDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone (QString, QString, bool, int, int, QString);

  public:
    ScriptLaunchButtonDialog (QWidget *, QString script, QString icon, bool, int row, int col, QString text);
    void createMainPage ();
    void setGUI (QString script, QString icon, bool use, int tor, int col, QString text);

  public slots:
    void done ();
    void useIconToggled (bool);
    void buttonStatus ();

  private:
    IconButton *_icon;
    QComboBox *_script;
    WidgetLineEdit *_text;
    QCheckBox *_useIcon;
    QSpinBox *_row;
    QSpinBox *_col;
};

#endif
