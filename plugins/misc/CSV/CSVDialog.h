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

#ifndef CSV_DIALOG_HPP
#define CSV_DIALOG_HPP

#include <QDialog>
#include <QTextEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>

class CSVDialog : public QDialog
{
  Q_OBJECT
  
  public:
    CSVDialog ();
    ~CSVDialog ();
    void createMainPage ();
    void loadSettings ();
    
  public slots:
    void run ();
    void newRule ();
    void editRule ();
    void editRule (QString);
    void deleteRule ();
    void cancelButton ();
          
  private:
    QComboBox *_rules;
    QTextEdit *_log;
    QDialogButtonBox *_buttonBox;
    QPushButton *_newButton;
    QPushButton *_runButton;
    QPushButton *_editButton;
    QPushButton *_deleteButton;
    QPushButton *_cancelButton;
};

#endif
