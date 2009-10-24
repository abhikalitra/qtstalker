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

#ifndef INDICATORDIALOG_HPP
#define INDICATORDIALOG_HPP


#include <QString>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QToolButton>



class IndicatorDialog : public QDialog
{
  Q_OBJECT

  public:
    IndicatorDialog (QWidget *);
    void setName (QString &);
    void setTabRow (QString &);
    void getTabRow (QString &);
    void setCommand (QString &);
    void getCommand (QString &);
    void getFile (QString &);
    void setFile (QString &);
    
  public slots:
    void slotPathButtonPressed ();

  private:
    QDialogButtonBox *buttonBox;
    QLabel *name;
    QLineEdit *scriptCommand;
    QSpinBox *tabRow;
    QLabel *path;
    QToolButton *pathButton;
};

#endif
