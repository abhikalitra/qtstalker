/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#ifndef INDICATORPLUGIN_HPP
#define INDICATORPLUGIN_HPP

#include "PlotLine.h"
#include "Setting.h"
#include "BarData.h"
#include "Indicator.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qdict.h>
#include <qvaluelist.h>
#include <qobject.h>

class IndicatorPlugin : public QObject
{
  Q_OBJECT

  signals:
    void signalWakeup ();
  
  public:
  
    enum FormatType
    {
      FormatInputArray,
      FormatInputArray2,
      FormatInteger,
      FormatDouble,
      FormatString,
      FormatMAType,
      FormatBool
    };

    enum Operator
    {
      Equal,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual,
      And,
      Or
    };

    IndicatorPlugin();
    virtual ~IndicatorPlugin();
    void setIndicatorInput (BarData *);
    Indicator * getIndicator ();
    void clearOutput ();
    void loadFile (QString &, Setting &);
    void saveFile (QString &, Setting &);
    void getPluginName (QString &);
    void getHelpFile (QString &);
    IndicatorPlugin::Operator getOperator (QString &);
    PlotLine * getWilderMA (PlotLine *d, int);
    void getMATypes (QStringList &);
    bool checkFormat (QString &, QPtrList<PlotLine> &, int, int);

    virtual void calculate ();
    virtual int indicatorPrefDialog (QWidget *);
    virtual PlotLine * calculateCustom (QString &, QPtrList<PlotLine> &);
    virtual void getIndicatorSettings (Setting &);
    virtual void setIndicatorSettings (Setting &);
    virtual void setCustomFunction (QStringList &);
    virtual void loadIndicatorSettings (QString &);
    virtual void saveIndicatorSettings (QString &);
    virtual PlotLine * getMA (PlotLine *d, int, int);
    virtual void formatDialog (QStringList &, QString &, QString &);

  public slots:
    void wakeup ();
    
  protected:
    BarData *data;
    Indicator *output;
    QStringList lineTypes;
    QStringList inputTypeList;
    QStringList opList;
    QStringList maList;
    bool saveFlag;
    QString pluginName;
    QString helpFile;
    QValueList<FormatType> formatList;
    QStringList formatStringList;
};

#endif

