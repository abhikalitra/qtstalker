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

#include "PlotCursorZoom.h"
#include "PlotCursorInfo.h"

PlotCursorZoom::PlotCursorZoom ()
{
  _type = "Zoom";
  _rubberBand = 0;
}

PlotCursorZoom::~PlotCursorZoom ()
{
  if (_rubberBand)
    delete _rubberBand;
}

int PlotCursorZoom::getCursor ()
{
  return (int) Qt::ArrowCursor;
}

void PlotCursorZoom::mousePress (PlotData &pd, DateBar &dateBars, Indicator &)
{
  if (! _rubberBand)
    return;

  if (_rubberBand->width() < pd.barSpacing)
    return;

  // calculate new pixel spacing and position here
  int x = convertXToDataIndex(_mouseOrigin.x(), pd, dateBars);
  int ti = _rubberBand->width() / pd.barSpacing;
  ti = pd.plot->width() / ti;

  delete _rubberBand;
  _rubberBand = 0;

  pd.plot->unsetCursor();

  if (ti < pd.barSpacing)
    return;

  emit signalPixelSpaceChanged(x, ti);
}

void PlotCursorZoom::mouseDoubleClick (PlotData &pd, DateBar &, Indicator &)
{
  if (_rubberBand)
    return;

  _rubberBand = new QRubberBand(QRubberBand::Rectangle, pd.plot);
  _mouseOrigin = QPoint(pd.x, pd.y);
  _rubberBand->setGeometry(QRect(_mouseOrigin, QSize()));
  _rubberBand->show();
  pd.plot->setCursor(QCursor(Qt::SizeFDiagCursor));
}

void PlotCursorZoom::mouseMove (PlotData &pd, DateBar &dateBars, Indicator &ind)
{
  int i = convertXToDataIndex(pd.x, pd, dateBars);
  
  PlotCursorInfo info;
  Setting *mess = info.infoXY(i, pd.y, dateBars, ind.scaler());
  if (mess)
    emit signalInfoMessage(mess);

  if (_rubberBand)
  {
    QPoint p(pd.x, pd.y);
    _rubberBand->setGeometry(QRect(_mouseOrigin, p).normalized());
  }
}

