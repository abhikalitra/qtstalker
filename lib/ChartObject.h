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

// *************************************************************************************************
// Base class for chart objects. Buy,Sell etc inherit this.
// *************************************************************************************************

#ifndef CHART_OBJECT_HPP
#define CHART_OBJECT_HPP

#include <QStringList>
#include <QDateTime>
#include <QPoint>
#include <QObject>
#include <QMenu>
#include <qwt_plot.h>

#include "Setting.h"
#include "ChartObjectSettings.h"
#include "ChartObjectDraw.h"

class ChartObject : public QObject
{
  Q_OBJECT

  signals:
    void signalSelected (int);
    void signalUnselected (int);
    void signalDelete (int);
    void signalMoveStart (int);
    void signalMoveEnd (int);
  
  public:
    enum Type
    {
      _Buy,
      _HLine,
      _Retracement,
      _Sell,
      _Text,
      _TLine,
      _VLine
    };

    enum Status
    {
      _None,
      _Selected,
      _Move,
      _Move2
    };

    ChartObject ();
    ~ChartObject ();
    virtual void info (Setting &);
    virtual void load ();
    virtual int CUS (QStringList &);
    virtual int highLow (int start, int end, double &high, double &low);
    virtual void create ();

    void settings (ChartObjectSettings &);
    ChartObject::Status status ();
    int isModified ();
    void setZ (int);
    void attach (QwtPlot *);
    int isSelected (QPoint);
    void setSettings (ChartObjectSettings &);

  public slots:
    virtual void move (QPoint);
    virtual void click (int, QPoint);
    virtual void dialog ();

    void confirmDelete ();
    void deleteChartObject ();

  protected:
    Status _status;
    ChartObjectSettings _settings;
    int _modified;
    QMenu *_menu;
    ChartObjectDraw *_draw;
};

#endif
