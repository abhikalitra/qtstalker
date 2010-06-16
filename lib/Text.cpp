/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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
#include "Config.h"
#include "CODataBase.h"

#include "../pics/text.xpm"

#include <QObject>
#include <QPainter>

Text::Text ()
{
  _plugin = "Text";
  _price = 0;

  Config config;
  config.getData(Config::DefaultTextColor, _color);
  if (! _color.isValid())
  {
    _color.setNamedColor("white");
    config.setData(Config::DefaultTextColor, _color);
  }

  QString s;
  config.getData(Config::DefaultTextFont, s);
  if (s.isEmpty())
  {
    _font = QFont("Helvetica,9,50,0");
    config.setData(Config::DefaultTextFont, _font);
  }
  else
    config.getData(Config::DefaultTextFont, _font);

  config.getData(Config::DefaultTextLabel, _label);
  if (_label.isEmpty())
  {
    _label = _plugin;
    config.setData(Config::DefaultTextLabel, _label);
  }
}

void Text::draw (PlotData &pd, DateBar &dateBars, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x2 = dateBars.getX(_date);
  if (x2 == -1)
    return;

  int x = (x2 * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
  if (x == -1)
    return;

  painter.setFont(pd.plotFont);

  painter.setPen(_color);

  int y = scaler.convertToY(_price);

  painter.drawText(x, y, _label);

  QFontMetrics fm = painter.fontMetrics();
  clearSelectionArea();
  setSelectionArea(new QRegion(x,
		   y - fm.height(),
		   fm.width(_label, -1),
		   fm.height(),
		   QRegion::Rectangle));

  if (_selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - _handleWidth - 1,
		  y - (fm.height() / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x - _handleWidth - 1,
		     y - (fm.height() / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);
  }

  painter.end();
}

void Text::getInfo (Setting *info)
{
  QString k = QObject::tr("Type");
  QString d = QObject::tr("Text");
  info->setData(k, d);

  k = QObject::tr("Label");
  info->setData(k, _label);
}

void Text::dialog ()
{
  PrefDialog *dialog = new PrefDialog;
  QString s = QObject::tr("Edit VLine");
  dialog->setWindowTitle(s);
  s = QObject::tr("Settings");
  int page = 0;
  int pid = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Color");
  dialog->addColorItem(pid++, page, s, _color);

  s = QObject::tr("Price");
  dialog->addDoubleItem(pid++, page, s, _price);

  s = QObject::tr("Label");
  dialog->addTextItem(pid++, page, s, _label, QString());

  s = QObject::tr("Font");
  dialog->addFontItem(pid++, page, s, _font);

  int def = FALSE;
  s = QObject::tr("Default");
  dialog->addCheckItem(pid++, page, s, def);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pid = 0;
  dialog->getColor(pid++, _color);
  _price = dialog->getDouble(pid++);
  dialog->getText(pid++, _label);
  dialog->getFont(pid++, _font);
  def = dialog->getCheck(pid++);

  if (def)
  {
    Config config;
    config.setData((int) Config::DefaultTextColor, _color);
    config.setData(Config::DefaultTextLabel, _label);
    config.setData((int) Config::DefaultTextFont, _font);
  }

  _saveFlag = TRUE;
  
  delete dialog;
}

void Text::load (QSqlQuery &q)
{
  _id = q.value(0).toInt();
  _exchange = q.value(1).toString();
  _symbol = q.value(2).toString();
  _indicator = q.value(3).toString();
  _color.setNamedColor(q.value(5).toString()); // t1 field
  _date = QDateTime::fromString(q.value(6).toString(), Qt::ISODate); // t2 field
  _label = q.value(7).toString(); // t3 field

  QStringList l = q.value(8).toString().split(","); // t4 field
  _font.setFamily(l[0]);
  _font.setPointSize(l[1].toInt());
  _font.setWeight(l[2].toInt());
  _font.setItalic(l[3].toInt());
  _font.setBold(l[4].toInt());

  _price = q.value(25).toDouble(); // d1 field
}

void Text::save ()
{
  if (! _saveFlag)
    return;
  
  QString s = "INSERT OR REPLACE INTO chartObjects (id,exchange,symbol,indicator,plugin,t1,t2,d1,t3,t4) VALUES (";
  s.append(QString::number(_id));
  s.append(",'" + _exchange + "'");
  s.append(",'" + _symbol + "'");
  s.append(",'" + _indicator + "'");
  s.append(",'" + _plugin + "'");
  s.append(",'" + _color.name() + "'");
  s.append(",'" + _date.toString(Qt::ISODate) + "'");
  s.append("," + QString::number(_price));
  s.append(",'" + _label + "'");
  
  QStringList l;
  l << _font.family();
  l << QString::number(_font.pointSize());
  l << QString::number(_font.weight());
  l << QString::number(_font.italic ());
  l << QString::number(_font.bold ());
  s.append(",'" + l.join(",") + "'");

  s.append(")");

  CODataBase db;
  db.setChartObject(s);
  
  _saveFlag = FALSE;
}

void Text::create ()
{
  emit signalMessage(QString(QObject::tr("Select point to place Text...")));
}

int Text::create2 (QDateTime &x, double y)
{
  _saveFlag = TRUE;
  _date = x;
  _price = y;
  emit signalMessage(QString());
  return 0;
}

void Text::moving (QDateTime &x, double y, int)
{
  _saveFlag = TRUE;
  _date = x;
  _price = y;
  
  emit signalMessage(QString(x.toString(Qt::ISODate) + " " + QString::number(y)));
}

void Text::getIcon (QIcon &d)
{
  d = QIcon(text_xpm);
}

int Text::getHighLow (double &h, double &l)
{
  h = _price;
  l = _price;
  return 0;
}

int Text::inDateRange (QDateTime &startDate, QDateTime &endDate, DateBar &)
{
  int rc = FALSE;
  if (_date >= startDate && _date <= endDate)
    rc = TRUE;
  
  return rc;
}

int Text::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<PRICE>,<LABEL>,<COLOR>
  //  0    1      2      3       4       5

  if (l.count() != 6)
  {
    qDebug() << _plugin << "::CUS: invalid parm count" << l.count();
    return 1;
  }

  _date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _date.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid date" << l[2];
    return 1;
  }

  bool ok;
  _price = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::CUS: invalid price" << l[3];
    return 1;
  }

  _label = l[4];

  _color.setNamedColor(l[5]);
  if (! _color.isValid())
  {
    qDebug() << _plugin << "::CUS: invalid color" << l[5];
    return 1;
  }

  return 0;
}

