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

#ifndef FORMULAEDIT_HPP
#define FORMULAEDIT_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qtable.h>
#include "Toolbar.h"
#include "Config.h"

class FormulaEdit : public QWidget
{
  Q_OBJECT

  public:
  
    enum FormulaEditType
    {
      Indicator,
      Logic
    };
  
    FormulaEdit (QWidget *, int);
    ~FormulaEdit ();
    void setLine (QString &);
    QString getLine (int);
    int getLines ();
    bool checkError ();

  public slots:
    void addItem ();
    void editItem ();
    void deleteItem ();
    void insertItem ();
    void openRule ();
    void saveRule ();
    void slotDoubleClicked(int, int, int, const QPoint &);

  protected:
    QTable *list;
    Toolbar *toolbar;
    QStringList functionList;
    Config config;
    FormulaEditType type;
};

#endif
