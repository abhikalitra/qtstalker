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

#include "SellArrow.h"
#include "PrefDialog.h"
#include "DataBase.h"
#include "Config.h"

#include <QPainter>



SellArrow::SellArrow ()
{
  color.setNamedColor("red");
  helpFile = "sellarrow.html";
  type = (int) COSellArrow;
  
  loadDefaults();
}

void SellArrow::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;
    
  int y = scaler.convertToY(value);

  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
	          x + 2, y - 5,
	          x + 2, y - 11,
	          x - 2, y - 11,
	          x - 2, y - 5,
                  x - 5, y - 5);
  painter.setBrush(color);
  painter.drawPolygon(arrow, Qt::OddEvenFill);

  clearSelectionArea();
  setSelectionArea(new QRegion(arrow));
    
  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
    
    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  y + 1,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
				   
    painter.fillRect(x - (HANDLE_WIDTH / 2), y + 1, HANDLE_WIDTH, HANDLE_WIDTH, color);
  }

  painter.end();
}

void SellArrow::prefDialog ()
{
  QString cl = tr("Color");
  QString vl = tr("Value");
  QString il = tr("Identifier");
  QString bl = tr("Price");
  QString sd = tr("Set Default");

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Edit SellArrow"));
  dialog->addColorPrefItem(cl, color);
  dialog->addDoubleItem(vl, value);
//  dialog->addTextItem(il, identifier);
//  dialog->addTextItem(bl, price);
  dialog->addCheckItem(sd, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    value = dialog->getDouble(vl);
//    dialog->getText(il, identifier);
//    dialog->getText(bl, price);
    
    setSaveFlag(TRUE);
    
    bool f = dialog->getCheck(sd);
    if (f)
      saveDefaults();
    
    emit signalDraw();
  }
  
  delete dialog;
}

void SellArrow::newObject (QString &ind, QString &n)
{
  indicator = ind;
  id = n;
  status = ClickWait;
  emit message(tr("Select point to place SellArrow..."));
}

COBase::Status SellArrow::pointerClick (QPoint &point, QDateTime &x, double y)
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

void SellArrow::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double y)
{
  if (status != Moving)
    return;
    
  date = x;
  value = y;
  setSaveFlag(TRUE);
  emit signalDraw();
  QString s = x.toString(dateFormat) + " " + QString::number(y);
  emit message(s);
}

void SellArrow::loadDefaults ()
{
  Config config;
  config.getData(Config::DefaultSellArrowColor, color);
}

void SellArrow::saveDefaults ()
{
  Config config;
  config.setData(Config::DefaultSellArrowColor, color);
}

void SellArrow::saveSettings ()
{
  COSettings co(id, symbol, indicator, QString::number(type));
  co.setDate(date);
  co.setValue(value);
  co.setColor(color);

  DataBase db;
  db.setChartObject(co);
}

void SellArrow::loadSettings (COSettings &co)
{
  co.getSymbol(symbol);
  co.getID(id);
  co.getIndicator(indicator);
  co.getDate(date);
  value = co.getValue();
  co.getColor(color);
}

void SellArrow::getInfo (Setting &info)
{
  QString k = "D";
  QString d = date.toString("yyyy-MM-dd");
  info.setData(k, d);
  
  k = "T";
  d = date.toString("HH:MM:ss");
  info.setData(k, d);
  
  k = tr("Price");
  d = QString::number(value);
  info.setData(k, d);
}

