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

#ifndef CSV_RULE_DIALOG_HPP
#define CSV_RULE_DIALOG_HPP

#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QStringList>
#include <QCheckBox>
#include <QPushButton>

#include "FileButton.h"
#include "Dialog.h"

class CSVRuleDialog : public Dialog
{
  Q_OBJECT
  
  public:
    CSVRuleDialog (QString &);
    void createMainPage ();
    void createRulePage ();
    void loadRule ();
    void saveRule ();
    void clear ();
    
  public slots:
    void ruleChanged ();
    void insertClicked ();
    void deleteClicked ();
    void done ();
    void searchExchange ();
    void setExchangeCode (QString);
    void dateDialog2 (QString);
    void timeDialog2 (QString);
    void ruleSelectionChanged ();
    void fieldSelectionChanged ();
    void dateDialog ();
    void timeDialog ();
          
  private:
    QListWidget *_ruleList;
    QListWidget *_fieldList;
    QComboBox *_type;
    QComboBox *_delimeter;
    FileButton *_file;
    QComboBox *_exchange;
    bool _saveFlag;
    QString _name;
    QPushButton *_insertButton;
    QPushButton *_insertDateButton;
    QPushButton *_insertTimeButton;
    QPushButton *_deleteButton;
    QStringList _fields;
    QCheckBox *_removeSuffix;
    QCheckBox *_fileSymbol;
};

#endif

