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

#include "FiboLine.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "Config.h"
#include "../../../src/delete.xpm"
#include "../../../src/edit.xpm"
#include "../../../src/rename.xpm"
#include <qpainter.h>
#include <qsettings.h>
#include <qcursor.h>

FiboLine::FiboLine ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("white");
  objects.setAutoDelete(TRUE);
  helpFile = "fiboline.html";

  Config config;  
  QStringList l = QStringList::split(" ", config.getData(Config::PlotFont), FALSE);
  QFont f(l[0], l[1].toInt(), l[2].toInt());
  font = f;
    
  menu->insertItem(QPixmap(edit), tr("&Edit FiboLine"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move FiboLine"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete FiboLine"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

FiboLine::~FiboLine ()
{
}

void FiboLine::draw (int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setFont(font);
  
  QDictIterator<FiboLineObject> it(objects);
  for (; it.current(); ++it)
  {
    FiboLineObject *co = it.current();
    
    if (co->getStatus() == FiboLineObject::Delete)
      continue;
    
    BarDate dt = co->getStartDate();
    if (! dt.getDate().isValid())
      continue;
    
    int x2 = data->getX(dt);
    if (x2 == -1)
      continue;

    int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
    if (x == -1)
        continue;
	
    BarDate dt2 = co->getEndDate();
    if (! dt2.getDate().isValid())
      continue;
    
    x2 = data->getX(dt2);
    if (x2 == -1)
      continue;

    x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
    if (x2 == -1)
        continue;
    
    painter.setPen(co->getColor());
  
    double high = co->getHigh();
    double low = co->getLow();
    
    int loop;
    for (loop = 1; loop <= 6; loop++)
    {
      if (co->getLine(loop) != 0)
      {
        double r = getY(co->getLine(loop), co->getHigh(), co->getLow());
        int y = scaler->convertToY(r);
        painter.drawLine (x, y, x2, y);
    
        painter.drawText(x, y - 1, QString::number(co->getLine(loop) * 100) + "%", -1);
    
        if (r > high)
          high = r;
        if (r < low)
          low = r;
      }
    }
  
    // draw the low line
    int y = scaler->convertToY(co->getLow());
    painter.drawLine (x, y, x2, y);
    if (co->getStatus() == FiboLineObject::Selected)
      painter.drawText(x, y - 1, QString::number(co->getLow()), -1);

    co->clearSelectionArea();
    QPointArray array;
  
    // store the selectable area the low line occupies
    array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
    co->setSelectionArea(new QRegion(array));
    
    // draw the high line
    int y2 = scaler->convertToY(co->getHigh());
    painter.drawLine (x, y2, x2, y2);
    if (co->getStatus() == FiboLineObject::Selected)
      painter.drawText(x, y2 - 1, QString::number(co->getHigh()), -1);

    // store the selectable area the high line occupies
    array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
    co->setSelectionArea(new QRegion(array));
    
    if (co->getStatus() == FiboLineObject::Selected)
    {
      co->clearGrabHandles();
    
      //bottom left corner
      y = scaler->convertToY(low);
      co->setGrabHandle(new QRegion(x,
             			    y - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(x, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
    
      //top right corner
      y2 = scaler->convertToY(high);
      co->setGrabHandle(new QRegion(x2,
             			    y2 - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(x2, y2 - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    }
  }

  painter.end();
}

double FiboLine::getY (double v, double high, double low)
{
  double range = high - low;
  double r = 0;
  if (v < 0)
    r = high + (range * v);
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
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit FiboLine"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile (helpFile);
  dialog->addColorItem(tr("Color"), tr("Details"), selected->getColor());
  dialog->addCheckItem(tr("Set Default"), tr("Details"), FALSE);

  dialog->createPage (tr("Levels"));
  dialog->addFloatItem(tr("Line 1"), tr("Levels"), selected->getLine(1));
  dialog->addFloatItem(tr("Line 2"), tr("Levels"), selected->getLine(2));
  dialog->addFloatItem(tr("Line 3"), tr("Levels"), selected->getLine(3));
  dialog->addFloatItem(tr("Line 4"), tr("Levels"), selected->getLine(4));
  dialog->addFloatItem(tr("Line 5"), tr("Levels"), selected->getLine(5));
  dialog->addFloatItem(tr("Line 6"), tr("Levels"), selected->getLine(6));
    
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    selected->setColor(dialog->getColor(tr("Color")));
    selected->setLine(1, dialog->getFloat(tr("Line 1")));
    selected->setLine(2, dialog->getFloat(tr("Line 2")));
    selected->setLine(3, dialog->getFloat(tr("Line 3")));
    selected->setLine(4, dialog->getFloat(tr("Line 4")));
    selected->setLine(5, dialog->getFloat(tr("Line 5")));
    selected->setLine(6, dialog->getFloat(tr("Line 6")));
    
    bool f = dialog->getCheck(tr("Set Default"));
    if (f)
    {
      defaultColor = dialog->getColor(tr("Color"));
      
      saveDefaults();
    }
    
    emit signalDraw();
  }
  
  delete dialog;
}

void FiboLine::addObject (Setting *set)
{
  FiboLineObject *co = new FiboLineObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void FiboLine::newObject (QString ind, QString n)
{
  indicator = ind;
  name = n;
  mpx2 = -1;
  mpy2 = -1;
  status = ClickWait;
  emit message(tr("Select FiboLine high point..."));
}

COPlugin::Status FiboLine::pointerClick (QPoint point, BarDate x, double y)
{
  if (status == None)
  {
    QDictIterator<FiboLineObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->getStatus() == FiboLineObject::Delete)
        continue;
    
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(FiboLineObject::Selected);
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
	selected->setStatus(FiboLineObject::Plot);
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
    emit message(tr("Select FiboLine low point..."));
    return status;
  }
  
  if (status == ClickWait2)
  {
    FiboLineObject *co = new FiboLineObject(indicator, name, tx, ty, x, y);
    co->setSaveFlag(TRUE);
    co->setColor(defaultColor);
    
    objects.replace(name, co);
    
    emit signalDraw();

    status = None;
    emit message("");
    return status;
  }

  return status;    
}

void FiboLine::pointerMoving (QPoint point, BarDate x, double y)
{
  if (status == ClickWait2)
  {
    drawMovingPointer(point);
    return;
  }
  
  if (! moveFlag || status != Moving)
    return;
    
  if (moveFlag == 1)
  {
    // bottom left corner
    if (x.getDateValue() >= selected->getEndDate().getDateValue())
      return;
    
    if (y >= selected->getHigh())
      return;

    selected->setStartDate(x);
    selected->setLow(y);
    selected->setSaveFlag(TRUE);
    
    emit signalDraw();
    
    QString s = x.getDateString(TRUE) + " " + QString::number(y);
    emit message(s);
  }
  else
  {
    //top right corner
    if (x.getDateValue() <= selected->getStartDate().getDateValue())
      return;

    if (y <= selected->getLow())
      return;

    selected->setEndDate(x);
    selected->setHigh(y);
    selected->setSaveFlag(TRUE);
    
    emit signalDraw();
    
    QString s = x.getDateString(TRUE) + " " + QString::number(y);
    emit message(s);
  }
}

void FiboLine::drawMovingPointer (QPoint point)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setRasterOp(Qt::XorROP);
  painter.setPen(defaultColor);
      
  // erase the previous line drawn
  if (mpx2 != -1 && mpy2 != -1)
  {
    painter.drawLine (mpx, mpy, mpx2, mpy);
    painter.drawLine (mpx, mpy2, mpx2, mpy2);
  }
      
  // draw the new line
  painter.drawLine (mpx, mpy, point.x(), mpy);
  painter.drawLine (mpx, point.y(), point.x(), point.y());
        
  mpx2 = point.x();
  mpy2 = point.y();
  
  painter.end();
  
  emit signalRefresh();
}

void FiboLine::saveObjects (QString chartPath)
{
  if (! chartPath.length())
    return;

  ChartDb *db =  new ChartDb;
  db->openChart(chartPath);

  QDictIterator<FiboLineObject> it(objects);
  for (; it.current(); ++it)
  {
    FiboLineObject *co = it.current();
    
    if (co->getStatus() == FiboLineObject::Delete)
    {
      db->deleteChartObject(co->getName());
      continue;
    }
    
    if (co->getSaveFlag())
    {
      Setting *set = co->getSettings();
      db->setChartObject(co->getName(), set);
      delete set;
    }
  }
  
  delete db;
}

void FiboLine::loadDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultFiboLineColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void FiboLine::saveDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultFiboLineColor";
  settings.writeEntry(s, defaultColor.name());
}

void FiboLine::keyEvent (QKeyEvent *key)
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

void FiboLine::moveObject ()
{
  status = Moving;
}

void FiboLine::removeObject ()
{
  selected->setStatus(FiboLineObject::Delete);
  selected = 0;
  status = None;
  emit signalDraw();
}

void FiboLine::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double FiboLine::getHigh ()
{
  double high = -99999999.0;
  
  QDictIterator<FiboLineObject> it(objects);
  for (; it.current(); ++it)
  {
    FiboLineObject *co = it.current();
    if (co->getHigh() > high)
      high = co->getHigh();
  }
  
  return high;
}

double FiboLine::getLow ()
{
  double low = 99999999.0;
  
  QDictIterator<FiboLineObject> it(objects);
  for (; it.current(); ++it)
  {
    FiboLineObject *co = it.current();
    if (co->getLow() < low)
      low = co->getLow();
  }

  return low;
}

void FiboLine::showMenu ()
{
  if (selected)
    menu->exec(QCursor::pos());
}

//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  FiboLine *o = new FiboLine;
  return ((COPlugin *) o);
}

