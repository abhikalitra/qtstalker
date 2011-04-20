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

#ifndef PLUGIN_PDI_WIDGET_HPP
#define PLUGIN_PDI_WIDGET_HPP

#include <QWidget>
#include <QSpinBox>

#include "PluginWidget.h"
#include "LineEdit.h"

class PDIWidget : public PluginWidget
{
  Q_OBJECT

  public:
    PDIWidget (QWidget *);
    void createGeneralPage ();

  public slots:
    void setCommand (QString);
    void commands (QStringList &, int);

  private:
    LineEdit *_output;
    LineEdit *_hinput;
    LineEdit *_linput;
    LineEdit *_cinput;
    QSpinBox *_period;
};

#endif