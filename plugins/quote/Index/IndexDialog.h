/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include <qlistview.h>
#include <qpushbutton.h>
#include <qtabdialog.h>
#include <qlabel.h>
#include <qdict.h>

class IndexDialog : public QTabDialog
{
  Q_OBJECT
  
  public:
    IndexDialog ();
    ~IndexDialog ();
    void setList (QString);
    QString getList ();
    void saveData ();
    
  public slots:
    void addItem ();
    void editItem ();
    void deleteItem ();
    void buttonStatus ();
    void newIndex ();
    void openIndex ();
    void saveIndex ();
    void okPressed ();
          
  private:
    bool saveFlag;
    QString symbol;
    QString file;
    QString dataPath;
    QListView *list;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *newButton;
    QPushButton *openButton;
    QPushButton *saveButton;
    QLabel *name;
    QDict<QString> symbolDict;
};

