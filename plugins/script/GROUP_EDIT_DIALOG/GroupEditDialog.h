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

#ifndef GROUP_EDIT_DIALOG_DIALOG_HPP
#define GROUP_EDIT_DIALOG_DIALOG_HPP

#include <QDialog>
#include <QStringList>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

#include "ScriptPluginFactory.h"

class GroupEditDialog : public QDialog
{
  Q_OBJECT

  public:
    GroupEditDialog (Command *);
    ~GroupEditDialog ();
    void createGUI ();
    void loadSettings ();
    void saveSettings ();

  public slots:
    void done ();
    void cancel ();
    void selectionChanged ();
    void help ();
    void addButtonPressed ();
    void addButtonPressed2 ();
    void deleteButtonPressed ();

  private:
    QListWidget *_list;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QPushButton *_addButton;
    QPushButton *_deleteButton;
    QString _helpFile;
    QLabel *_message;
    QString _name;
    ScriptPluginFactory _factory;
    Command *_command;
    Command *_symbolDialogCommand;
};

#endif
