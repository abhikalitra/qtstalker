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

#ifndef INDICATORPAGE_HPP
#define INDICATORPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include "Setting.h"
#include "Macro.h"
#include "MacroKey.h"
#include "MyComboBox.h"
#include "MyListBox.h"


class IndicatorPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalDisableIndicator (QString);
    void signalEnableIndicator (QString);
    void signalEditIndicator (Setting *);
    void signalDeleteIndicator (QString);
    void signalNewIndicator (Setting *);
    void signalKeyPressed (int, int, int, int, QString);
    void signalReloadChart ();
  
  public:
  
    enum HotKey
    {
      NewIndicatorGroup,
      DeleteIndicatorGroup,
      NewIndicator,
      DeleteIndicator,
      EditIndicator,
      MoveIndicator,
      Help,
      Tab
    };
  
    IndicatorPage (QWidget *);
    ~IndicatorPage ();
    void updateGroups ();
    void getIndicatorGroups (QStringList &);
    QString getIndicatorGroup ();
    void setFocus ();
    void setChartPath (QString);
    void removeLocalIndicators ();
    void addLocalIndicators (QString);
    void saveLocalIndicator (QString d, Setting &);

  public slots:
    void doubleClick (QListBoxItem *);
    void updateList ();
    void slotHelp ();
    void rightClick (QListBoxItem *);
    void newIndicator ();
    void editIndicator (QString);
    void editIndicator ();
    void deleteIndicator ();
    void itemSelected (const QString &);
    void changeIndicator (QString);
    void setKeyFlag (bool);
    void slotAccel (int);
    void doKeyPress (QKeyEvent *key);
    void runMacro (Macro *);
    void moveIndicator ();
    void slotGroupChanged (int);
    void newIndicatorGroup ();
    void deleteIndicatorGroup ();

  protected:
    virtual void keyPressEvent (QKeyEvent *);
    
    MyListBox *list;
    MyComboBox *group;
    QPopupMenu *menu;
    bool keyFlag;
    Macro *macro;
    bool macroFlag;
    QString baseDir;
    QString cusRulePath;
    QString currentGroup;
    bool updateEnableFlag;
    QString chartPath;
    QStringList localIndicators;
};

#endif
