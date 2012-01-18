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

#ifndef ADD_INDICATOR_HPP
#define ADD_INDICATOR_HPP

#include <QObject>

class AddIndicator : public QObject
{
  Q_OBJECT

  public:
    AddIndicator (QObject *p);
    void run ();

  public slots:
    void addIndicator ();
    void addIndicator2 (QString);
    void addIndicator3 (QString command, QString file);
    void done ();
    
  private:
    QString _name;
};

#endif
