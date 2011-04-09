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

#ifndef RULE_WIDGET_HPP
#define RULE_WIDGET_HPP

#include <QTreeWidget>
#include <QList>
#include <QToolButton>

#include "Setting.h"

class RuleWidget : public QWidget
{
  Q_OBJECT

  public:
    enum Type
    {
      _OUTPUT,
      _INPUT,
      _TEXT,
      _INTEGER,
      _DOUBLE,
      _COLOR,
      _OPERATOR,
      _MA,
      _CANDLE,
      _STYLE,
      _PLOT,
      _LIST
    };
    
    RuleWidget (QWidget *, QString);
    void createGeneralPage();
    void setRules (Setting *, QList<int>, QStringList);
    void loadSettings ();
    void addRule (QStringList &);
    void setList (QStringList);

  public slots:
    void save ();
    void addRule ();
    void deleteRule ();
    void selectionChanged ();

  private:
    Setting *_settings;
    QTreeWidget *_plist;
    QList<int> _format;
    QString _saveKey;
    QStringList _list;
    QToolButton *_deleteButton;
};

#endif
