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

#ifndef DATE_RANGE_CONTROL_HPP
#define DATE_RANGE_CONTROL_HPP

#include <QToolButton>
#include <QMenu>
#include <QStringList>

class DateRangeControl : public QToolButton
{
  Q_OBJECT

//  signals:
//    void signalDateRangeChanged ();

  public:
    DateRangeControl ();
    void createMenu ();
    int dateRange ();

  public slots:
    void rangeChanged (QAction *);

  private:
    int _dateRange;
    QMenu *_menu;
};

#endif
