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
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qapplication.h>

Text::Text (Scaler *s, QPixmap *p, QString indicator, QString n, BarDate d, double v)
{
  scaler = s;
  buffer = p;
  type = ChartObject::Text;
  plot = indicator;
  name = n;
  date = d;
  value = v;
  label = tr("Text");
  
  menu->insertItem(tr("Edit Text"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Move Text"), this, SLOT(moveObject()));
  menu->insertItem(tr("Delete Text"), this, SLOT(remove()));
  
  loadDefaults("Text");
}

Text::~Text ()
{
}

void Text::draw (int x, int)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setFont(font);
  painter.setPen(color);

  int y = scaler->convertToY(value);

  painter.drawText(x, y, label);
  
  QFontMetrics fm = painter.fontMetrics();
  selectionArea.clear();
  selectionArea.append(new QRegion(x, y - fm.height(), fm.width(label, -1), fm.height(), QRegion::Rectangle));
  
  if (status)
  {
    grabHandles.clear();

    grabHandles.append(new QRegion(x + fm.width(label, -1),
             			   y - (fm.height() / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(x + fm.width(label, -1), y - (fm.height() / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
  }

  painter.end();
}

void Text::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Text"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), tr("Details"), color);
  dialog->addFontItem(tr("Font"), tr("Details"), font);
  dialog->addTextItem(tr("Label"), tr("Details"), label);
  dialog->addCheckItem(tr("Set Default"), tr("Details"), FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    label = dialog->getText(tr("Label"));
    font = dialog->getFont(tr("Font"));
    
    bool f = dialog->getCheck(tr("Set Default"));
    if (f)
      saveDefaults("Text");
    
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void Text::move (BarDate d, double v)
{
  date = d;
  value = v;
  saveFlag = TRUE;
  emit signalDraw();
  
  QString s = d.getDateString(TRUE) + " " + QString::number(v);
  emit message(s);
}

Setting * Text::getSettings ()
{
  Setting *set = new Setting;
  set->setData("Date", date.getDateTimeString(FALSE));
  set->setData("Value", QString::number(value));
  set->setData("Color", color.name());
  set->setData("Plot", plot);
  set->setData("Name", name);
  set->setData("ObjectType", QString::number(type));
  set->setData("Label", label);

  QString s = font.family();
  s.append(" ");
  s.append(QString::number(font.pointSize()));
  s.append(" ");
  s.append(QString::number(font.weight()));
  set->setData("Font", s);
  
  return set;
}

void Text::setSettings (Setting *set)
{
  date.setDate(set->getData("Date"));
  value = set->getFloat("Value");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
  label = set->getData("Label");
  
  QStringList l = QStringList::split(" ", set->getData("Font"), FALSE);
  if (l.count())
    font = QFont(l[0], l[1].toInt(), l[2].toInt());
}

