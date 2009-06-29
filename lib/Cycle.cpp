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

#include "Cycle.h"
#include "PrefDialog.h"
#include "DataBase.h"
#include "Config.h"
#include <QPainter>
#include <QPolygon>



Cycle::Cycle ()
{
  color.setNamedColor("red");
  helpFile = "cycle.html";
  interval = 10;
  grabPosition = -1;
  type = (int) COCycle;
  
  loadDefaults();
}

Cycle::~Cycle ()
{
}

void Cycle::draw (QPixmap &buffer, Scaler &, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  int x2 = data->getX(date);
  if (x2 == -1)
    return;
      
  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;
      
  tpixelspace = pixelspace; // save this for adjusting the cycle interval
    
  int origx = x;
      
  painter.setPen(color);
  clearSelectionArea();
    
  while (x <= buffer.width())
  {
    if ((x + (interval * pixelspace)) > 0)
    {
      painter.drawArc(x,
                      buffer.height() - ((interval * 4) / 2) - 2,
	              interval * pixelspace,
		      interval * 4,
		      16 * 180,
		      16 * -180);
      
      QPolygon array;
      array.putPoints(0,
  		      4,
		      x - (HANDLE_WIDTH / 2), buffer.height(),
		      x - (HANDLE_WIDTH / 2), buffer.height() - HANDLE_WIDTH,
		      x + (HANDLE_WIDTH / 2), buffer.height() - HANDLE_WIDTH,
		      x + (HANDLE_WIDTH / 2), buffer.height());
      setSelectionArea(new QRegion(array));
    }
      
    x = x + (interval * pixelspace);
  }
    
  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
      
    x = origx;
      
    while (x <= buffer.width())
    {
      if ((x + (interval * pixelspace)) > 0)
      {
        setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			  buffer.height() - HANDLE_WIDTH,
				  HANDLE_WIDTH,
				  HANDLE_WIDTH,
				  QRegion::Rectangle));
					
        painter.fillRect(x - (HANDLE_WIDTH / 2),
	                 buffer.height() - HANDLE_WIDTH,
			 HANDLE_WIDTH,
			 HANDLE_WIDTH,
			 color);
      }
	
      x = x + (interval * pixelspace);
    }
  }

  painter.end();
}

void Cycle::prefDialog ()
{
  QString cl = tr("Color");
  QString il = tr("Interval");
  QString sd = tr("Set Default");

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Edit Cycle"));
  dialog->addColorPrefItem(cl, color);
  dialog->addIntItem(il, interval, 1, 9999);
  dialog->addCheckItem(sd, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    interval = dialog->getInt(il);
    saveFlag = TRUE;
    
    bool f = dialog->getCheck(sd);
    if (f)
      saveDefaults();
    
    emit signalDraw();
  }
  
  delete dialog;
}

void Cycle::newObject (QString &ind, QString &n)
{
  indicator = ind;
  id = n;
  status = ClickWait;
  emit message(tr("Select starting point of Cycle..."));
}

COBase::Status Cycle::pointerClick (QPoint &point, QDateTime &x, double)
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
      if (isGrabSelected(point) && grabPosition < 2 && grabPosition > -1)
      {
        tpoint = point;
        tpoint.setX(point.x() - (interval * tpixelspace));
        status = Moving;
      }
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
      setSaveFlag(TRUE);
      date = x;
      emit signalDraw();
      status = None;
      emit message("");
      break;
    default:
      break;
  }
    
  return status;    
}

void Cycle::pointerMoving (QPixmap &, QPoint &p, QDateTime &x, double)
{
  if (status != Moving)
    return;

  int gp = grabPosition;
  if (gp == -1)
    return;
  
  if (gp == 0)
  {
    date = x;
    setSaveFlag(TRUE);
  
    QString s = x.toString(dateFormat);
    emit message(s);
    emit signalDraw();
  }
  else
  {
    if (p.x() <= tpoint.x())
      return;
	
    int t = (p.x() - tpoint.x()) / tpixelspace;
    if (t == 0)
      return;
	  
    interval = t;
    setSaveFlag(TRUE);
	
    QString s = tr("Interval") + "=" + QString::number(t);
    emit message(s);
    emit signalDraw();
  }
}

void Cycle::loadDefaults ()
{
  Config config;
  config.getData(Config::DefaultCycleColor, color);
  config.getData(Config::DefaultCycleInterval, interval);
}

void Cycle::saveDefaults ()
{
  Config config;
  config.setData(Config::DefaultCycleColor, color);
  config.setData(Config::DefaultCycleInterval, interval);
}

double Cycle::getHigh ()
{
  return data->getMax();
}

double Cycle::getLow ()
{
  return data->getMin();
}

void Cycle::saveSettings ()
{
  COSettings co(id, symbol, indicator, QString::number(type));
  co.setDate(date);
  co.setValue(value);
  co.setColor(color);
  QString k("Interval");
  QString d = QString::number(interval);
  co.setString(k, d);

  DataBase db;
  db.setChartObject(co);
}

void Cycle::loadSettings (COSettings &co)
{
  co.getSymbol(symbol);
  co.getID(id);
  co.getIndicator(indicator);
  co.getDate(date);
  value = co.getValue();
  co.getColor(color);
  
  QString k("Interval");
  QString d;
  co.getString(k, d);
  interval = d.toInt();
}

bool Cycle::isGrabSelected (QPoint point)
{
  grabPosition = -1;
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
    {
      grabPosition = loop;
      return TRUE;
    }
  }
  
  return FALSE;
}

