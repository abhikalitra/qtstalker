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

#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <QStringList>
#include <QHash>
#include <QObject>

#include "Curve.h"
#include "ChartObject.h"
#include "SettingGroup.h"

class Script : public QObject
{
  Q_OBJECT

  public:
    Script (QObject *);
    ~Script ();
    void clear ();
    void setCurve (QString key, Curve *);
    Curve * curve (QString);
    void setCurves (QHash<QString, Curve *>);
    QHash<QString, Curve *> curves ();
    void setSettingGroup (SettingGroup *);
    SettingGroup * settingGroup (QString);
    Setting * setting (QString);
    int chartObjectCount ();
    void setChartObject (QString key, ChartObject *);
    QHash<QString, ChartObject *> chartObjects ();
    void setSession (QString);
    QString session ();
    void setName (QString);
    QString name ();
    void removeSettingGroup (QString);
    void setFile (QString);
    QString & file ();
    qint64 pid ();
    void setCurrentStep (QString);
    QString currentStep ();
    QString nextROID ();

  public slots:
    int run ();
    int runWait ();
    int kill ();

  protected:
    qint64 _pid;
    QString _session;
    QString _name;
    QString _file;
    QString _currentStep;
    QHash<QString, Curve *> _curves;
    QHash<QString, ChartObject *> _chartObjects;
    QHash<QString, SettingGroup *> _settingGroups;
};

#endif
