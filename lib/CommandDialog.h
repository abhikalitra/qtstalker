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

#ifndef COMMAND_DIALOG_HPP
#define COMMAND_DIALOG_HPP

#include <QStringList>
#include <QHash>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QTabWidget>

#include "Dialog.h"
#include "SettingGroup.h"
#include "LineEdit.h"
#include "ColorButton.h"
#include "FileButton.h"

class CommandDialog : public Dialog
{
  Q_OBJECT

  signals:
//    void signalDone (QString);

  public:
    CommandDialog (QWidget *);
    void createGUI ();
    void setWidgets (SettingGroup *);

  public slots:
    void done ();

  protected:
    QHash<QString, LineEdit *> _lineEdit;
    QHash<QString, ColorButton *> _colorButton;
    QHash<QString, QSpinBox *> _spinBox;
    QHash<QString, QDoubleSpinBox *> _doubleSpinBox;
    QHash<QString, QCheckBox *> _checkBox;
    QHash<QString, QDateTimeEdit *> _dateTimeEdit;
    QHash<QString, QComboBox *> _comboBox;
    QHash<QString, FileButton *> _fileButton;
    SettingGroup *_settings;
    QTabWidget *_tabs;
};

#endif
