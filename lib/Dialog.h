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

#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QLayout>
#include <QString>
#include <QLabel>
#include <QCheckBox>

class Dialog : public QDialog
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    enum ConfirmStatus
    {
      _ConfirmNone = -1,
      _ConfirmNo = 0,
      _ConfirmYes = 2
    };
    
    Dialog ();
    Dialog (QWidget *parent);
    void createGUI ();
    void setMessage (QString);
    void setMessage2 (QString);
    void setConfirm (QString message, QString check);
    void unsetConfirm ();
    void setIcon (QPixmap);
    void setFontBold (int);

  public slots:
    virtual void done ();
    void confirmChanged (int);

  protected:
    QTabWidget *_tabs;
    QDialogButtonBox *_buttonBox;
    QVBoxLayout *_vbox;
    QLabel *_message;
    QLabel *_icon;
    QCheckBox *_confirm;
    ConfirmStatus _confirmFlag;
};

#endif
