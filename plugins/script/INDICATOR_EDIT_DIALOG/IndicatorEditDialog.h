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

#ifndef INDICATOR_EDIT_DIALOG_DIALOG_HPP
#define INDICATOR_EDIT_DIALOG_DIALOG_HPP

#include <QDialog>
#include <QStringList>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

#include "Command.h"

class IndicatorEditDialog : public QDialog
{
  Q_OBJECT

  public:
    IndicatorEditDialog (Command *);
    void createGUI ();
    void loadSettings ();
    void saveSettings ();

  public slots:
    void done ();
    void cancel ();
    void buttonStatus ();
    void help ();
    void fileButtonPressed ();
    void fileButtonPressed2 (QString);

  private:
    QLineEdit *_com;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QString _helpFile;
    QLabel *_message;
    QPushButton *_fileButton;
    QSpinBox *_row;
    QCheckBox *_date;
    QCheckBox *_log;
    QString _file;
    Command *_command;
    QString _name;
};

#endif
