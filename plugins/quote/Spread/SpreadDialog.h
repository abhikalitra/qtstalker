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

#include "SymbolButton.h"
#include "Toolbar.h"
#include <qcombobox.h>
#include <qtabdialog.h>
#include <qstring.h>
#include <qlineedit.h>

class SpreadDialog : public QTabDialog
{
  Q_OBJECT
  
  public:
    SpreadDialog ();
    ~SpreadDialog ();
    void saveData ();
    
  public slots:
    void buttonStatus ();
    void newSpread ();
    void openSpread ();
    void saveSpread ();
    void dataChanged ();
    void methodChanged (int);
          
  private:
    bool saveFlag;
    QString file;
    QString symbol;
    SymbolButton *firstButton;
    SymbolButton *secondButton;
    QComboBox *method;
    QLineEdit *name;
    QString dataPath;
    Toolbar *toolbar;
};

