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

#include "SellArrow.h"
#include "PrefDialog.h"
#include "DbPlugin.h"
#include "Config.h"
#include "../../../pics/delete.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/rename.xpm"
#include <qpainter.h>
#include <qsettings.h>
#include <qcursor.h>

SellArrow::SellArrow ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("red");
  objects.setAutoDelete(TRUE);
  helpFile = "sellarrow.html";
  
  menu->insertItem(QPixmap(edit), tr("&Edit SellArrow"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move SellArrow"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete SellArrow"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

SellArrow::~SellArrow ()
{
}

void SellArrow::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  QDictIterator<SellArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    SellArrowObject *co = it.current();
    
    if (co->getStatus() == SellArrowObject::Delete)
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
    
    int y = scaler.convertToY(co->getValue());

    arrow.putPoints(0, 7, x, y,
                    x + 5, y - 5,
	            x + 2, y - 5,
	            x + 2, y - 11,
	            x - 2, y - 11,
	            x - 2, y - 5,
                    x - 5, y - 5);
    painter.setBrush(co->getColor());
    painter.drawPolygon(arrow, TRUE, 0, -1);

    co->clearSelectionArea();
    co->setSelectionArea(new QRegion(arrow));
    
    if (co->getStatus() == SellArrowObject::Selected)
    {
      co->clearGrabHandles();
    
      co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			    y + 1,
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
				   
      painter.fillRect(x - (HANDLE_WIDTH / 2), y + 1, HANDLE_WIDTH, HANDLE_WIDTH, co->getColor());
    }
  }

  painter.end();
}

void SellArrow::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString vl = tr("Value");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit SellArrow"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  QColor color = selected->getColor();
  dialog->addColorItem(cl, pl, color);
  dialog->addFloatItem(vl, pl, selected->getValue());
  dialog->addCheckItem(sd, pl, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    selected->setColor(color);
    selected->setValue(dialog->getFloat(vl));
    
    selected->setSaveFlag(TRUE);
    
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

void SellArrow::addObject (Setting &set)
{
  SellArrowObject *co = new SellArrowObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void SellArrow::newObject (QString &ind, QString &n)
{
  loadDefaults();
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place SellArrow..."));
}

COPlugin::Status SellArrow::pointerClick (QPoint &point, BarDate &x, double y)
{
  if (status == None)
  {
    QDictIterator<SellArrowObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->getStatus() == SellArrowObject::Delete)
        continue;
    
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(SellArrowObject::Selected);
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
	selected->setStatus(SellArrowObject::Plot);
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
    SellArrowObject *co = new SellArrowObject(indicator, name, x, y);
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

void SellArrow::pointerMoving (QPixmap &, QPoint &, BarDate &x, double y)
{
  if (status != Moving)
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

void SellArrow::saveObjects (QString &chartPath)
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

  QDictIterator<SellArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    SellArrowObject *co = it.current();
    
    if (co->getStatus() == SellArrowObject::Delete)
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

void SellArrow::loadDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultSellArrowColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void SellArrow::saveDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultSellArrowColor";
  settings.writeEntry(s, defaultColor.name());
}

void SellArrow::keyEvent (QKeyEvent *key)
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

void SellArrow::moveObject ()
{
  status = Moving;
}

void SellArrow::removeObject ()
{
  selected->setStatus(SellArrowObject::Delete);
  selected = 0;
  status = None;
  emit signalDraw();
}

void SellArrow::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double SellArrow::getHigh ()
{
  double high = -99999999.0;
  
  QDictIterator<SellArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    SellArrowObject *co = it.current();
    if (co->getValue() > high)
      high = co->getValue();
  }
  
  return high;
}

double SellArrow::getLow ()
{
  double low = 99999999.0;
  
  QDictIterator<SellArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    SellArrowObject *co = it.current();
    if (co->getValue() < low)
      low = co->getValue();
  }

  return low;
}

void SellArrow::showMenu ()
{
  if (selected)
    menu->exec(QCursor::pos());
}

void SellArrow::getNameList (QStringList &d)
{
  d.clear();
  QDictIterator<SellArrowObject> it(objects);
  for (; it.current(); ++it)
    d.append(it.current()->getName());
}


//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  SellArrow *o = new SellArrow;
  return ((COPlugin *) o);
}

