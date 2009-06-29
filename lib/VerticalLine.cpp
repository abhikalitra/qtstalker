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

#include "VerticalLine.h"
#include "PrefDialog.h"
#include "DataBase.h"
#include "Config.h"
#include <QPainter>
#include <QPolygon>



VerticalLine::VerticalLine ()
{
  color.setNamedColor("red");
  helpFile = "verticalline.html";
  type = (int) COVerticalLine;
  
  loadDefaults();
}

VerticalLine::~VerticalLine ()
{
}

void VerticalLine::draw (QPixmap &buffer, Scaler &, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  int x2 = data->getX(date);
  if (x2 == -1)
    return;
      
  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;
      
  painter.setPen(color);

  painter.drawLine (x, 0, x, buffer.height());
  
  clearSelectionArea();
  QPolygon array;
  array.putPoints(0,
  	          4,
		  x - (HANDLE_WIDTH / 2), 0,
		  x + (HANDLE_WIDTH / 2), 0,
		  x + (HANDLE_WIDTH / 2), buffer.height(),
		  x - (HANDLE_WIDTH / 2), buffer.height());
  setSelectionArea(new QRegion(array));
    
  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
    
    int t = (int) buffer.height() / 4;
    
    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  0,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), 0, HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
            	  t,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t, HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
            	  t * 2,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t * 2, HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  t * 3,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t * 3, HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  t * 4,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t * 4, HANDLE_WIDTH, HANDLE_WIDTH, color);
  }

  painter.end();
}

void VerticalLine::prefDialog ()
{
  QString cl = tr("Color");
  QString sd = tr("Set Default");

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Edit VerticalLine"));
  dialog->addColorPrefItem(cl, color);
  dialog->addCheckItem(sd, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    saveFlag = TRUE;
    
    bool f = dialog->getCheck(sd);
    if (f)
      saveDefaults();
    
    emit signalDraw();
  }
  
  delete dialog;
}

void VerticalLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  id = n;
  status = ClickWait;
  emit message(tr("Select point to place VerticalLine..."));
}

COBase::Status VerticalLine::pointerClick (QPoint &point, QDateTime &x, double)
{
  switch (status)
  {
    case None:
      if (isSelected(point))
      {
        status = Selected;
        emit signalDraw();
      }
      break;
    case Selected:
      if (isGrabSelected(point))
        status = Moving;
      else
      {
        if (! isSelected(point))
        {
          status = None;
          emit signalDraw();
        }
      }
      break;
    case Moving:
      status = Selected;
      break;
    case ClickWait:
      date = x;
      setSaveFlag(TRUE);
      emit signalDraw();
      status = None;
      emit message("");
      break;
    default:
      break;
  }
    
  return status;    
}

void VerticalLine::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double)
{
  if (status != Moving)
    return;
    
  date = x;
  setSaveFlag(TRUE);
  emit signalDraw();
  QString s = x.toString(dateFormat);
  emit message(s);
}

void VerticalLine::loadDefaults ()
{
  Config config;
  config.getData(Config::DefaultVerticalLineColor, color);
}

void VerticalLine::saveDefaults ()
{
  Config config;
  config.setData(Config::DefaultVerticalLineColor, color);
}

double VerticalLine::getHigh ()
{
  return data->getMax();
}

double VerticalLine::getLow ()
{
  return data->getMin();
}

void VerticalLine::saveSettings ()
{
  COSettings co(id, symbol, indicator, QString::number(type));
  co.setDate(date);
  co.setColor(color);

  DataBase db;
  db.setChartObject(co);
}

void VerticalLine::loadSettings (COSettings &co)
{
  co.getSymbol(symbol);
  co.getID(id);
  co.getIndicator(indicator);
  co.getDate(date);
  co.getColor(color);
}

