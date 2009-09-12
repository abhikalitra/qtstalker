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

#include "HorizontalLine.h"
#include "PrefDialog.h"
#include "DataBase.h"
#include "Config.h"

#include <QPainter>
#include <QPolygon>



HorizontalLine::HorizontalLine ()
{
  color.setNamedColor("red");
  helpFile = "horizontalline.html";
  type = (int) COHorizontalLine;

  loadDefaults();
}

void HorizontalLine::draw (QPixmap &buffer, Scaler &scaler, int, int, int)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(font);
  
  int y = scaler.convertToY(value);
  
  // if value is off chart then don't draw it
  if (value < scaler.getLow())
    return;
      
  painter.setPen(color);
    
  QFontMetrics fm(font);
  int pixelsWide = fm.width(text);
  painter.drawLine (0, y, buffer.width(), y);
  painter.drawText(0, y - 1, text);
  painter.drawText(0 + pixelsWide + 1, y - 1, QString::number(value));
  
  clearSelectionArea();
  QPolygon array;
  array.putPoints(0, 4, 0, y - 4, 0, y + 4, buffer.width(), y + 4, buffer.width(), y - 4);
  setSelectionArea(new QRegion(array));
    
  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
    int t = (int) buffer.width() / 4;
    
    setGrabHandle(new QRegion(0,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(0, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
  
    setGrabHandle(new QRegion(t,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    setGrabHandle(new QRegion(t * 2,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
		              HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t * 2, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    setGrabHandle(new QRegion(t * 3,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
		  	      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t * 3, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    setGrabHandle(new QRegion(t * 4,
             	              y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
		    	      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t * 4, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
  }

  painter.end();
}

void HorizontalLine::prefDialog ()
{
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString vl = tr("Value");
  QString tx = tr("Text");

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Edit HorizontalLine"));
  dialog->addColorPrefItem(cl, color);
  dialog->addDoubleItem(vl, value);
  dialog->addTextItem(tx, text);  //cz odkazy na objekty definovane nahore
  dialog->addCheckItem(sd, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    value = dialog->getDouble(vl);
    dialog->getText(tx, text);
    setSaveFlag(TRUE);
    
    bool f = dialog->getCheck(sd);
    if (f)
      saveDefaults();
    
    emit signalDraw();
  }
  
  delete dialog;
}

void HorizontalLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  id = n;
  status = ClickWait;
  emit message(tr("Select point to place HorizontalLine..."));
}

COBase::Status HorizontalLine::pointerClick (QPoint &point, QDateTime &, double y)
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
      value = y;
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

void HorizontalLine::pointerMoving (QPixmap &, QPoint &, QDateTime &, double y)
{
  if (status != Moving)
    return;
    
  value = y;
  setSaveFlag(TRUE);
  emit signalDraw();
  QString s = QString::number(y);
  emit message(s);
}

void HorizontalLine::loadDefaults ()
{
  Config config;
  config.getData(Config::DefaultHorizontalLineColor, color);
}

void HorizontalLine::saveDefaults ()
{
  Config config;
  config.setData(Config::DefaultHorizontalLineColor, color);
}

void HorizontalLine::saveSettings ()
{
  COSettings co(id, symbol, indicator, QString::number(type));
  co.setValue(value);
  co.setColor(color);
  co.setText(text);

  DataBase db;
  db.setChartObject(co);
}

void HorizontalLine::loadSettings (COSettings &co)
{
  co.getSymbol(symbol);
  co.getID(id);
  co.getIndicator(indicator);
  co.getText(text);
  value = co.getValue();
  co.getColor(color);
}

void HorizontalLine::getInfo (Setting &info)
{
  QString k = tr("Text");
  info.setData(k, text);
  
  k = tr("Price");
  QString d = QString::number(value);
  info.setData(k, d);
}

