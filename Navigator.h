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

#ifndef NAVIGATOR_HPP
#define NAVIGATOR_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qdir.h>

class Navigator : public QWidget
{
  Q_OBJECT

  signals:
    void doubleClick (QString);
    void fileSelected (QString);

  public:
    Navigator (QWidget *, QString);
    ~Navigator ();
    void updateList ();
    void clearFileInfo ();
    void setFileInfo ();
    QString getFileSelection ();
    void setDirectory (QString);

  public slots:
    void upDirectory ();
    void fileSelection (QListViewItem *);
    void slotDoubleClicked ();

  protected:
    QString basePath;
    QListView *list;
    QListViewItem *item;
    QLabel *symbol;
    QLabel *title;
    QLabel *type;
    QLabel *firstDate;
    QLabel *lastDate;
    QDir currentDir;
};

#endif
