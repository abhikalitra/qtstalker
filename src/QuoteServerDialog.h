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

#ifndef QUOTE_SERVER_DIALOG_HPP
#define QUOTE_SERVER_DIALOG_HPP

#include <QLineEdit>
#include <QSpinBox>
#include <QString>
#include <QPushButton>

#include "Dialog.h"

class QuoteServerDialog : public Dialog
{
  Q_OBJECT
  
  public:
    QuoteServerDialog ();
    void createMainPage ();

  public slots:
    void done ();
    void refreshServer ();
    void refreshServer2 ();

  private:
    QLineEdit *_hostName;
    QSpinBox *_port;
    int _oport;
    QString _ohostName;
    QPushButton *_refreshButton;
};

#endif
