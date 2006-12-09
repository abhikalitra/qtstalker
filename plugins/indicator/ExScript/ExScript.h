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

#include "IndicatorPlugin.h"
#include <qprocess.h>
#include <qtimer.h>

class ExScript : public IndicatorPlugin
{
  Q_OBJECT
  
  public:
    ExScript ();
    virtual ~ExScript ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    PlotLine * calculateCustom (QString &, QPtrList<PlotLine> &);
    void formatDialog (QStringList &vl, QString &rv, QString &rs);
    void createOutput ();
    void getInput (QString &);

  public slots:
    void readFromStdout ();
    void timerDone ();
    
  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    QString scriptPath;
    QString comlineParms;
    QProcess *proc;
    QString buffer;
    QTimer *timer;
    int seconds;

    QString colorLabel;
    QString labelLabel;
    QString lineTypeLabel;
    QString scriptPathLabel;
    QString comlineParmsLabel;
    QString pluginLabel;
    QString dateLabel;
    QString openLabel;
    QString highLabel;
    QString lowLabel;
    QString closeLabel;
    QString volumeLabel;
    QString oiLabel;
    QString timeoutLabel;

    bool dateFlag;
    bool openFlag;
    bool highFlag;
    bool lowFlag;
    bool closeFlag;
    bool volumeFlag;
    bool oiFlag;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}
