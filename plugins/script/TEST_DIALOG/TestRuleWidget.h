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

#ifndef PLUGIN_TEST_RULE_WIDGET_HPP
#define PLUGIN_TEST_RULE_WIDGET_HPP

#include <QListWidget>
#include <QStringList>
#include <QToolButton>

#include "Indicator.h"

class TestRuleWidget : public QWidget
{
  Q_OBJECT

  public:
    TestRuleWidget ();
    ~TestRuleWidget ();
    void createGUI ();
    int load (QString name, QString prefix);
    int save (QString name, QString prefix);
    void clear ();

  public slots:
    void addIndicator ();
    void addIndicator2 (QString);
    void deleteIndicator ();
    void editIndicator ();
    void editIndicatorRules ();
    void listSelected ();
        
  private:
    QListWidget *_list;
    QList<Indicator *> _iList;
    QList<Setting *> _ruleList;
    QToolButton *_editButton;
    QToolButton *_editRuleButton;
    QToolButton *_deleteButton;
};

#endif