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

#include "Cycle.h"
#include "PrefDialog.h"
#include "DbPlugin.h"
#include "Config.h"
#include "../../../pics/delete.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/rename.xpm"
#include <qpainter.h>
#include <qsettings.h>
#include <qcursor.h>

Cycle::Cycle ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("red");
  objects.setAutoDelete(TRUE);
  defaultInterval = 10;
  helpFile = "cycle.html";
  
  menu->insertItem(QPixmap(edit), tr("&Edit Cycle"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move Cycle"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Cycle"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

Cycle::~Cycle ()
{
}

void Cycle::draw (QPixmap &buffer, Scaler &, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  QDictIterator<CycleObject> it(objects);
  for (; it.current(); ++it)
  {
    CycleObject *co = it.current();
    
    if (co->getStatus() == CycleObject::Delete)
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
      
    tpixelspace = pixelspace; // save this for adjusting the cycle interval
    
    int origx = x;
      
    painter.setPen(co->getColor());
    int interval = co->getInterval();
    co->clearSelectionArea();
    
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
      
        QPointArray array;
        array.putPoints(0,
  		        4,
		        x - (HANDLE_WIDTH / 2), buffer.height(),
		        x - (HANDLE_WIDTH / 2), buffer.height() - HANDLE_WIDTH,
		        x + (HANDLE_WIDTH / 2), buffer.height() - HANDLE_WIDTH,
		        x + (HANDLE_WIDTH / 2), buffer.height());
        co->setSelectionArea(new QRegion(array));
      }
      
      x = x + (interval * pixelspace);
    }
    
    if (co->getStatus() == CycleObject::Selected)
    {
      co->clearGrabHandles();
      
      x = origx;
      
      while (x <= buffer.width())
      {
        if ((x + (interval * pixelspace)) > 0)
	{
          co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			        buffer.height() - HANDLE_WIDTH,
				        HANDLE_WIDTH,
				        HANDLE_WIDTH,
				        QRegion::Rectangle));
					
          painter.fillRect(x - (HANDLE_WIDTH / 2),
	                   buffer.height() - HANDLE_WIDTH,
			   HANDLE_WIDTH,
			   HANDLE_WIDTH,
			   co->getColor());
	}
	
        x = x + (interval * pixelspace);
      }
    }
  }

  painter.end();
}

void Cycle::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString il = tr("Interval");
  QString sd = tr("Set Default");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Cycle"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  QColor color = selected->getColor();
  dialog->addColorItem(cl, pl, color);
  dialog->addIntItem(il, pl, selected->getInterval(), 1, 9999);
  dialog->addCheckItem(sd, pl, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    selected->setColor(color);
    selected->setInterval(dialog->getInt(il));
    
    bool f = dialog->getCheck(sd);
    if (f)
    {
      defaultColor = color;
      saveDefaults();
    }
    
    emit signalDraw();
  }
  
  delete dialog;
}

void Cycle::addObject (Setting &set)
{
  CycleObject *co = new CycleObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void Cycle::newObject (QString &ind, QString &n)
{
  loadDefaults();
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select starting point of Cycle..."));
}

COPlugin::Status Cycle::pointerClick (QPoint &point, BarDate &x, double)
{
  if (status == None)
  {
    QDictIterator<CycleObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->getStatus() == CycleObject::Delete)
        continue;
    
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(CycleObject::Selected);
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
      tpoint = point;
      tpoint.setX(point.x() - (selected->getInterval() * tpixelspace));
      status = Moving;
      return status;
    }
    else
    {
      if (! selected->isSelected(point))
      {
        status = None;
	selected->setStatus(CycleObject::Plot);
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
    CycleObject *co = new CycleObject(indicator, name, x, defaultInterval);
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

void Cycle::pointerMoving (QPixmap &, QPoint &p, BarDate &x, double)
{
  if (status != Moving)
    return;

  int gp = selected->getGrabPosition();
  if (gp == -1)
    return;
  
  if (gp == 0)
  {
    selected->setDate(x);
    selected->setSaveFlag(TRUE);
  
    QString s;
    x.getDateString(TRUE, s);
    emit message(s);
    emit signalDraw();
  }
  else
  {
    if (gp > 0)
    {
      if (p.x() <= tpoint.x())
        return;
	
      int t = (p.x() - tpoint.x()) / tpixelspace;
      if (t == 0)
	return;
	  
      selected->setInterval(t);
      selected->setSaveFlag(TRUE);
	
      QString s = tr("Interval") + "=" + QString::number(t);
      emit message(s);
      emit signalDraw();
    }
  }
}

void Cycle::saveObjects (QString &chartPath)
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

  QDictIterator<CycleObject> it(objects);
  for (; it.current(); ++it)
  {
    CycleObject *co = it.current();
    
    if (co->getStatus() == CycleObject::Delete)
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

void Cycle::loadDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultCycleColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);

  s = "/Qtstalker/DefaultCycleInterval";
  s = settings.readEntry(s);
  if (s.length())
    defaultInterval = s.toInt();
}

void Cycle::saveDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultCycleColor";
  settings.writeEntry(s, defaultColor.name());

  s = "/Qtstalker/DefaultCycleInterval";
  settings.writeEntry(s, QString::number(defaultInterval));
}

void Cycle::keyEvent (QKeyEvent *key)
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

void Cycle::moveObject ()
{
  status = Moving;
}

void Cycle::removeObject ()
{
  selected->setStatus(CycleObject::Delete);
  selected = 0;
  status = None;
  emit signalObjectDeleted();
  emit signalDraw();
}

void Cycle::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double Cycle::getHigh ()
{
  return data->getMax();
}

double Cycle::getLow ()
{
  return data->getMin();
}

void Cycle::showMenu ()
{
  if (selected)
    menu->exec(QCursor::pos());
}

void Cycle::getNameList (QStringList &d)
{
  d.clear();
  QDictIterator<CycleObject> it(objects);
  for (; it.current(); ++it)
    d.append(it.current()->getName());
}

//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  Cycle *o = new Cycle;
  return ((COPlugin *) o);
}

