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

#include "PlotCursorNormal.h"
#include "COPlugin.h"
#include "PlotCursorInfo.h"

#include <QList>
#include <QPoint>
#include <QCursor>

PlotCursorNormal::PlotCursorNormal ()
{
  _type = "Normal";
}

int PlotCursorNormal::getCursor ()
{
  return (int) Qt::ArrowCursor;
}

void PlotCursorNormal::mousePress (PlotData &pd, DateBar &, Scaler &, Indicator &ind)
{
  QList<int> keyList;
  ind.coKeys(keyList);
  
  int loop = 0;
  for (; loop < keyList.count(); loop++)
  {
    COPlugin *co = ind.chartObject(keyList.at(loop));
    
    QPoint p(pd.x, pd.y);
    
    if (co->isSelected(p))
    {
      emit signalCOSelected(keyList.at(loop));
      return;
    }
  }
}

// center chart on double click mouse position
void PlotCursorNormal::mouseDoubleClick (PlotData &pd, DateBar &, Scaler &)
{
  int center = ((pd.buffer.width() - pd.scaleWidth) / pd.barSpacing) / 2;
  int i = pd.x / pd.barSpacing;
  if (i < center)
    emit signalIndexChanged(pd.startIndex - (center - i));
  else
  {
    if (i > center)
      emit signalIndexChanged(pd.startIndex + (i - center));
  }
}

void PlotCursorNormal::mouseMove (PlotData &pd, DateBar &dateBars, Scaler &, Indicator &ind)
{
  // update the data panel with new data
  if (! pd.infoFlag)
    return;

  // determine if we are over a chart object, if so update cursor
  QPoint p(pd.x, pd.y);
  
  QList<int> keyList;
  ind.coKeys(keyList);
  
  int loop = 0;
  int flag = 0;
  for (; loop < keyList.count(); loop++)
  {
    COPlugin *co = ind.chartObject(keyList.at(loop));
    if (co->isSelected(p))
    {
      pd.plot->setCursor(QCursor(Qt::PointingHandCursor));
      flag = 1;
      break;
    }
  }
  
  if (! flag)
    pd.plot->setCursor(QCursor(Qt::ArrowCursor));

  PlotCursorInfo info;
  Setting *mess = info.info(p, pd, ind, dateBars);
  if (mess)
    emit signalInfoMessage(mess);
}

