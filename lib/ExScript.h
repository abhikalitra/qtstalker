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

#ifndef EXSCRIPT_HPP
#define EXSCRIPT_HPP


#include "BarData.h"
#include "PlotLine.h"

#include <QProcess>
#include <QList>
#include <QString>
#include <QStringList>
#include <QObject>


class ExScript : public QObject
{
  Q_OBJECT

  signals:
    void signalDone ();

  public:
    enum Function
    {
      CLEAR, // clears all data
      INDICATOR, // get a library indicator
      GET_INDICATOR, // pass indicator data to the calling script
      GET_INDICATOR_INDEX, // pass a single indicator value to the calling script
      GROUP_ADD, // add a group item to a group
      SET_INDICATOR, // pass indicator data from script to qtstalker
      PLOT, // plot the desired indicator
      SYMBOL_LIST // get a list of symbols in the db
    };

    ExScript ();
    ~ExScript ();
    void clear ();
    void setBarData (BarData *d);
    int calculate (QString &command);
    void getLines (QList<PlotLine *> &);

  public slots:
    void readFromStdout ();
    void readFromStderr ();
    void done (int, QProcess::ExitStatus);

  private:
    QProcess *proc;
    QStringList functionList;
    QHash<QString, PlotLine *> tlines;
    QStringList plotOrder;
    BarData *data;
    QStringList inputList;
};

#endif
