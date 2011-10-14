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

#ifndef VERIFY_DATA_INPUT_HPP
#define VERIFY_DATA_INPUT_HPP

#include "Script.h"
#include "Data.h"

#include <QStringList>
#include <QColor>
#include <QDateTime>
#include <QFont>
#include <QList>

class VerifyDataInput
{
  public:
    VerifyDataInput ();
    int toString (Script *script, QString key, QString &data);
    int toInteger (Script *script, QString key, int &data);
    int toDouble (Script *script, QString key, double &data);
    int toBool (Script *script, QString key, bool &data);
    int toColor (Script *script, QString key, QColor &data);
    int toBarLength (Script *script, QString key, int &data);
    int toDateRange (Script *script, QString key, int &data);
    int toDateTime (Script *script, QString key, QDateTime &data);
    int toFile (Script *script, QString key, QStringList &data);
    int toFont (Script *script, QString key, QFont &data);
    int toMA (Script *script, QString key, int &data);
    int toOp (Script *script, QString key, int &data);
    int toSymbol (Script *script, QString key, QStringList &data);
    int toList (Script *script, QString key, QString &data);
    Data * toCurve (Script *script, QString key);
    int curveKeys (QList<Data *> &list, QList<int> &keys);
    int curveValue (Data *in, QList<int> &keys, int index, int offset, double &v);
};

#endif
