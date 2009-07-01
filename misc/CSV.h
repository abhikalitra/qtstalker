/*
 *  CSV stock quote importer
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

#ifndef CSV_H
#define CSV_H


#include "CSVRule.h"
#include "Bar.h"
#include <QStringList>
#include <QString>
#include <QObject>
#include <QTimer>
#include <QDateTime>



class CSV : public QObject
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalActive (QString);
    void signalInactive (QString);

  public:
    CSV (CSVRule &rule);
    void setChart (QList<Bar> *bars);
    void importSingleShot ();
    void status ();

  public slots:
    void import ();

  private:
    int interval;
    QTimer *timer;
    CSVRule rule;
};

#endif

