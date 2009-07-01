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

#ifndef CSVRULEDIALOG_H
#define CSVRULEDIALOG_H



#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QStringList>
#include <QListWidget>
#include <QPushButton>
#include <QString>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>



class CSVRuleDialog : public QDialog
{
  Q_OBJECT
  
  public:
    CSVRuleDialog (QWidget *, QString &);
    ~CSVRuleDialog ();
    void createRulePage ();
    void loadRule ();
    
  public slots:
    void saveRule ();
    void insertField ();
    void deleteField ();
    void ruleFieldSelected ();
    void fieldListSelected ();
    void importFileDialog ();
          
  private:
    QDialogButtonBox *buttonBox;
    QListWidget *ruleList;
    QListWidget *fieldList;
    QLineEdit *delimiter;
    QString rule;
    QPushButton *insertButton;
    QPushButton *deleteButton;
    QCheckBox *useFileName;
    QLineEdit *dateFormat;
    QPushButton *fileButton;
    QSpinBox *seconds;
    QStringList fileList;
};

#endif

