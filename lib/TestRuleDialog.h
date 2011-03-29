/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef TEST_RULE_DIALOG_HPP
#define TEST_RULE_DIALOG_HPP

#include <QTreeWidget>
#include <QStringList>

#include "Dialog.h"
#include "Setting.h"

class TestRuleDialog : public Dialog
{
  Q_OBJECT

  public:
    TestRuleDialog (QWidget *, Setting *);
    void createGUI ();
    void loadRule ();

  public slots:
    void done ();
    void addRule ();
    void addRule (QString check, QString val, QString offset, QString op, QString val2, QString offset2);
    void deleteRule ();
    void saveRule ();
        
  private:
    Setting *_settings;
    QTreeWidget *_tree;
};

#endif
