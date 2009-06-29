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

#include "FiboLine.h"
#include "PrefDialog.h"
#include "DataBase.h"
#include "Config.h"
#include <QPainter>
#include <QPolygon>



FiboLine::FiboLine ()
{
  color.setNamedColor("red");
  helpFile = "fiboline.html";
  extend = FALSE;
  line1 = 0.382;
  line2 = 0.5;
  line3 = 0.618;
  line4 = 0;
  line5 = 0;
  line6 = 0;
  startDate = QDateTime::currentDateTime();
  endDate = startDate;
  type = (int) COFiboLine;

  loadDefaults();
}

FiboLine::~FiboLine ()
{
}

void FiboLine::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(font);
  
  int x2 = data->getX(startDate);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;
	
  QDateTime dt = endDate;
  if (extend)
    data->getDate(data->count() - 1, dt);
    
  x2 = data->getX(dt);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x2 == -1)
    return;
    
  painter.setPen(color);
  
  QPolygon array;
  clearSelectionArea();
    
  int loop;
  for (loop = 1; loop <= 6; loop++)
  {
    if (getLine(loop) != 0)
    {
      double r = getY(getLine(loop), getHigh(), getLow());
      int y = scaler.convertToY(r);
      painter.drawLine (x, y, x2, y);
      painter.drawText(x, y - 1, QString::number(getLine(loop) * 100) + "% - " + QString::number(r));
	
      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      setSelectionArea(new QRegion(array));
    }
  }
  
  // draw the low line
  int y = scaler.convertToY(getLow());
  painter.drawLine (x, y, x2, y);
  // if (co->getStatus() == FiboLineObject::Selected)
  painter.drawText(x, y - 1, "0% - " + QString::number(getLow()));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  setSelectionArea(new QRegion(array));
    
  // draw the high line
  int y2 = scaler.convertToY(getHigh());
  painter.drawLine (x, y2, x2, y2);
  // if (co->getStatus() == FiboLineObject::Selected)
  painter.drawText(x, y2 - 1, "100% - " + QString::number(getHigh()));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));
    
  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
    
    //bottom left corner
    y = scaler.convertToY(getLow());
    setGrabHandle(new QRegion(x,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(x, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    //top right corner
    y2 = scaler.convertToY(getHigh());
    setGrabHandle(new QRegion(x2,
             		      y2 - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(x2, y2 - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
  }

  painter.end();
}

double FiboLine::getY (double v, double high, double low)
{
  double range = high - low;
  double r = 0;
  if (v < 0)
    r = low + (range * v);
  else
  {
    if (v > 0)
      r = low + (range * v);
    else
    {
      if (v < 0)
        r = high;
      else
        r = low;
    }
  }

  return r;
}

void FiboLine::prefDialog ()
{
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString l1 = tr("Line 1");
  QString l2 = tr("Line 2");
  QString l3 = tr("Line 3");
  QString l4 = tr("Line 4");
  QString l5 = tr("Line 5");
  QString l6 = tr("Line 6");
  QString hl = tr("High");
  QString ll = tr("Low");
  QString el = tr("Extend");

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Edit FiboLine"));
  dialog->addColorPrefItem(cl, color);
  dialog->addDoubleItem(hl, getHigh());
  dialog->addDoubleItem(ll, getLow());
  dialog->addCheckItem(el, extend);
  dialog->addCheckItem(sd, FALSE);

  dialog->addDoubleItem(l1, getLine(1));
  dialog->addDoubleItem(l2, getLine(2));
  dialog->addDoubleItem(l3, getLine(3));
  dialog->addDoubleItem(l4, getLine(4));
  dialog->addDoubleItem(l5, getLine(5));
  dialog->addDoubleItem(l6, getLine(6));
    
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    line1 = dialog->getDouble(l1);
    line2 = dialog->getDouble(l2);
    line3 = dialog->getDouble(l3);
    line4 = dialog->getDouble(l4);
    line5 = dialog->getDouble(l5);
    line6 = dialog->getDouble(l6);
    high = dialog->getDouble(hl);
    low = dialog->getDouble(ll);
    extend = dialog->getCheck(el);
    
    setSaveFlag(TRUE);
    
    bool f = dialog->getCheck(sd);
    if (f)
      saveDefaults();
    
    emit signalDraw();
  }
  
  delete dialog;
}

void FiboLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  id = n;
  mpx2 = -1;
  mpy2 = -1;
  status = ClickWait;
  emit message(tr("Select FiboLine high point..."));
}

COBase::Status FiboLine::pointerClick (QPoint &point, QDateTime &x, double y)
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
      moveFlag = isGrabSelected(point);
      if (moveFlag)
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
      tx = x;
      ty = y;
      mpx = point.x();
      mpy = point.y();
      status = ClickWait2;
      emit message(tr("Select FiboLine low point..."));
      break;
    case ClickWait2:
      if (x <= tx)
        break;
      if (y >= ty)
        break;
      status = None;
      emit message("");
      break;
    default:
      break;
  }

  return status;    
}

void FiboLine::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double y)
{
  if (status == ClickWait2)
  {
    startDate = tx;
    endDate = x;
    high = ty;
    low = y;
    setSaveFlag(TRUE);
    emit signalDraw();
    QString s = x.toString(dateFormat) + " " + QString::number(y);
    emit message(s);
    return;
  }
  
  if (! moveFlag || status != Moving)
    return;
    
  if (moveFlag == 1)
  {
    // bottom left corner
    if (x >= endDate)
      return;
    
    if (y >= getHigh())
      return;

    startDate = x;
    low = y;
    setSaveFlag(TRUE);
    
    emit signalDraw();
    
    QString s = x.toString(dateFormat) + " " + QString::number(y);
    emit message(s);
  }
  else
  {
    //top right corner
    if (x <= startDate)
      return;

    if (y <= getLow())
      return;

    endDate = x;
    high = y;
    setSaveFlag(TRUE);
    
    emit signalDraw();
    
    QString s = x.toString(dateFormat) + " " + QString::number(y);
    emit message(s);
  }
}

void FiboLine::loadDefaults ()
{
  Config config;
  config.getData(Config::DefaultFiboLineColor, color);
  config.getData(Config::DefaultFiboLine1, line1);
  config.getData(Config::DefaultFiboLine2, line2);
  config.getData(Config::DefaultFiboLine3, line3);
  config.getData(Config::DefaultFiboLine4, line4);
  config.getData(Config::DefaultFiboLine5, line5);
  config.getData(Config::DefaultFiboLine6, line6);
}

void FiboLine::saveDefaults ()
{
  Config config;
  config.setData(Config::DefaultFiboLineColor, color);
  config.setData(Config::DefaultFiboLine1, line1);
  config.setData(Config::DefaultFiboLine2, line2);
  config.setData(Config::DefaultFiboLine3, line3);
  config.setData(Config::DefaultFiboLine4, line4);
  config.setData(Config::DefaultFiboLine5, line5);
  config.setData(Config::DefaultFiboLine6, line6);
}

double FiboLine::getHigh ()
{
  return high;
}

double FiboLine::getLow ()
{
  return low;
}

void FiboLine::saveSettings ()
{
  COSettings co(id, symbol, indicator, QString::number(type));
  co.setDate(startDate);
  co.setDate2(endDate);
  co.setColor(color);

  QString k("High");
  QString d = QString::number(high);
  co.setString(k, d);
  
  k = "Low";
  d = QString::number(low);
  co.setString(k, d);
  
  k = "Line1";
  d = QString::number(line1);
  co.setString(k, d);
  
  k = "Line2";
  d = QString::number(line2);
  co.setString(k, d);

  k = "Line3";
  d = QString::number(line3);
  co.setString(k, d);

  k = "Line4";
  d = QString::number(line4);
  co.setString(k, d);

  k = "Line5";
  d = QString::number(line5);
  co.setString(k, d);

  k = "Line6";
  d = QString::number(line6);
  co.setString(k, d);

  k = "Extend";
  d = QString::number(extend);
  co.setString(k, d);

  DataBase db;
  db.setChartObject(co);
}

void FiboLine::loadSettings (COSettings &co)
{
  co.getSymbol(symbol);
  co.getID(id);
  co.getIndicator(indicator);
  co.getDate(startDate);
  co.getDate2(endDate);
  co.getColor(color);

  QString k("High");
  QString d;
  co.getString(k, d);
  high = d.toDouble();  

  k = "Low";
  co.getString(k, d);
  low = d.toDouble();  

  k = "Line1";
  co.getString(k, d);
  line1 = d.toDouble();  

  k = "Line2";
  co.getString(k, d);
  line2 = d.toDouble();  

  k = "Line3";
  co.getString(k, d);
  line3 = d.toDouble();  

  k = "Line4";
  co.getString(k, d);
  line4 = d.toDouble();  

  k = "Line5";
  co.getString(k, d);
  line5 = d.toDouble();  

  k = "Line6";
  co.getString(k, d);
  line6 = d.toDouble();  

  k = "Extend";
  co.getString(k, d);
  extend = d.toInt();  
}

int FiboLine::isGrabSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return loop + 1;
  }
  
  return 0;
}

double FiboLine::getLine (int i)
{
  double v = 0.0;
  
  switch (i)
  {
    case 1:
      v = line1;
      break;
    case 2:
      v = line2;
      break;
    case 3:
      v = line3;
      break;
    case 4:
      v = line4;
      break;
    case 5:
      v = line5;
      break;
    case 6:
      v = line6;
      break;
    default:
      break;
  }
  
  return v;
}

void FiboLine::adjustForSplit (QDateTime &dt, double d)
{
  if (startDate < dt && endDate < dt)
  {
    high = high * d;
    low = low * d;
  }
}

