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

#ifndef SCRIPT_TIMER_HPP
#define SCRIPT_TIMER_HPP

#include <QTimer>
#include <QString>

class ScriptTimer : public QTimer
{
  Q_OBJECT

  signals:
    void signalStartScript (QString, QString);

  public:
    ScriptTimer (QObject *);
    void setIntervalString (QString);
    void setFile (QString);
    void setName (QString);
    void setCommand (QString);

  public slots:
    void startScript ();

  private:
    QString _file;
    QString _name;
    QString _command;
};

#endif
