/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#ifndef GROUPDIALOG_HPP
#define GROUPDIALOG_HPP

#include <qtoolbutton.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qlist.h>
#include <qstringlist.h>
#include "Config.h"
#include "Navigator.h"

class GroupDialog : public QDialog
{
  Q_OBJECT

  public:
    GroupDialog (Config *);
    ~GroupDialog ();
    QStringList getGroup ();
    bool getFlag ();

  public slots:
    void insertItem ();
    void deleteItem ();
    void symbolSelected (QString);
    void listSelected (QListViewItem *i);
    void upDirectory ();

  private:
    Config *config;
    QToolButton *okButton;
    QToolButton *cancelButton;
    QToolButton *deleteButton;
    QToolButton *insertButton;
    QToolButton *upButton;
    QGridLayout *toolbar;
    QListView *list;
    QListViewItem *item;
    bool flag;
    Navigator *fileSelector;
};

#endif
