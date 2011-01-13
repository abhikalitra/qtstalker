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

#ifndef SCRIPT_EDIT_DIALOG_DIALOG_HPP
#define SCRIPT_EDIT_DIALOG_DIALOG_HPP

#include <QDialog>
#include <QStringList>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>

#include "Command.h"
#include "FileButton.h"

class ScriptEditDialog : public QDialog
{
  Q_OBJECT

  public:
    ScriptEditDialog (Command *);
    void createGUI ();
    void loadSettings ();
    void saveSettings ();
    void loadScript ();

  public slots:
    void done ();
    void buttonStatus ();
    void cancel ();
    void help ();

  private:
    QLineEdit *_com;
    QSpinBox *_minutes;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    FileButton *_fileButton;
    QString _helpFile;
    QLabel *_message;
    QString _file;
    QString _name;
    Command *_command;
};

#endif
