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

#ifndef PP_HPP
#define PP_HPP

#include "IndicatorBase.h"

class PP : public IndicatorBase
{
  public:
    PP ();
    int getIndicator (Indicator &ind, BarData *data);
    int getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data);
    PlotLine * getPP (BarData *data, int point);
    int dialog ();

  protected:
    QString r1cKey;
    QString r2cKey;
    QString r3cKey;
    QString s1cKey;
    QString s2cKey;
    QString s3cKey;
    QString r1lKey;
    QString r2lKey;
    QString r3lKey;
    QString s1lKey;
    QString s2lKey;
    QString s3lKey;
    QString r1ShowKey;
    QString r2ShowKey;
    QString r3ShowKey;
    QString s1ShowKey;
    QString s2ShowKey;
    QString s3ShowKey;
};

#endif
