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

#include "SELL.h"
#include "SellDialog.h"
#include "Globals.h"
#include "DataDataBase.h"
#include "ConfirmDialog.h"
#include "DateScaleDraw.h"

#include "../pics/delete.xpm"
#include "../pics/edit.xpm"

#include <QtDebug>
#include <QSettings>
#include <qwt_plot.h>

SELL::SELL ()
{
  _plugin = "SELL";
  _type = "CHART_OBJECT";
  _status = _NONE;
  _dialog = 0;
  _selected = 0;

  _menu = new QMenu;
  _editAction = _menu->addAction(QPixmap(edit_xpm), tr("&Edit"), this, SLOT(dialog()), Qt::ALT+Qt::Key_E);
  _deleteAction = _menu->addAction(QPixmap(delete_xpm), tr("&Delete"), this, SLOT(deleteChartObject()), Qt::ALT+Qt::Key_D);
}

SELL::~SELL ()
{
  delete _menu;
  if (_dialog)
    delete _dialog;
  qDeleteAll(_items);
}

int SELL::request (Setting *request, Setting *data)
{
  QStringList l;
  l << "INFO" << "HIGH_LOW" << "CREATE" << "CLEAR" << "ADD" << "DELETE_ALL";

  switch (l.indexOf(request->data("REQUEST")))
  {
    case 0:
      return info(data);
      break;
    case 1:
      return highLow(request, data);
      break;
    case 2:
      return create(request);
      break;
    case 3:
      return clear();
      break;
    case 4:
      return addItem(data);
      break;
    case 5:
      return deleteAll();
      break;
    default:
      return 1;
      break;
  }
}

void SELL::setParent (void *p)
{
  _plot = (QwtPlot *) p;
}

int SELL::clear ()
{
  qDeleteAll(_items);
  _items.clear();
  _status = _NONE;
  return 0;
}

int SELL::addItem (Setting *data)
{
  SellDraw *co = new SellDraw;
  data->copy(co->settings());
  co->attach(_plot);
  _items.insert(co->settings()->data("ID"), co);
  return 0;
}

int SELL::info (Setting *info)
{
  SellDraw *item = _items.value(info->data("ID"));
  if (! item)
    return 1;

  Setting *set = item->settings();
  if (! set)
    return 1;
  
  info->setData(tr("Type"), tr("Sell"));

  QDateTime dt = set->dateTime("DATE");
  info->setData("D", dt.toString("yyyy-MM-dd"));
  info->setData("T", dt.toString("HH:mm:ss"));

  info->setData(tr("Price"), set->data("PRICE"));

  return 0;
}

int SELL::highLow (Setting *request, Setting *data)
{
  double high = 0;
  double low = 0;
  int flag = 0;
  int start = request->getInt("START");
  int end = request->getInt("END");
  
  QHashIterator<QString, SellDraw *> it(_items);
  while (it.hasNext())
  {
    it.next();

    Setting *set = it.value()->settings();
    if (! set)
      continue;
    
    DateScaleDraw *dsd = (DateScaleDraw *) _plot->axisScaleDraw(QwtPlot::xBottom);
    if (! dsd)
      continue;
    
    int x = dsd->x(set->dateTime("DATE"));

    if (x < start || x > end)
      continue;

    double d = set->getDouble("PRICE");
    if (d > high)
      high = d;
    
    if (! flag)
    {
      low = d;
      flag++;
    }
    else
    {
      if (d < low)
        low = d;
    }
  }

  if (! flag)
    return 1;
  
  data->setData("HIGH", high);
  data->setData("LOW", low);
  
  return 0;
}

void SELL::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      QwtScaleMap map = _selected->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) _selected->plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);

      Setting *set = _selected->settings();
      if (! set)
	break;
      set->setData("DATE", dt);

      map = _selected->plot()->canvasMap(QwtPlot::yRight);
      set->setData("PRICE", map.invTransform((double) p.y()));

      _plot->replot();
      break;
    }
    default:
      break;
  }
}

void SELL::click (int button, QPoint p)
{
//  if (_settings->getInt("RO"))
//    return;

  switch (_status)
  {
    case _SELECTED:
    {
      switch (button)
      {
        case Qt::LeftButton:
          if (_selected->isGrabSelected(p))
          {
            _status = _MOVE;
            return;
          }

          if (! _selected->isSelected(p))
          {
            _status = _NONE;
            _selected->setSelected(FALSE);
            _plot->replot();
            emit signalUnselected();
            return;
          }
          break;
        case Qt::RightButton:
	  if (! _dialog)
            _menu->exec(QCursor::pos());
          break;
        default:
          break;
      }

      break;
    }
    case _MOVE:
    {
      switch (button)
      {
        case Qt::LeftButton:
          _status = _SELECTED;
          save();
          return;
        default:
          break;
      }

      break;
    }
    default: // _None
    {
      switch (button)
      {
        case Qt::LeftButton:
	{
          QHashIterator<QString, SellDraw *> it(_items);
          while (it.hasNext())
          {
            it.next();
            if (it.value()->isSelected(p))
            {
              _status = _SELECTED;
	      _selected = it.value();
              it.value()->setSelected(TRUE);
              emit signalSelected();
              _plot->replot();
	      return;
	    }
          }
          break;
	}
        default:
          break;
      }

      break;
    }
  }
}

int SELL::create (Setting *parms)
{
  _selected = new SellDraw;
  Setting *set = _selected->settings();
  set->setData("KEY", parms->data("KEY"));
  
  QString id = QUuid::createUuid().toString();
  id = id.remove(QString("{"), Qt::CaseSensitive);
  id = id.remove(QString("}"), Qt::CaseSensitive);
  id = id.remove(QString("-"), Qt::CaseSensitive);
  set->setData("ID", id);

  _selected->attach(_plot);
  _items.insert(id, _selected);
  _status = _MOVE;
  _selected->setSelected(TRUE);
  g_middleMan->statusMessage(tr("Place Sell object..."));

  return 0;
}

void SELL::dialog ()
{
  if (_dialog)
    return;

  _dialog = new Dialog(g_parent);
  QWidget *w = new SellDialog(_dialog, _selected->settings());
  connect(_dialog, SIGNAL(accepted()), w, SLOT(save()));
  _dialog->setWidget(w);
  connect(_dialog, SIGNAL(accepted()), this, SLOT(dialogOK()));
  connect(_dialog, SIGNAL(finished(int)), this, SLOT(dialogCancel()));
  _dialog->show();
}

void SELL::dialogCancel ()
{
  _dialog = 0;
}

void SELL::dialogOK ()
{
  save();
  _plot->replot();
}

void SELL::deleteChartObject ()
{
  ConfirmDialog *dialog = new ConfirmDialog(g_parent);
  dialog->setMessage(tr("Confirm chart object delete"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteChartObject2()));
  dialog->show();
}

void SELL::deleteChartObject2 ()
{
  QString id = _selected->settings()->data("ID");
  delete _selected;
  _items.remove(id);
  _status = _NONE;

  DataDataBase db("chartObjects");
  db.transaction();
  db.removeName(id);
  db.commit();

  _plot->replot();
}

int SELL::deleteAll ()
{
  DataDataBase db("chartObjects");
  db.transaction();

  QHashIterator<QString, SellDraw *> it(_items);
  while (it.hasNext())
  {
    it.next();
    db.removeName(it.key());
  }

  db.commit();

  clear();

  _plot->replot();

  return 0;
}

void SELL::load ()
{
/*  
  if (_settings->getInt("RO"))
    return;

  DataDataBase db("chartObjects");
  if (db.load(_settings->data("ID"), _settings))
    qDebug() << "ChartObject::load: load error";
*/
}

void SELL::save ()
{
  Setting *set = _selected->settings();
  if (set->getInt("RO"))
    return;

  DataDataBase db("chartObjects");
  db.transaction();

  if (db.removeName(set->data("ID")))
  {
    qDebug() << "ChartObject::save: remove error";
    return;
  }

  if (db.save(set->data("ID"), set))
    qDebug() << "ChartObject::save: save error";

  db.commit();
}

void SELL::update ()
{
  _plot->replot();
}

int SELL::calculate (BarData *, Indicator *i, Setting *settings)
{
  int rows = settings->getInt("ROWS");
  int loop = 0;
  for (; loop < rows; loop++)
  {
    // output
    int col = 0;

    Setting co;
    QString key = "-" + QString::number(i->chartObjectCount() + 1);
    co.setData("ID", key);

    co.setData("PLUGIN", _plugin);
    co.setData("TYPE", settings->data("TYPE"));
    co.setData("RO", 1);

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    co.setData("DATE", settings->data(key));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    co.setData("PRICE", settings->data(key));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    co.setData("COLOR", settings->data(key));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    co.setData("Z", settings->data(key));
    if (settings->getInt(key) < 0)
      continue;

    i->addChartObject(co);
  }

  return 0;
}

int SELL::command (Command *command)
{
  // PARMS:
  // TYPE
  // NAME
  // INDICATOR
  // EXCHANGE
  // SYMBOL
  // DATE
  // PRICE
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify TYPE
  QString type = command->parm("TYPE");
  if (type.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid TYPE" << type;
    return 1;
  }

  QStringList typeList;
  typeList << "RO" << "RW";
  SellDraw tco;
  Setting *co = tco.settings();
  int saveFlag = 0;
  switch (typeList.indexOf(type))
  {
    case 0: // RO
    {
      QString key = "-" + QString::number(i->chartObjectCount() + 1);
      co->setData("PLUGIN", _plugin);
      co->setData("TYPE", QString("Sell"));
      co->setData("ID", key);
      co->setData("RO", 1);
      break;
    }
    case 1:
      co->setData("PLUGIN", _plugin);
      co->setData("TYPE", QString("Sell"));
      co->setData("ID", command->parm("NAME"));
      co->setData("INDICATOR", command->parm("INDICATOR"));
      co->setData("EXCHANGE", command->parm("EXCHANGE"));
      co->setData("SYMBOL", command->parm("SYMBOL"));
      saveFlag++;
      break;
    default:
      qDebug() << _plugin << "::command: invalid TYPE" << type;
      return 1;
      break;
  }

  // verify date
  QDateTime dt = QDateTime::fromString(command->parm("DATE"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE" << command->parm("DATE");
    return 1;
  }
  co->setData("DATE", command->parm("DATE"));

  // verify price
  bool ok;
  command->parm("PRICE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PRICE" << command->parm("PRICE");
    return 1;
  }
  co->setData("PRICE", command->parm("PRICE"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  co->setData("COLOR", command->parm("COLOR"));

  if (saveFlag)
    save();

  Setting set;
  co->copy(&set);
  i->addChartObject(set);

  command->setReturnCode("0");

  return 0;
}

QWidget * SELL::dialog (QWidget *p, Setting *set)
{
  return new SellDialog(p, set);
}

void SELL::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SELL *o = new SELL;
  return ((Plugin *) o);
}
