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
      INDICATOR_GET, // pass indicator data to the calling script
      INDICATOR_GET_INDEX, // pass a single indicator value to the calling script
      INDICATOR_GET_SIZE, // returns the size of the indicator.
      INDICATOR_SET, // pass indicator data from script to qtstalker
      GROUP_ADD, // add a group item to a group
      GROUP_DELETE, // deletes a group and contents
      GROUP_GET, // returns a csv list of the group contents
      PLOT, // plot the desired indicator
      QUOTE, // quote database functions
      SYMBOL_GET, // get the current symbol
      SYMBOL_LIST, // get a list of symbols in the db
      TEST_ENTER_LONG, // sets the enter long indicator for the tester
      TEST_EXIT_LONG,  // sets the exit long indicator for the tester
      TEST_ENTER_SHORT,  // sets the enter short indicator for the tester
      TEST_EXIT_SHORT  // sets the exit short indicator for the tester
    };

    ExScript (QString &, QString &);
    ~ExScript ();
    void clear ();
    void setBarData (BarData *d);
    int calculate (QString &command);
    int calculate2 (QString &command); // used for non indicator scripts
    void getLines (QList<PlotLine *> &);
    int getState ();
    void stop ();
    void setDeleteFlag (int);
    PlotLine * getEnterLong ();
    PlotLine * getExitLong ();
    PlotLine * getEnterShort ();
    PlotLine * getExitShort ();

  public slots:
    void readFromStdout ();
    void readFromStderr ();
    void done (int, QProcess::ExitStatus);

  private:
    QProcess *proc;
    QStringList functionList;
    QStringList notPluginList;
    QHash<QString, PlotLine *> tlines;
    QStringList plotOrder;
    BarData *data;
    QStringList inputList;
    int deleteFlag;
    PlotLine * enterLong;
    PlotLine * exitLong;
    PlotLine * enterShort;
    PlotLine * exitShort;
    int killFlag;
    QString indicatorPluginPath;
    QString dbPluginPath;
};

#endif
