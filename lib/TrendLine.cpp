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

#include "TrendLine.h"
#include "PrefDialog.h"
#include "Config.h"
#include "DataBase.h"
#include <QPainter>
#include <QPolygon>



TrendLine::TrendLine ()
{
  color.setNamedColor("red");
  usebar = FALSE;
  extend = TRUE;
  bar = "Close";
  helpFile = "trendline.html";
  date2 = date;
  type = (int) COTrendLine;

  loadDefaults();
}

TrendLine::~TrendLine ()
{
}

void TrendLine::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;
	
  x2 = data->getX(date2);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x2 == -1)
    return;
    
  int y;
  int y2;
  if (usebar)
  {
    int i = data->getX(date);
    int i2 = data->getX(date2);

    while (1)
    {
      if (! bar.compare(tr("Open")))
      {
        y = scaler.convertToY(data->getOpen(i));
        y2 = scaler.convertToY(data->getOpen(i2));
	break;
      }

      if (! bar.compare(tr("High")))
      {
        y = scaler.convertToY(data->getHigh(i));
        y2 = scaler.convertToY(data->getHigh(i2));
	break;
      }

      if (! bar.compare(tr("Low")))
      {
        y = scaler.convertToY(data->getLow(i));
        y2 = scaler.convertToY(data->getLow(i2));
	break;
      }

      if (! bar.compare(tr("Close")))
      {
        y = scaler.convertToY(data->getClose(i));
        y2 = scaler.convertToY(data->getClose(i2));
	break;
      }

      return;
    }
  }
  else
  {
    y = scaler.convertToY(value);
    y2 = scaler.convertToY(value2);
  }

  painter.setPen(color);
  painter.drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (extend)
  {  
    int ydiff = y - y2;
    int xdiff = x2 - x;
    while (x2 < buffer.width())
    {
      x = x2;
      y = y2;
      x2 = x2 + xdiff;
      y2 = y2 - ydiff;
      painter.drawLine (x, y, x2, y2);
    }
  }

  // store the selectable area the line occupies
  clearSelectionArea();
  QPolygon array;
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));
    
  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
    
    setGrabHandle(new QRegion(tx,
           	  ty - (HANDLE_WIDTH / 2),
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(tx, ty - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
      
    setGrabHandle(new QRegion(tx2,
             	  ty2 - (HANDLE_WIDTH / 2),
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(tx2, ty2 - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
  }

  painter.end();
}

void TrendLine::prefDialog ()
{
  QStringList l;
  l.append(tr("Open"));
  l.append(tr("High"));
  l.append(tr("Low"));
  l.append(tr("Close"));

  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString ub = tr("Use Bar");
  QString el = tr("Extend Line");
  QString bf = tr("Bar Field");
  QString sl = tr("Start Value");
  QString dl = tr("End Value");
  
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Edit TrendLine"));
  dialog->addColorPrefItem(cl, color);
  dialog->addComboItem(bf, l, bar);
  dialog->addCheckItem(ub, usebar);
  dialog->addCheckItem(el, extend);
  dialog->addDoubleItem(sl, value);
  dialog->addDoubleItem(dl, value2);
  dialog->addCheckItem(sd, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    dialog->getCombo(bf, bar);
    usebar = dialog->getCheck(ub);
    extend = dialog->getCheck(el);
    value = dialog->getDouble(sl);
    value2 = dialog->getDouble(dl);
    
    setSaveFlag(TRUE);
    
    bool f = dialog->getCheck(sd);
    if (f)
      saveDefaults();
    
    emit signalDraw();
  }
  
  delete dialog;
}

void TrendLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  id = n;
  mpx2 = -1;
  mpy2 = -1;
  status = ClickWait;
  emit message(tr("Select TrendLine starting point..."));
}

COBase::Status TrendLine::pointerClick (QPoint &point, QDateTime &x, double y)
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
      emit message(tr("Select TrendLine ending point..."));
      break;
    case ClickWait2:
      if (x <= tx)
        break;
  
      date = tx;
      date2 = x;
      value = ty;
      value2 = y;

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

void TrendLine::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double y)
{
  if (status == ClickWait2)
  {
    date = tx;
    value = ty;
    date2 = x;
    value2 = y;
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
    if (x >= date2)
      return;
    
    date = x;
    value = y;
    setSaveFlag(TRUE);
    emit signalDraw();
    QString s = x.toString(dateFormat) + " " + QString::number(y);
    emit message(s);
  }
  else
  {
    if (x <= date)
      return;
    
    date2 = x;
    value2 = y;
    setSaveFlag(TRUE);
    emit signalDraw();
    
    QString s = x.toString(dateFormat) + " " + QString::number(y);
    emit message(s);
  }
}

void TrendLine::loadDefaults ()
{
  Config config;
  config.getData(Config::DefaultTrendLineColor, color);
  config.getData(Config::DefaultTrendLineBar, bar);
  config.getData(Config::DefaultTrendLineExtend, extend);
  config.getData(Config::DefaultTrendLineUseBar, usebar);
}

void TrendLine::saveDefaults ()
{
  Config config;
  config.setData(Config::DefaultTrendLineColor, color);
  config.setData(Config::DefaultTrendLineBar, bar);
  config.setData(Config::DefaultTrendLineExtend, extend);
  config.setData(Config::DefaultTrendLineUseBar, usebar);
}

double TrendLine::getHigh ()
{
  double high = -99999999.0;

  if (value > high)
      high = value;
  if (value2 > high)
      high = value2;
  
  return high;
}

double TrendLine::getLow ()
{
  double low = 99999999.0;
  
  if (value < low)
    low = value;
  if (value2 < low)
    low = value2;

  return low;
}

int TrendLine::isGrabSelected (QPoint point)
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

void TrendLine::saveSettings ()
{
  COSettings co(id, symbol, indicator, QString::number(type));
  co.setDate(date);
  co.setDate2(date2);
  co.setValue(value);
  co.setValue2(value2);
  co.setColor(color);

  QString k("Field");
  co.setString(k, bar);

  k = "UseBar";
  QString d = QString::number(usebar);
  co.setString(k, d);

  k = "Extend";
  d = QString::number(extend);
  co.setString(k, d);

  DataBase db;
  db.setChartObject(co);
}

void TrendLine::loadSettings (COSettings &co)
{
  co.getSymbol(symbol);
  co.getID(id);
  co.getIndicator(indicator);
  co.getDate(date);
  co.getDate2(date2);
  value = co.getValue();
  value2 = co.getValue2();
  co.getColor(color);
  
  QString k("Field");
  co.getString(k, bar);

  k = "UseBar";
  QString d;
  co.getString(k, d);
  usebar = d.toInt();

  k = "Extend";
  co.getString(k, d);
  extend = d.toInt();
}

void TrendLine::adjustForSplit (QDateTime &dt, double d)
{
  if (date < dt)
    value = value * d;
  if (date2 < dt)
    value2 = value2 * d;
}


