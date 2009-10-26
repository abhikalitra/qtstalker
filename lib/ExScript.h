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

#include "BarData.h"
#include "IndicatorScript.h"

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
      INDICATOR, // get a library indicator
      GET_INDICATOR, // pass indicator data to the calling script
      SET_INDICATOR, // pass indicator data from script to qtstalker
      PLOT // plot the desired indicator
    };

    ExScript ();
    ~ExScript ();
    void clear ();
    void calculate (QString &command);
    void getLines (QList<PlotLine *> &);
    void setInput (BarData *bd);

  public slots:
    void readFromStdout ();
    void readFromStderr ();
    void done (int, QProcess::ExitStatus);
    
  private:
    QProcess *proc;
    BarData *data;
    QStringList functionList;
    IndicatorScript *is;
};

