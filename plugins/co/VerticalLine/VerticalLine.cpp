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

#include "VerticalLine.h"
#include "PrefDialog.h"
#include "DbPlugin.h"
#include "Config.h"
#include "../../../pics/delete.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/rename.xpm"
#include <qpainter.h>
#include <qsettings.h>
#include <qcursor.h>

VerticalLine::VerticalLine ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("red");
  objects.setAutoDelete(TRUE);
  helpFile = "verticalline.html";
  
  menu->insertItem(QPixmap(edit), tr("&Edit VerticalLine"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move VerticalLine"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete VerticalLine"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

VerticalLine::~VerticalLine ()
{
}

void VerticalLine::draw (int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(buffer);
  
  QDictIterator<VerticalLineObject> it(objects);
  for (; it.current(); ++it)
  {
    VerticalLineObject *co = it.current();
    
    if (co->getStatus() == VerticalLineObject::Delete)
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
      
    painter.setPen(co->getColor());

    painter.drawLine (x, 0, x, buffer->height());
  
    co->clearSelectionArea();
    QPointArray array;
    array.putPoints(0,
  		    4,
		    x - (HANDLE_WIDTH / 2), 0,
		    x + (HANDLE_WIDTH / 2), 0,
		    x + (HANDLE_WIDTH / 2), buffer->height(),
		    x - (HANDLE_WIDTH / 2), buffer->height());
    co->setSelectionArea(new QRegion(array));
    
    if (co->getStatus() == VerticalLineObject::Selected)
    {
      co->clearGrabHandles();
    
      int t = (int) buffer->height() / 4;
    
      co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			    0,
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(x - (HANDLE_WIDTH / 2), 0, HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
      co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			    t,
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(x - (HANDLE_WIDTH / 2), t, HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
      co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			    t * 2,
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(x - (HANDLE_WIDTH / 2), t * 2, HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
      co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			    t * 3,
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(x - (HANDLE_WIDTH / 2), t * 3, HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    
      co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			    t * 4,
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
      painter.fillRect(x - (HANDLE_WIDTH / 2), t * 4, HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    }
  }

  painter.end();
}

void VerticalLine::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit VerticalLine"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile (helpFile);
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

void VerticalLine::addObject (Setting &set)
{
  VerticalLineObject *co = new VerticalLineObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void VerticalLine::newObject (QString ind, QString n)
{
  loadDefaults();
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place VerticalLine..."));
}

COPlugin::Status VerticalLine::pointerClick (QPoint point, BarDate x, double)
{
  if (status == None)
  {
    QDictIterator<VerticalLineObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->getStatus() == VerticalLineObject::Delete)
        continue;
    
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(VerticalLineObject::Selected);
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
	selected->setStatus(VerticalLineObject::Plot);
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
    VerticalLineObject *co = new VerticalLineObject(indicator, name, x);
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

void VerticalLine::pointerMoving (QPoint, BarDate x, double)
{
  if (status != Moving)
    return;
    
  selected->setDate(x);
  selected->setSaveFlag(TRUE);
  
  emit signalDraw();
  
  QString s = x.getDateString(TRUE);
  emit message(s);
}

void VerticalLine::saveObjects (QString chartPath)
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

  QDictIterator<VerticalLineObject> it(objects);
  for (; it.current(); ++it)
  {
    VerticalLineObject *co = it.current();
    
    if (co->getStatus() == VerticalLineObject::Delete)
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

void VerticalLine::loadDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultVerticalLineColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void VerticalLine::saveDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultVerticalLineColor";
  settings.writeEntry(s, defaultColor.name());
}

void VerticalLine::keyEvent (QKeyEvent *key)
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

void VerticalLine::moveObject ()
{
  status = Moving;
}

void VerticalLine::removeObject ()
{
  selected->setStatus(VerticalLineObject::Delete);
  selected = 0;
  status = None;
  emit signalDraw();
}

void VerticalLine::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double VerticalLine::getHigh ()
{
  return data->getMax();
}

double VerticalLine::getLow ()
{
  return data->getMin();
}

void VerticalLine::showMenu ()
{
  if (selected)
    menu->exec(QCursor::pos());
}

void VerticalLine::getNameList (QStringList &d)
{
  QDictIterator<VerticalLineObject> it(objects);
  for (; it.current(); ++it)
    d.append(it.current()->getName());
}

//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  VerticalLine *o = new VerticalLine;
  return ((COPlugin *) o);
}

