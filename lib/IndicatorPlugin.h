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

#ifndef INDICATOR_PLUGIN_HPP
#define INDICATOR_PLUGIN_HPP

#include <QString>
#include <QStringList>
#include <QHash>
#include <QColor>

#include "BarData.h"
#include "PlotLine.h"
#include "Indicator.h"
#include "Setting.h"
#include "PrefDialog.h"

class IndicatorPlugin
{
  public:
    IndicatorPlugin ();
    virtual ~IndicatorPlugin ();
    virtual int getIndicator (Indicator &, BarData *);
    virtual int getCUS (QStringList &, QHash<QString, PlotLine *> &, BarData *);
    virtual int dialog (int);
    virtual void getMethodList (QStringList &);
    virtual PlotLine * getMA (PlotLine *, int, int);
    virtual PlotLine * getBARS (BarData *, QColor &, QColor &, QColor &);

    void setSettings (Indicator &);
    void getSettings (Indicator &);
    void getIndicator (QString &);
    void getDialogSettings (PrefDialog *);
    int getDeleteFlag ();
    void getPlotList (QStringList &, int);
    PlotLine * getLocalMA (PlotLine *, int, int);
    PlotLine * getLocalBARS (BarData *, QColor &, QColor &, QColor &);
    void getMAList (QStringList &);

  protected:
    Setting settings;
    QString name;
    QString indicator;
    QStringList opList;
    QStringList inputList;
    QStringList plotList;
    int deleteFlag;
};

#endif
