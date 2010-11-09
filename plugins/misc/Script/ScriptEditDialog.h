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

#ifndef SCRIPT_EDIT_DIALOG_HPP
#define SCRIPT_EDIT_DIALOG_HPP

#include <QString>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>

#include "FileButton.h"
#include "Dialog.h"

class ScriptEditDialog : public Dialog
{
  Q_OBJECT

  public:
    ScriptEditDialog (QString);
    void createMainPage ();
    void loadScript ();

  public slots:
    void done ();
    void buttonStatus ();
    void confirmYes ();
    void cancel ();
    void fileChanged ();

  private:
    QString _name;
    QComboBox *_type;
    QLineEdit *_command;
    QTextEdit *_comment;
    FileButton *_file;
    int _saveFlag;
};

#endif