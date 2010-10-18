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

#ifndef ALERT_CONFIGURE_DIALOG_HPP
#define ALERT_CONFIGURE_DIALOG_HPP

#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>

#include "Dialog.h"
#include "FileButton.h"

class AlertConfigureDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalInterval (int);
    void signalEnable (int);
    
  public:
    AlertConfigureDialog ();
    ~AlertConfigureDialog ();
    void createMainPage ();
    void createMailPage ();
    void loadSettings ();
    void saveSettings ();

  public slots:
    void done ();
    void intervalChanged (int);
    void enableChanged (int);

  private:
    QCheckBox *_enable;
    QSpinBox *_interval;
    FileButton *_sound;
    QLineEdit *_address;
    QLineEdit *_subject;
    QTextEdit *_text;
    int _intervalFlag;
    int _enableFlag;
};

#endif
