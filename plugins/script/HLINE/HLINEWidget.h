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

#ifndef PLUGIN_HLINE_WIDGET_HPP
#define PLUGIN_HLINE_WIDGET_HPP

#include <QTreeWidget>
#include <QToolButton>

#include "PluginWidget.h"

class HLINEWidget : public PluginWidget
{
  Q_OBJECT

  public:
    HLINEWidget (QWidget *);
    void createGeneralPage();
    void loadSettings();

  public slots:
    void setCommand (QString);
    void commands (QStringList &, int);
    void selectionChanged ();
    void deleteItem ();
    void addItem ();
    void addItem (QString, QString, QString);

  private:
    QTreeWidget *_tree;
    QToolButton *_deleteButton;
};

#endif
