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

#ifndef NEWINDICATORDIALOG_HPP
#define NEWINDICATORDIALOG_HPP

#include <qdialog.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qstring.h>
#include "Config.h"

class NewIndicatorDialog : public QDialog
{
  Q_OBJECT

  public:
    NewIndicatorDialog (Config *);
    ~NewIndicatorDialog ();
    QString getIndicator ();
    QString getName ();
    bool getTabFlag ();

  public slots:
    void check ();

  protected:
    Config *config;
    QCheckBox *tabFlag;
    QLineEdit *name;
    QComboBox *indicator;
};

#endif
