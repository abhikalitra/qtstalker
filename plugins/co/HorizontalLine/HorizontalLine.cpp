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

#include "HorizontalLine.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "Config.h"
#include <qpainter.h>
#include <qsettings.h>
#include <qpointarray.h>

HorizontalLine::HorizontalLine ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("white");
  objects.setAutoDelete(TRUE);

  Config config;  
  QStringList l = QStringList::split(" ", config.getData(Config::PlotFont), FALSE);
  QFont f(l[0], l[1].toInt(), l[2].toInt());
  font = f;
  
  menu->insertItem(tr("&Edit HorizontalLine"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(tr("&Move HorizontalLine"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(tr("&Delete HorizontalLine"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

HorizontalLine::~HorizontalLine ()
{
}

void HorizontalLine::draw (int, int, int)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setFont(font);
  
  QDictIterator<HorizontalLineObject> it(objects);
  for (; it.current(); ++it)
  {
    HorizontalLineObject *co = it.current();
    
    if (co->getStatus() == HorizontalLineObject::Delete)
      continue;
      
    int y = scaler->convertToY(co->getValue());
  
    // if value is off chart then don't draw it
    if (co->getValue() < scaler->getLow())
      continue;
      
    painter.setPen(co->getColor());
      
    painter.drawLine (0, y, buffer->width(), y);
    painter.drawText(0, y - 1, QString::number(co->getValue()), -1);
  
    co->clearSelectionArea();
    QPointArray array;
    array.putPoints(0, 4, 0, y - 4, 0, y + 4, buffer->width(), y + 4, buffer->width(), y - 4);
    co->setSelectionArea(new QRegion(array));
    
    if (co->getStatus() == HorizontalLineObject::Selected)
    {
      co->clearGrabHandles();
      int t = (int) buffer->width() / 4;
    
      co->setGrabHandle(new QRegion(0,
             			    y - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(0, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
  
      co->setGrabHandle(new QRegion(t,
             			    y - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(t, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
      co->setGrabHandle(new QRegion(t * 2,
             			    y - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(t * 2, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
      co->setGrabHandle(new QRegion(t * 3,
             			    y - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(t * 3, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
      co->setGrabHandle(new QRegion(t * 4,
             			    y - (HANDLE_WIDTH / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(t * 4, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    }
  }

  painter.end();
}

void HorizontalLine::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit HorizontalLine"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), tr("Details"), selected->getColor());
  dialog->addCheckItem(tr("Set Default"), tr("Details"), FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QColor color = dialog->getColor(tr("Color"));
    selected->setColor(color);
    
    bool f = dialog->getCheck(tr("Set Default"));
    if (f)
    {
      defaultColor = color;
      saveDefaults();
    }
    
    emit signalDraw();
  }
  
  delete dialog;
}

void HorizontalLine::addObject (Setting *set)
{
  HorizontalLineObject *co = new HorizontalLineObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void HorizontalLine::newObject (QString ind, QString n)
{
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place HorizontalLine..."));
}

COPlugin::Status HorizontalLine::pointerClick (QPoint point, BarDate, double y)
{
  if (status == None)
  {
    QDictIterator<HorizontalLineObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(HorizontalLineObject::Selected);
        emit signalDraw();
	return status;
      }
    }
  
    return status;
  }
    
  if (status == Selected)
  {
    if (selected->isGrabSelected(point))
    {
      status = Moving;
      return status;
    }
    else
    {
      if (! selected->isSelected(point))
      {
        status = None;
	selected->setStatus(HorizontalLineObject::Plot);
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
    HorizontalLineObject *co = new HorizontalLineObject(indicator, name, y);
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

void HorizontalLine::pointerMoving (QPoint, BarDate, double y)
{
  if (status != Moving)
    return;
    
  selected->setValue(y);
  selected->setSaveFlag(TRUE);
  
  emit signalDraw();
  
  QString s = QString::number(y);
  emit message(s);
}

void HorizontalLine::saveObjects (QString chartPath)
{
  if (! chartPath.length())
    return;

  ChartDb *db =  new ChartDb;
  db->openChart(chartPath);

  QDictIterator<HorizontalLineObject> it(objects);
  for (; it.current(); ++it)
  {
    HorizontalLineObject *co = it.current();
    
    if (co->getStatus() == HorizontalLineObject::Delete)
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

void HorizontalLine::loadDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultHorizontalLineColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void HorizontalLine::saveDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultHorizontalLineColor";
  settings.writeEntry(s, defaultColor.name());
}

void HorizontalLine::keyEvent (QKeyEvent *key)
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

void HorizontalLine::moveObject ()
{
  status = Moving;
}

void HorizontalLine::removeObject ()
{
  selected->setStatus(HorizontalLineObject::Delete);
  selected = 0;
  status = None;
  emit signalDraw();
}

void HorizontalLine::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double HorizontalLine::getHigh ()
{
  double high = -99999999.0;
  
  QDictIterator<HorizontalLineObject> it(objects);
  for (; it.current(); ++it)
  {
    HorizontalLineObject *co = it.current();
    if (co->getValue() > high)
      high = co->getValue();
  }
  
  return high;
}

double HorizontalLine::getLow ()
{
  double low = 99999999.0;
  
  QDictIterator<HorizontalLineObject> it(objects);
  for (; it.current(); ++it)
  {
    HorizontalLineObject *co = it.current();
    if (co->getValue() < low)
      low = co->getValue();
  }

  return low;
}

//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  HorizontalLine *o = new HorizontalLine;
  return ((COPlugin *) o);
}

