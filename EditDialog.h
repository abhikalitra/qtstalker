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

#ifndef EDITDIALOG_HPP
#define EDITDIALOG_HPP

#include <qdialog.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qdir.h>
#include <qgroupbox.h>

#include "Setting.h"
#include "Config.h"

class EditDialog : public QDialog
{
  Q_OBJECT

  public:
    EditDialog (Config *);
    ~EditDialog ();
    void setItems (Setting *);
    void makeSettings ();
    void colorDialog ();
    void dateDialog ();
    void textDialog ();
    void intDialog ();
    void floatDialog ();
    void fileDialog ();
    void symbolDialog ();
    void checkDialog ();
    void listDialog ();
    void updateFileList ();
    void clearFileInfo ();
    void setFileInfo ();

  public slots:
    void cellSelected (QListViewItem *);
    void saveData ();
    void upDirectory ();
    void fileSelection (QListViewItem *);

  protected:
    Config *config;
    Setting *settings;
    QVBoxLayout *baseBox;
    QVBoxLayout *topBox;
    QHBoxLayout *fileBox;
    QGroupBox *gbox;
    QListView *list;
    QListView *fileList;
    QListViewItem *item;
    QToolButton *okButton;
    QToolButton *cancelButton;
    QToolButton *upButton;
    QGridLayout *toolbar;
    QLabel *symbol;
    QLabel *title;
    QLabel *type;
    QLabel *firstDate;
    QLabel *lastDate;
    QDir currentDir;
};

class DateDialog : public QDialog
{
  Q_OBJECT

  public:
    DateDialog ();
    ~DateDialog ();
    void setDate (QDate);
    QDate getDate ();

  private:
    QDateEdit *date;
};

#endif
