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

#ifndef SCRIPT_DIALOG_HPP
#define SCRIPT_DIALOG_HPP

#include <QString>
#include <QLineEdit>
#include <QTextEdit>

#include "Script.h"
#include "FileButton.h"
#include "Dialog.h"
#include "ScriptDataBase.h"

class ScriptDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalRunScript (QString, QString);
  
  public:
    ScriptDialog (QString);
    void createMainPage ();

  public slots:
    void done ();
    void buttonStatus ();
    void confirmYes ();
    void cancel ();

  private:
    Script _script;
    QLineEdit *_command;
    QTextEdit *_comment;
    FileButton *_file;
    ScriptDataBase _db;
    int _saveFlag;
    int _editFlag;
};

#endif
