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

#include "Text.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "Config.h"
#include "../../../src/delete.xpm"
#include "../../../src/edit.xpm"
#include "../../../src/rename.xpm"
#include <qpainter.h>
#include <qsettings.h>

Text::Text ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("white");
  objects.setAutoDelete(TRUE);
  
  Config config;  
  QStringList l = QStringList::split(" ", config.getData(Config::PlotFont), FALSE);
  QFont f(l[0], l[1].toInt(), l[2].toInt());
  font = f;
  
  menu->insertItem(QPixmap(edit), tr("&Edit Text"), this, SLOT(prefDialog()), CTRL+Key_E);
  menu->insertItem(QPixmap(renam), tr("&Move Text"), this, SLOT(moveObject()), CTRL+Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Text"), this, SLOT(removeObject()), CTRL+Key_D);
  
  loadDefaults();
}

Text::~Text ()
{
}

void Text::draw (int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(buffer);
  
  QDictIterator<TextObject> it(objects);
  for (; it.current(); ++it)
  {
    TextObject *co = it.current();
    
    if (co->getStatus() == TextObject::Delete)
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
      
    painter.setFont(co->getFont());
    painter.setPen(co->getColor());
    
    int y = scaler->convertToY(co->getValue());

    painter.drawText(x, y, co->getLabel());
  
    QFontMetrics fm = painter.fontMetrics();
    co->clearSelectionArea();
    co->setSelectionArea(new QRegion(x,
                                     y - fm.height(),
				     fm.width(co->getLabel(), -1),
				     fm.height(),
				     QRegion::Rectangle));
    
    if (co->getStatus() == TextObject::Selected)
    {
      co->clearGrabHandles();
    
      co->setGrabHandle(new QRegion(x + fm.width(co->getLabel(), -1),
             			    y - (fm.height() / 2),
				    HANDLE_WIDTH,
				    HANDLE_WIDTH,
				    QRegion::Rectangle));
				    
      painter.fillRect(x + fm.width(co->getLabel(), -1),
                       y - (fm.height() / 2),
		       HANDLE_WIDTH,
		       HANDLE_WIDTH,
		       co->getColor());
    }
  }

  painter.end();
}

void Text::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Text"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), tr("Details"), selected->getColor());
  dialog->addFontItem(tr("Font"), tr("Details"), selected->getFont());
  dialog->addTextItem(tr("Label"), tr("Details"), selected->getLabel());
  dialog->addCheckItem(tr("Set Default"), tr("Details"), FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    selected->setColor(dialog->getColor(tr("Color")));
    selected->setLabel(dialog->getText(tr("Label")));
    selected->setFont(dialog->getFont(tr("Font")));
    
    bool f = dialog->getCheck(tr("Set Default"));
    if (f)
    {
      defaultColor = dialog->getColor(tr("Color"));
      font = dialog->getFont(tr("Font"));
      saveDefaults();
    }
    
    emit signalDraw();
  }
  
  delete dialog;
}

void Text::addObject (Setting *set)
{
  TextObject *co = new TextObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void Text::newObject (QString ind, QString n)
{
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place Text..."));
}

COPlugin::Status Text::pointerClick (QPoint point, BarDate x, double y)
{
  if (status == None)
  {
    QDictIterator<TextObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->isSelected(point))
      {
        selected = it.current();
        status = Selected;
	selected->setStatus(TextObject::Selected);
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
	selected->setStatus(TextObject::Plot);
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
    TextObject *co = new TextObject(indicator, name, x, y, "Text", font);
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

void Text::pointerMoving (QPoint, BarDate x, double y)
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

void Text::saveObjects (QString chartPath)
{
  if (! chartPath.length())
    return;

  ChartDb *db =  new ChartDb;
  db->openChart(chartPath);

  QDictIterator<TextObject> it(objects);
  for (; it.current(); ++it)
  {
    TextObject *co = it.current();
    
    if (co->getStatus() == TextObject::Delete)
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

void Text::loadDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultTextColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);

  s = "/Qtstalker/DefaultTextFont";
  s = settings.readEntry(s);
  if (s.length())
  {
    QStringList l = QStringList::split(",", s, FALSE);
    font = QFont(l[0], l[1].toInt(), l[2].toInt());
  }
}

void Text::saveDefaults ()
{
  QSettings settings;
  
  QString s = "/Qtstalker/DefaultTextColor";
  settings.writeEntry(s, defaultColor.name());
  
  s = "/Qtstalker/DefaultTextFont";
  settings.writeEntry(s, font.family() + "," +
                      QString::number(font.pointSize()) + "," +
		      QString::number(font.weight()));
}

void Text::keyEvent (QKeyEvent *key)
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

void Text::moveObject ()
{
  status = Moving;
}

void Text::removeObject ()
{
  selected->setStatus(TextObject::Delete);
  selected = 0;
  status = None;
  emit signalDraw();
}

void Text::clear ()
{
  objects.clear();
  status = None;
  selected = 0;
}

double Text::getHigh ()
{
  double high = -99999999.0;
  
  QDictIterator<TextObject> it(objects);
  for (; it.current(); ++it)
  {
    TextObject *co = it.current();
    if (co->getValue() > high)
      high = co->getValue();
  }
  
  return high;
}

double Text::getLow ()
{
  double low = 99999999.0;
  
  QDictIterator<TextObject> it(objects);
  for (; it.current(); ++it)
  {
    TextObject *co = it.current();
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
  Text *o = new Text;
  return ((COPlugin *) o);
}

