/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
  defaultColor.setNamedColor("green");
  helpFile = "buyarrow.html";
  color.setNamedColor("green");
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
  
  menu->insertItem(QPixmap(edit), tr("&Edit BuyArrow"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move BuyArrow"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete BuyArrow"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

BuyArrow::~BuyArrow ()
{
}

void BuyArrow::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  if (! date.getDate().isValid())
    continue;
    
  int x2 = data->getX(date);
  if (x2 == -1)
    continue;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
      continue;
    
  int y = scaler.convertToY(value);

  arrow.putPoints(0, 7, x, y,
                  x + 5, y + 5,
                  x + 2, y + 5,
                  x + 2, y + 11,
                  x - 2, y + 11,
	          x - 2, y + 5,
                  x - 5, y + 5);
  painter.setBrush(color);
  painter.drawPolygon(arrow, TRUE, 0, -1);

  clearSelectionArea();
  setSelectionArea(new QRegion(arrow));
    
  if (status == COBase::Selected)
  {
    clearGrabHandles();
    
    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             			   y - HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
				   
    painter.fillRect(x - (HANDLE_WIDTH / 2),
                     y - HANDLE_WIDTH,
		     HANDLE_WIDTH,
		     HANDLE_WIDTH,
		     color);
  }

  painter.end();
}

void BuyArrow::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString vl = tr("Value");
  QString sd = tr("Set Default");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit BuyArrow"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addDoubleItem(vl, pl, value);
  dialog->addCheckItem(sd, pl, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    value = dialog->getDouble(vl);
    saveFlag = TRUE;
    
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

void BuyArrow::addObject (Setting &set)
{
  setSettings(set);
}

void BuyArrow::newObject (QString &ind, QString &n)
{
  loadDefaults();
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place BuyArrow..."));
}

COPlugin::Status BuyArrow::pointerClick (QPoint &point, BarDate &x, double y)
{
  if (status == None)
  {
    if (isSelected(point))
    {
      status = Selected;
      setStatus(COBase::Selected);
      emit signalDraw();
      return status;
    }
    else
      return status;
  }
  
  if (status == Selected)
  {
    if (isGrabSelected(point))
    {
      status = Moving;
      return status;
    }
    else
    {
      if (! isSelected(point))
      {
        status = None;
	setStatus(COBase::Plot);
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
    setSaveFlag(TRUE);
    setColor(defaultColor);
    emit signalDraw();
    status = None;
    emit message("");
    return status;
  }
    
  return status;    
}

void BuyArrow::pointerMoving (QPixmap &, QPoint &, BarDate &x, double y)
{
  if (status != Moving)
    return;
    
  setDate(x);
  setValue(y);
  setSaveFlag(TRUE);
  
  emit signalDraw();
  
  QString s;
  x.getDateString(TRUE, s);
  s = s + " " + QString::number(y);
  emit message(s);
}

void BuyArrow::saveObjects (QString &chartPath)
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

  if (getSaveFlag())
  {
    Setting set;
    getSettings(set);
    QString s = getName();
    db->setChartObject(s, set);
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
  emit signalObjectDeleted(name);
}

void BuyArrow::getSettings (Setting &set)
{
  QString s;
  date.getDateTimeString(FALSE, s);
  set.setData("Date", s);
  set.setData("Value", QString::number(value));
  set.setData("Color", color.name());
  set.setData("Plot", plot);
  set.setData("Name", name);
  set.setData("Type", "BuyArrow");
}

void BuyArrow::setSettings (Setting &set)
{
  QString s = set.getData("Date");
  date.setDate(s);
  value = set.getDouble("Value");
  color.setNamedColor(set.getData("Color"));
  plot = set.getData("Plot");
  name = set.getData("Name");
}


