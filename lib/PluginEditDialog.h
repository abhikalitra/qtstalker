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

#ifndef PLUGIN_EDIT_DIALOG_HPP
#define PLUGIN_EDIT_DIALOG_HPP

#include <QStringList>
#include <QTabWidget>
#include <QList>
#include <QListWidget>
#include <QStackedWidget>
#include <QSplitter>
#include <QAction>
#include <QMenu>

#include "Dialog.h"
#include "Setting.h"

class PluginEditDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone ();
    void signalSave ();

  public:
    enum Status
    {
      _NONE,
      _DONE,
      _APPLY
    };
    
    PluginEditDialog (QWidget *, QString, QString);
    ~PluginEditDialog ();
    void createGUI ();
    void createActions ();
    void newDialog ();
    void loadPlugins ();
    void moveItem (int oldRow, int newRow);

  public slots:
    void loadSettings ();
    void saveSettings ();
    void done ();
    void addItem ();
    void addItem2 (QString);
    void deleteItem ();
    void insertItem ();
    void insertItem2 (QString);
    void newDialog2 (QString);
    void apply ();
    int applySave ();
    void renameItem ();
    void renameItem2 (QString);
    void listSelectionChanged ();
    void rightClick(const QPoint &);
    void upItem ();
    void downItem ();

  private:
    QList<Setting *> _settings;
    QListWidget *_list;
    QStackedWidget *_stack;
    QAction *_deleteAction;
    QAction *_insertAction;
    QAction *_renameAction;
    QAction *_addAction;
    QAction *_upAction;
    QAction *_downAction;
    QSplitter *_splitter;
    QMenu *_menu;
    QString _name;
    QString _table;
    int _status;
    int _newFlag;
};

#endif
