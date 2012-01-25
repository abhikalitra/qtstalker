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

#ifndef DATA_DIALOG_HPP
#define DATA_DIALOG_HPP

#include <QStringList>
#include <QColor>
#include <QFormLayout>
#include <QHash>
#include <QTabWidget>

#include "Dialog.h"

class DataDialog : public Dialog
{
  Q_OBJECT
  
  public:
    DataDialog (QWidget *, QString id, Entity);
    void createGUI ();
    void setSettings (Entity);
    Entity settings ();
    void setGUI ();
    void addTab (int pos);
    int setText (int tab, QString key, QString label, QString text, QString tt);
    int setColor (int tab, QString key, QString label, QColor c, QString tt);
    int setInteger (int tab, QString key, QString label, int v, int h, int l, QString tt);
    int setDouble (int tab, QString key, QString label, double v, double h, double l, QString tt);
    int setBool (int tab, QString key, QString label, bool v, QString tt);
    int setList (int tab, QString key, QString label, QString v, QStringList l, QString tt);
    int setDateTime (int tab, QString key, QString label, QDateTime v, QString tt);
    int setFile (int tab, QString key, QString label, QStringList v, QString tt);
    int setSymbol (int tab, QString key, QString label, QStringList v, QString tt);
    void setTabTitle (int pos, QString);
    void setSettings (Entity *);

  public slots:
    void done ();

  protected:
    QHash<int, QFormLayout *> _formList;
    QHash<QString, void *> _widgets;
    QTabWidget *_tabs;
};

#endif
