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

#include "BuyArrow.h"
#include "PrefDialog.h"
#include "DbPlugin.h"
#include "Config.h"
#include "../../../pics/delete.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/rename.xpm"
#include <qpainter.h>
#include <qsettings.h>
#include <qcursor.h>

BuyArrow::BuyArrow ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("green");
  objects.setAutoDelete(TRUE);
  helpFile = "buyarrow.html";
  
  menu->insertItem(QPixmap(edit), tr("&Edit BuyArrow"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move BuyArrow"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete BuyArrow"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

BuyArrow::~BuyArrow ()
{
}

void BuyArrow::draw (int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(buffer);
  
  QDictIterator<BuyArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    BuyArrowObject *co = it.current();
    
    if (co->getStatus() == BuyArrowObject::Delete)
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
    
    int y = scaler->convertToY(co->getValue());

    arrow.putPoints(0, 7, x, y,
                    x + 5, y + 5,
                    x + 2, y + 5,
                    x + 2, y + 11,
	            x - 2, y + 11,
	            x - 2, y + 5,
                    x - 5, y + 5);
    painter.setBrush(co->getColor());
    painter.drawPolygon(arrow, TRUE, 0, -1);

    co->clearSelectionArea();
    co->setSelectionArea(new QRegion(arrow));
    
    if (co->getStatus() == BuyArrowObject::Selected)
    {
      co->clearGrabHandles();
    
      co->setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			     y - HANDLE_WIDTH,
				     HANDLE_WIDTH,
				     HANDLE_WIDTH,
				     QRegion::Rectangle));
				   
      painter.fillRect(x - (HANDLE_WIDTH / 2),
                       y - HANDLE_WIDTH,
		       HANDLE_WIDTH,
		       HANDLE_WIDTH,
		       co->getColor());
    }
  }

  painter.end();
}

void BuyArrow::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit BuyArrow"));
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

void BuyArrow::addObject (Setting &set)
{
  BuyArrowObject *co = new BuyArrowObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void BuyArrow::newObject (QString ind, QString n)
{
  loadDefaults();
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place BuyArrow..."));
}

COPlugin::Status BuyArrow::pointerClick (QPoint point, BarDate x, double y)
{
  if (status == None)
  {
    QDictIterator<BuyArrowObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->getStatus() == BuyArrowObject::Delete)
        continue;
    
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(BuyArrowObject::Selected);
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
	selected->setStatus(BuyArrowObject::Plot);
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
    BuyArrowObject *co = new BuyArrowObject(indicator, name, x, y);
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

void BuyArrow::pointerMoving (QPoint, BarDate x, double y)
{
  if (status != Moving)
    return;
    
  selected->setDate(x);
  selected->setValue(y);
  selected->setSaveFlag(TRUE);
  
  emit signalDraw();
  
  QString s = x.getDateString(TRUE) + " " + QString::number(y);
  emit message(s);
}

void BuyArrow::saveObjects (QString chartPath)
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

  QDictIterator<BuyArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    BuyArrowObject *co = it.current();
    
    if (co->getStatus() == BuyArrowObject::Delete)
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

void BuyArrow::loadDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultBuyArrowColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void BuyArrow::saveDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultBuyArrowColor";
  settings.writeEntry(s, defaultColor.name());
}

void BuyArrow::keyEvent (QKeyEvent *key)
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

void BuyArrow::moveObject ()
{
  status = Moving;
}

void BuyArrow::removeObject ()
{
  selected->setStatus(BuyArrowObject::Delete);
  selected = 0;
  status = None;
  emit signalDraw();
}

void BuyArrow::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double BuyArrow::getHigh ()
{
  double high = -99999999.0;
  
  QDictIterator<BuyArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    BuyArrowObject *co = it.current();
    if (co->getValue() > high)
      high = co->getValue();
  }
  
  return high;
}

double BuyArrow::getLow ()
{
  double low = 99999999.0;
  
  QDictIterator<BuyArrowObject> it(objects);
  for (; it.current(); ++it)
  {
    BuyArrowObject *co = it.current();
    if (co->getValue() < low)
      low = co->getValue();
  }

  return low;
}

void BuyArrow::showMenu ()
{
  if (selected)
    menu->exec(QCursor::pos());
}

void BuyArrow::getNameList (QStringList &d)
{
  QDictIterator<BuyArrowObject> it(objects);
  for (; it.current(); ++it)
    d.append(it.current()->getName());
}

//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  BuyArrow *o = new BuyArrow;
  return ((COPlugin *) o);
}

