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

#include "HLINE.h"
#include "HLineDialog.h"
#include "Globals.h"
#include "DataDataBase.h"
#include "ConfirmDialog.h"
#include "DateScaleDraw.h"
#include "RuleWidget.h"

#include "../pics/delete.xpm"
#include "../pics/edit.xpm"

#include <QtDebug>
#include <QSettings>
#include <qwt_plot.h>

HLINE::HLINE ()
{
  _plugin = "HLINE";
  _type = "CHART_OBJECT";
  _status = _NONE;
  _dialog = 0;
  _selected = 0;

  _menu = new QMenu;
  _editAction = _menu->addAction(QPixmap(edit_xpm), tr("&Edit"), this, SLOT(dialog()), Qt::ALT+Qt::Key_E);
  _deleteAction = _menu->addAction(QPixmap(delete_xpm), tr("&Delete"), this, SLOT(deleteChartObject()), Qt::ALT+Qt::Key_D);
}

HLINE::~HLINE ()
{
  delete _menu;
  if (_dialog)
    delete _dialog;
  qDeleteAll(_items);
}

int HLINE::request (Setting *request, Setting *data)
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

void HLINE::setParent (void *p)
{
  _plot = (QwtPlot *) p;
}

int HLINE::clear ()
{
  qDeleteAll(_items);
  _items.clear();
  _status = _NONE;
  return 0;
}

int HLINE::addItem (Setting *data)
{
  HLineDraw *co = new HLineDraw;
  data->copy(co->settings());
  co->attach(_plot);
  _items.insert(co->settings()->data("ID"), co);
  return 0;
}

int HLINE::info (Setting *info)
{
  HLineDraw *item = _items.value(info->data("ID"));
  if (! item)
    return 1;

  Setting *set = item->settings();
  if (! set)
    return 1;

  info->setData(tr("Type"), QString("HLine"));
  info->setData(tr("Price"), set->data("PRICE"));

  return 0;
}

int HLINE::highLow (Setting *, Setting *data)
{
  double high = 0;
  double low = 0;
  int flag = 0;
  QHashIterator<QString, HLineDraw *> it(_items);
  while (it.hasNext())
  {
    it.next();

    Setting *set = it.value()->settings();
    if (! set)
      continue;

    double d = set->getDouble("PRICE");

    if (! flag)
    {
      high = d;
      low = d;
      flag++;
    }
    else
    {
      if (d > high)
        high = d;

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

void HLINE::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      QwtScaleMap map = _plot->canvasMap(QwtPlot::yRight);

      Setting *set = _selected->settings();
      if (! set)
	break;
      set->setData("PRICE", map.invTransform((double) p.y()));

      _plot->replot();
      break;
    }
    default:
      break;
  }
}

void HLINE::click (int button, QPoint p)
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
          QHashIterator<QString, HLineDraw *> it(_items);
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

int HLINE::create (Setting *parms)
{
  _selected = new HLineDraw;
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
  g_middleMan->statusMessage(tr("Place HLine object..."));

  return 0;
}

void HLINE::dialog ()
{
  if (_dialog)
    return;

  _dialog = new Dialog(g_parent);
  QWidget *w = new HLineDialog(_dialog, _selected->settings());
  connect(_dialog, SIGNAL(accepted()), w, SLOT(save()));
  _dialog->setWidget(w);
  connect(_dialog, SIGNAL(accepted()), this, SLOT(dialogOK()));
  connect(_dialog, SIGNAL(finished(int)), this, SLOT(dialogCancel()));
  _dialog->show();
}

void HLINE::dialogCancel ()
{
  _dialog = 0;
}

void HLINE::dialogOK ()
{
  save();
  _plot->replot();
}

void HLINE::deleteChartObject ()
{
  ConfirmDialog *dialog = new ConfirmDialog(g_parent);
  dialog->setMessage(tr("Confirm chart object delete"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteChartObject2()));
  dialog->show();
}

void HLINE::deleteChartObject2 ()
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

int HLINE::deleteAll ()
{
  DataDataBase db("chartObjects");
  db.transaction();

  QHashIterator<QString, HLineDraw *> it(_items);
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

void HLINE::load ()
{
/*
  if (_settings->getInt("RO"))
    return;

  DataDataBase db("chartObjects");
  if (db.load(_settings->data("ID"), _settings))
    qDebug() << "ChartObject::load: load error";
*/
}

void HLINE::save ()
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

void HLINE::update ()
{
  _plot->replot();
}

int HLINE::calculate (BarData *, Indicator *i, Setting *settings)
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

int HLINE::command (Command *command)
{
  // PARMS:
  // TYPE
  // NAME
  // INDICATOR
  // EXCHANGE
  // SYMBOL
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
  HLineDraw tco;
  Setting *co = tco.settings();
  int saveFlag = 0;
  switch (typeList.indexOf(type))
  {
    case 0: // RO
    {
      QString key = "-" + QString::number(i->chartObjectCount() + 1);
      co->setData("PLUGIN", _plugin);
      co->setData("TYPE", QString("HLine"));
      co->setData("ID", key);
      co->setData("RO", 1);
      break;
    }
    case 1:
      co->setData("PLUGIN", _plugin);
      co->setData("TYPE", QString("HLine"));
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

QWidget * HLINE::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Value") << tr("Color") << tr("Plot");

  QList<int> format;
  format << RuleWidget::_DOUBLE << RuleWidget::_COLOR << RuleWidget::_PLOT;

  RuleWidget *w = new RuleWidget(p, _plugin);
  w->setRules(set, format, header);
  w->loadSettings();
  return w;
}

void HLINE::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HLINE *o = new HLINE;
  return ((Plugin *) o);
}
