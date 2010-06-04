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

#include <QDialog>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QStringList>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QPushButton>

#include "FileButton.h"

class CSVRuleDialog : public QDialog
{
  Q_OBJECT
  
  public:
    CSVRuleDialog (QString &);
    void createGUI ();
    void loadRule ();
    void saveRule ();
    void clear ();
    void dateDialog (QString &);
    void timeDialog (QString &);
    
  public slots:
    void ruleChanged ();
    void addClicked ();
    void insertClicked ();
    void deleteClicked ();
    void done ();
          
  private:
    QDialogButtonBox *_buttonBox;
    QListWidget *_ruleList;
    QComboBox *_type;
    QComboBox *_delimeter;
    FileButton *_file;
    QCheckBox *_fileSymbol;
    QComboBox *_exchange;
    bool _saveFlag;
    QString _name;
    QPushButton *_addButton;
    QPushButton *_insertButton;
    QPushButton *_deleteButton;
    QStringList _fieldList;
};

#endif

