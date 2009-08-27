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

#include "COBase.h"
#include "../pics/delete.xpm"
#include "../pics/edit.xpm"
//#include "../pics/rename.xpm"
#include "BuyArrow.h"
#include "Cycle.h"
#include "FiboLine.h"
#include "HorizontalLine.h"
#include "SellArrow.h"
#include "Text.h"
#include "TrendLine.h"
#include "VerticalLine.h"
#include <QCursor>
#include <QPixmap>
#include <QKeyEvent>
#include <QMenu>

COBase::COBase ()
{
  data = 0;
  menu = new QMenu();
  status = None;
  saveFlag = FALSE;
  date = QDateTime::currentDateTime();
  dateFormat = "yyyy-MM-dd HH:mm:ss";

  menu->addAction(QPixmap(edit), tr("&Edit Object"), this, SLOT(prefDialog()), Qt::CTRL+Qt::Key_E);
//  menu->insertItem(QPixmap(renam), tr("&Move Object"), this, SLOT(moveObject()), Qt::CTRL+Qt::Key_M);
  menu->addAction(QPixmap(deleteitem), tr("&Delete Object"), this, SLOT(removeObject()), Qt::CTRL+Qt::Key_D);
}

COBase::~COBase ()
{
  delete menu;
  qDeleteAll(grabHandles);
  qDeleteAll(selectionArea);
}

void COBase::draw (QPixmap &, Scaler &, int, int, int)
{
}

void COBase::prefDialog ()
{
}

void COBase::showMenu ()
{
  menu->exec(QCursor::pos());
}

void COBase::setData (BarData *d)
{
  data = d;
}

void COBase::newObject (QString &, QString &)
{
}

COBase::Status COBase::pointerClick (QPoint &, QDateTime &, double)
{
  return COBase::None;
}

void COBase::pointerMoving (QPixmap &, QPoint &, QDateTime &, double)
{
}

void COBase::keyEvent (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_M:
//      if (key->state() == Qt::ControlButton)
        moveObject();
      break;
    case Qt::Key_E:
//      if (key->state() == Qt::ControlButton)
        prefDialog();
      break;
    case Qt::Key_D:
//      if (key->state() == Qt::ControlButton)
        removeObject();
      break;
    default:
      key->ignore();
      break;
  }
}

void COBase::loadSettings (COSettings &)
{
}

void COBase::saveSettings ()
{
}

double COBase::getHigh ()
{
  return value;
}

double COBase::getLow ()
{
  return value;
}

QString COBase::getHelpFile ()
{
  return helpFile;
}

void COBase::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool COBase::getSaveFlag ()
{
  return saveFlag;
}

void COBase::clearGrabHandles ()
{
  qDeleteAll(grabHandles);
  grabHandles.clear();
}

void COBase::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void COBase::clearSelectionArea ()
{
  qDeleteAll(selectionArea);
  selectionArea.clear();
}

void COBase::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString COBase::getID ()
{
  return id;
}

void COBase::setStatus (Status d)
{
  status = d;
}

COBase::Status COBase::getStatus ()
{
  return status;
}

bool COBase::isSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) selectionArea.count(); loop++)
  {
    QRegion *r = selectionArea.at(loop);
    if (r->contains(point))
      return TRUE;
  }
  
  return FALSE;
}

bool COBase::isGrabSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return TRUE;
  }
  
  return FALSE;
}

void COBase::removeObject ()
{
  emit signalObjectDeleted(id);
}

void COBase::moveObject ()
{
  status = Moving;
}

COBase * COBase::getCO (int s)
{
  COBase *t = 0;

  switch (s)
  {
    case COBuyArrow:
      t = new BuyArrow();
      break;
    case COCycle:
      t = new Cycle();
      break;
    case COFiboLine:
      t = new FiboLine();
      break;
    case COHorizontalLine:
      t = new HorizontalLine();
      break;
    case COSellArrow:
      t = new SellArrow();
      break;
    case COText:
      t = new Text();
      break;
    case COTrendLine:
      t = new TrendLine();
      break;
    case COVerticalLine:
      t = new VerticalLine();
      break;
    default:
      break;
  }

  return t;
}

void COBase::setSymbol (QString &d)
{
  symbol = d;
}

