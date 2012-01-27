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

#ifndef WIDGET_LINE_EDIT_HPP
#define WIDGET_LINE_EDIT_HPP

#include <QStringList>
#include <QComboBox>
#include <QLineEdit>

class WidgetLineEdit : public QComboBox
{
  Q_OBJECT

  signals:
    void signalStatus (bool);

  public:
    WidgetLineEdit (QWidget *);
    void createGUI ();
    void setItems (QStringList);
    void setText (QString);
    QString text ();

  public slots:
    void textChanged (const QString &);

  private:
    QStringList _items;
    QLineEdit *_edit;
};

#endif
