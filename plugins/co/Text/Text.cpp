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
#include "DbPlugin.h"
#include "Config.h"
#include "../../../pics/delete.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/rename.xpm"
#include <qpainter.h>
#include <qsettings.h>
#include <qcursor.h>

Text::Text ()
{
  status = None;
  selected = 0;
  defaultColor.setNamedColor("red");
  objects.setAutoDelete(TRUE);
  helpFile = "text.html";
  
  Config config;  
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
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

void Text::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
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
    
    int y = scaler.convertToY(co->getValue());

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
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString fl = tr("Font");
  QString ll = tr("Label");
  QString vl = tr("Value");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Text"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  QColor color = selected->getColor();
  dialog->addColorItem(cl, pl, color);
  QFont f = selected->getFont();
  dialog->addFontItem(fl, pl, f);
  QString t = selected->getLabel();
  dialog->addTextItem(ll, pl, t);
  dialog->addFloatItem(vl, pl, selected->getValue());
  dialog->addCheckItem(sd, pl, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    selected->setColor(dialog->getColor(cl));
    selected->setLabel(dialog->getText(ll));
    selected->setFont(dialog->getFont(fl));
    selected->setValue(dialog->getFloat(vl));
    
    selected->setSaveFlag(TRUE);
    
    bool f = dialog->getCheck(sd);
    if (f)
    {
      defaultColor = dialog->getColor(cl);
      font = dialog->getFont(fl);
      saveDefaults();
    }
    
    emit signalDraw();
  }
  
  delete dialog;
}

void Text::addObject (Setting &set)
{
  TextObject *co = new TextObject;
  co->setSettings(set);
  objects.replace(co->getName(), co);
}

void Text::newObject (QString &ind, QString &n)
{
  loadDefaults();
  indicator = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place Text..."));
}

COPlugin::Status Text::pointerClick (QPoint &point, BarDate &x, double y)
{
  if (status == None)
  {
    QDictIterator<TextObject> it(objects);
    for (; it.current(); ++it)
    {
      if (it.current()->getStatus() == TextObject::Delete)
        continue;
    
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

void Text::pointerMoving (QPixmap &, QPoint &, BarDate &x, double y)
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

void Text::saveObjects (QString &chartPath)
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

  QDictIterator<TextObject> it(objects);
  for (; it.current(); ++it)
  {
    TextObject *co = it.current();
    
    if (co->getStatus() == TextObject::Delete)
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

void Text::showMenu ()
{
  if (selected)
    menu->exec(QCursor::pos());
}

void Text::getNameList (QStringList &d)
{
  d.clear();
  QDictIterator<TextObject> it(objects);
  for (; it.current(); ++it)
    d.append(it.current()->getName());
}

//****************************************************
//****************************************************
//****************************************************

COPlugin * createCOPlugin ()
{
  Text *o = new Text;
  return ((COPlugin *) o);
}

