/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "DbPlugin.h"
#include "Config.h"
#include "../../../pics/delete.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/rename.xpm"
#include <qpainter.h>
#include <qcursor.h>

TrendLine::TrendLine ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("red");
  objects.setAutoDelete(TRUE);
  usebar = FALSE;
  extend = TRUE;
  bar = "Close";
  helpFile = "trendline.html";
  
  menu->insertItem(QPixmap(edit), tr("&Edit TrendLine"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move TrendLine"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete TrendLine"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

TrendLine::~TrendLine ()
{
}

void TrendLine::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  QDictIterator<TrendLineObject> it(objects);
  for (; it.current(); ++it)
  {
    TrendLineObject *co = it.current();
    
    if (co->getStatus() == TrendLineObject::Delete)
      continue;
    
    BarDate dt = co->getDate();
    if (! dt.getDate().isValid())
      continue;
    
    int x2 = data->getX(dt);
    if (x2 == -1)
      continue;

    int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
    if (x == -1)
        continue;
	
    BarDate dt2 = co->getDate2();
    if (! dt2.getDate().isValid())
      continue;
    
    x2 = data->getX(dt2);
    if (x2 == -1)
      continue;

    x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
    if (x2 == -1)
        continue;
    
    int y;
    int y2;
    if (co->getUseBar())
    {
      int i = data->getX(dt);
      int i2 = data->getX(dt2);

      while (1)
      {
        if (! co->getBar().compare(tr("Open")))
        {
          y = scaler.convertToY(data->getOpen(i));
          y2 = scaler.convertToY(data->getOpen(i2));
	  break;
        }

        if (! co->getBar().compare(tr("High")))
        {
          y = scaler.convertToY(data->getHigh(i));
          y2 = scaler.convertToY(data->getHigh(i2));
	  break;
        }

        if (! co->getBar().compare(tr("Low")))
        {
          y = scaler.convertToY(data->getLow(i));
          y2 = scaler.convertToY(data->getLow(i2));
	  break;
        }

        if (! co->getBar().compare(tr("Close")))
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
      y = scaler.convertToY(co->getValue());
      y2 = scaler.convertToY(co->getValue2());
    }

    painter.setPen(co->getColor());
    painter.drawLine (x, y, x2, y2);

    // save old values;
    int tx2 = x2;
    int ty2 = y2;
    int tx = x;
    int ty = y;

    if (co->getExtend())
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
    co->clearSelectionArea();
    QPointArray array;
    array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);
    co->setSelectionArea(new QRegion(array));
    
    if (co->getStatus() == TrendLineObject::Selected)
    {
      co->clearGrabHandles();
    
      co->setGrabHandle(new QRegion(tx,
             			    ty - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(tx, ty - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
      
      co->setGrabHandle(new QRegion(tx2,
             			    ty2 - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(tx2, ty2 - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    }
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

  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString ub = tr("Use Bar");
  QString el = tr("Extend Line");
  QString bf = tr("Bar Field");
  
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit TrendLine"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  QColor color = selected->getColor();
  dialog->addColorItem(cl, pl, color);
  QString t = selected->getBar();
  dialog->addComboItem(bf, pl, l, t);
  bool f = selected->getUseBar();
  dialog->addCheckItem(ub, pl, f);
  f = selected->getExtend();
  dialog->addCheckItem(el, pl, f);
  dialog->addCheckItem(sd, pl, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    selected->setColor(dialog->getColor(cl));
    selected->setBar(dialog->getCombo(bf));
    selected->setUseBar(dialog->getCheck(ub));
    selected->setExtend(dialog->getCheck(el));
    selected->setSaveFlag(TRUE);
    
    bool f = dialog->getCheck(sd);
    if (f)
    {
      defaultColor = dialog->getColor(cl);
      bar = dialog->getCombo(bf);
      usebar = dialog->getCheck(ub);
      extend = dialog->getCheck(el);
      
      saveDefaults();
    }
    
    emit signalDraw();
  }
  
  delete dialog;
}

void TrendLine::addObject (Setting &set)
{
  TrendLineObject *co = new TrendLineObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void TrendLine::newObject (QString &ind, QString &n)
{
  loadDefaults();
  indicator = ind;
  name = n;
  mpx2 = -1;
  mpy2 = -1;
  status = ClickWait;
  emit message(tr("Select TrendLine starting point..."));
}

COPlugin::Status TrendLine::pointerClick (QPoint &point, BarDate &x, double y)
{
  if (status == None)
  {
    QDictIterator<TrendLineObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->getStatus() == TrendLineObject::Delete)
        continue;
    
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(TrendLineObject::Selected);
        emit signalDraw();
	return status;
      }
    }
  
    return status;
  }
    
  if (status == Selected)
  {
    moveFlag = selected->isGrabSelected(point);
    if (moveFlag)
    {
      status = Moving;
      return status;
    }
    else
    {
      if (! selected->isSelected(point))
      {
        status = None;
	selected->setStatus(TrendLineObject::Plot);
	selected = 0;
        emit signalDraw();
        return status;
      }
      else
        return status;
    }
  }
  
  if (status == Moving)
  {
    status = Selected;
    return status;
  }
  
  if (status == ClickWait)
  {
    tx = x;
    ty = y;
    mpx = point.x();
    mpy = point.y();
    status = ClickWait2;
    emit message(tr("Select TrendLine ending point..."));
    return status;
  }
  
  if (status == ClickWait2)
  {
    if (x.getDateValue() <= tx.getDateValue())
      return status;
  
    TrendLineObject *co = new TrendLineObject(indicator, name, tx, ty, x, y);
    co->setSaveFlag(TRUE);
    co->setColor(defaultColor);
    co->setBar(bar);
    co->setUseBar(usebar);
    co->setExtend(extend);
    
    objects.replace(name, co);
    
    emit signalDraw();

    status = None;
    emit message("");
    return status;
  }

  return status;    
}

void TrendLine::pointerMoving (QPixmap &buffer, QPoint &point, BarDate &x, double y)
{
  if (status == ClickWait2)
  {
    drawMovingPointer(buffer, point);
    return;
  }
  
  if (! moveFlag || status != Moving)
    return;
    
  if (moveFlag == 1)
  {
    if (x.getDateValue() >= selected->getDate2().getDateValue())
      return;
    
    selected->setDate(x);
    selected->setValue(y);
    selected->setSaveFlag(TRUE);
    emit signalDraw();
    
    QString s;
    x.getDateString(TRUE, s);
    s = s + " " + QString::number(y);
    emit message(s);
  }
  else
  {
    if (x.getDateValue() <= selected->getDate().getDateValue())
      return;
    
    selected->setDate2(x);
    selected->setValue2(y);
    selected->setSaveFlag(TRUE);
    emit signalDraw();
    
    QString s;
    x.getDateString(TRUE, s);
    s = s + " " + QString::number(y);
    emit message(s);
  }
}

void TrendLine::drawMovingPointer (QPixmap &buffer, QPoint &point)
{
  if (point.x() < mpx)
    return;

  QPainter painter;
  painter.begin(&buffer);
  painter.setRasterOp(Qt::XorROP);
  painter.setPen(defaultColor);
      
  // erase the previous line drawn
  if (mpx2 != -1 && mpy2 != -1)
    painter.drawLine (mpx, mpy, mpx2, mpy2);
      
  // draw the new line
  painter.drawLine (mpx, mpy, point.x(), point.y());
      
  mpx2 = point.x();
  mpy2 = point.y();
  
  painter.end();
  
  emit signalRefresh();
}

void TrendLine::saveObjects (QString &chartPath)
{
  if (! chartPath.length())
    return;

  Config config;
  QString plugin = config.parseDbPlugin(chartPath);
  DbPlugin *db = config.getDbPlugin(plugin);
  if (! db)
  {
    config.closePlugin(plugin);
    return;
  }
  
  db->openChart(chartPath);

  QDictIterator<TrendLineObject> it(objects);
  for (; it.current(); ++it)
  {
    TrendLineObject *co = it.current();
    
    if (co->getStatus() == TrendLineObject::Delete)
    {
      QString s = co->getName();
      db->deleteChartObject(s);
      continue;
    }
    
    if (co->getSaveFlag())
    {
      Setting set;
      co->getSettings(set);
      QString s = co->getName();
      db->setChartObject(s, set);
    }
  }
  
  config.closePlugin(plugin);
}

void TrendLine::loadDefaults ()
{
  Config settings;
  
  QString s = "DefaultTrendLineColor";
  s = settings.getData(s);
  if (s.length())
    defaultColor.setNamedColor(s);

  s = "DefaultTrendLineBar";
  s = settings.getData(s);
  if (s.length())
    bar = s;

  s = "DefaultTrendLineExtend";
  s = settings.getData(s);
  if (s.length())
    extend = s.toInt();

  s = "DefaultTrendLineUseBar";
  s = settings.getData(s);
  if (s.length())
    usebar = s.toInt();
}

void TrendLine::saveDefaults ()
{
  Config config;
  
  QString s = "DefaultTrendLineColor";
  QString s2 = defaultColor.name();
  config.setData(s, s2);

  s = "DefaultTrendLineBar";
  config.setData(s, bar);

  s = "DefaultTrendLineExtend";
  s2 = QString::number(extend);
  config.setData(s, s2);

  s = "DefaultTrendLineUseBar";
  s2 = QString::number(usebar);
  config.setData(s, s2);
}

void TrendLine::keyEvent (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_M:
      if (key->state() == Qt::ControlButton)
        moveObject();
      break;
    case Qt::Key_E:
      if (key->state() == Qt::ControlButton)
        prefDialog();
      break;
    case Qt::Key_D:
      if (key->state() == Qt::ControlButton)
        removeObject();
      break;
    default:
      key->ignore();
      break;
  }
}

void TrendLine::moveObject ()
{
  status = Moving;
}

void TrendLine::removeObject ()
{
  selected->setStatus(TrendLineObject::Delete);
  selected = 0;
  status = None;
  emit signalDraw();
}

void TrendLine::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double TrendLine::getHigh ()
{
  double high = -99999999.0;
  
  QDictIterator<TrendLineObject> it(objects);
  for (; it.current(); ++it)
  {
    TrendLineObject *co = it.current();
    if (co->getValue() > high)
      high = co->getValue();
    if (co->getValue2() > high)
      high = co->getValue2();
  }
  
  return high;
}

double TrendLine::getLow ()
{
  double low = 99999999.0;
  
  QDictIterator<TrendLineObject> it(objects);
  for (; it.current(); ++it)
  {
    TrendLineObject *co = it.current();
    if (co->getValue() < low)
      low = co->getValue();
    if (co->getValue2() < low)
      low = co->getValue2();
  }

  return low;
}

void TrendLine::showMenu ()
{
  if (selected)
    menu->exec(QCursor::pos());
}

void TrendLine::getNameList (QStringList &d)
{
  d.clear();
  QDictIterator<TrendLineObject> it(objects);
  for (; it.current(); ++it)
    d.append(it.current()->getName());
}

//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  TrendLine *o = new TrendLine;
  return ((COPlugin *) o);
}

