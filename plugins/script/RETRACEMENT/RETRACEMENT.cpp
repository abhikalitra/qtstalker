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

#include "RETRACEMENT.h"
#include "RetracementDialog.h"
#include "Globals.h"
#include "DataDataBase.h"
#include "ConfirmDialog.h"
#include "DateScaleDraw.h"

#include "../pics/delete.xpm"
#include "../pics/edit.xpm"

#include <QtDebug>
#include <QSettings>
#include <qwt_plot.h>

RETRACEMENT::RETRACEMENT ()
{
  _plugin = "RETRACEMENT";
  _type = "CHART_OBJECT";
  _createFlag = 0;
  _status = _NONE;
  _dialog = 0;
  _selected = 0;

  _menu = new QMenu;
  _editAction = _menu->addAction(QPixmap(edit_xpm), tr("&Edit"), this, SLOT(dialog()), Qt::ALT+Qt::Key_E);
  _deleteAction = _menu->addAction(QPixmap(delete_xpm), tr("&Delete"), this, SLOT(deleteChartObject()), Qt::ALT+Qt::Key_D);
}

RETRACEMENT::~RETRACEMENT ()
{
  delete _menu;
  if (_dialog)
    delete _dialog;
  qDeleteAll(_items);
}

int RETRACEMENT::request (Setting *request, Setting *data)
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

void RETRACEMENT::setParent (void *p)
{
  _plot = (QwtPlot *) p;
}

int RETRACEMENT::clear ()
{
  qDeleteAll(_items);
  _items.clear();
  _status = _NONE;
  return 0;
}

int RETRACEMENT::addItem (Setting *data)
{
  RetracementDraw *co = new RetracementDraw;
  data->copy(co->settings());
  co->attach(_plot);
  _items.insert(co->settings()->data("ID"), co);
  return 0;
}

int RETRACEMENT::info (Setting *info)
{
  RetracementDraw *item = _items.value(info->data("ID"));
  if (! item)
    return 1;

  Setting *set = item->settings();
  if (! set)
    return 1;

  info->setData(tr("Type"), QString("Retracement"));

  QDateTime dt = set->dateTime("DATE");
  info->setData(tr("SD"), dt.toString("yyyy-MM-dd"));
  info->setData(tr("ST"), dt.toString("HH:mm:ss"));

  dt = set->dateTime("DATE2");
  info->setData(tr("ED"), dt.toString("yyyy-MM-dd"));
  info->setData(tr("ET"), dt.toString("HH:mm:ss"));

  info->setData(tr("High"), set->data("HIGH"));
  info->setData(tr("Low"), set->data("LOW"));
  info->setData(tr("Level 1"), set->data("LINE1"));
  info->setData(tr("Level 2"), set->data("LINE2"));
  info->setData(tr("Level 3"), set->data("LINE3"));
  info->setData(tr("Level 4"), set->data("LINE4"));
  info->setData(tr("Level 5"), set->data("LINE5"));
  info->setData(tr("Level 6"), set->data("LINE6"));

  return 0;
}

int RETRACEMENT::highLow (Setting *request, Setting *data)
{
  double high = 0;
  double low = 0;
  int flag = 0;
  int start = request->getInt("START");
  int end = request->getInt("END");
  QHashIterator<QString, RetracementDraw *> it(_items);
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
    int x2 = dsd->x(set->dateTime("DATE2"));

    if ((x >= start && x <= end) || ((x2 >= start && x2 <= end)))
    {
      if (! flag)
      {
        high = set->getDouble("HIGH");
        low = set->getDouble("LOW");
        flag++;
      }
      else
      {
        double d = set->getDouble("HIGH");
        if (d > high)
          high = d;
	
        d = set->getDouble("LOW");
        if (d < low)
          low = d;
      }
    }
  }

  if (! flag)
    return 1;

  data->setData("HIGH", high);
  data->setData("LOW", low);

  return 0;
}

void RETRACEMENT::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      QwtScaleMap map = _plot->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());
      if (x < 0)
        break;

      DateScaleDraw *dsd = (DateScaleDraw *) _plot->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      if (! dt.isValid())
        break;

      Setting *set = _selected->settings();
      if (! set)
	break;
      set->setData("DATE", dt);

      map = _plot->canvasMap(QwtPlot::yRight);
      set->setData("HIGH", map.invTransform((double) p.y()));

      if (_createFlag)
      {
        set->setData("DATE2", dt);
        set->setData("LOW", set->data("HIGH"));
      }

      _plot->replot();
      break;
    }
    case _MOVE2:
    {
      QwtScaleMap map = _plot->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());
      if (x < 0)
        break;

      DateScaleDraw *dsd = (DateScaleDraw *) _plot->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      if (! dt.isValid())
        break;

      Setting *set = _selected->settings();
      if (! set)
	break;
      set->setData("DATE2", dt);

      map = _plot->canvasMap(QwtPlot::yRight);
      set->setData("LOW", map.invTransform((double) p.y()));

      _plot->replot();
      break;
    }
    default:
      break;
  }
}

void RETRACEMENT::click (int button, QPoint p)
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
        {
          int grab = _selected->isGrabSelected(p);
          if (grab)
          {
            _status = _MOVE;
            if (grab == 2)
              _status = _MOVE2;
            return;
          }

          if (! _selected->isSelected(p))
          {
            _status = _NONE;
            _selected->setSelected(FALSE);
            emit signalUnselected();
            _plot->replot();
            return;
          }
          break;
        }
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
          if (_createFlag)
          {
            _status = _MOVE2;
            g_middleMan->statusMessage(tr("Select lowest ending point..."));
            return;
          }

          _status = _SELECTED;
          save();
          return;
        default:
          break;
      }

      break;
    }
    case _MOVE2:
    {
      switch (button)
      {
        case Qt::LeftButton:
          _status = _SELECTED;
          _createFlag = 0;
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
          QHashIterator<QString, RetracementDraw *> it(_items);
          while (it.hasNext())
          {
            it.next();
            if (it.value()->isSelected(p))
            {
              _status = _SELECTED;
	      _selected = it.value();
              _selected->setSelected(TRUE);
              emit signalSelected();
              _plot->replot();
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

int RETRACEMENT::create (Setting *parms)
{
  _selected = new RetracementDraw;
  Setting *set = _selected->settings();
  set->setData("KEY", parms->data("KEY"));

  QString id = QUuid::createUuid().toString();
  id = id.remove(QString("{"), Qt::CaseSensitive);
  id = id.remove(QString("}"), Qt::CaseSensitive);
  id = id.remove(QString("-"), Qt::CaseSensitive);
  set->setData("ID", id);

  _selected->attach(_plot);
  _items.insert(id, _selected);
  _createFlag = 1;
  _status = _MOVE;
  _selected->setSelected(TRUE);
  g_middleMan->statusMessage(tr("Select highest starting point..."));

  return 0;
}

void RETRACEMENT::dialog ()
{
  if (_dialog)
    return;

  _dialog = new Dialog(g_parent);
  QWidget *w = dialog(_dialog, _selected->settings());
  connect(_dialog, SIGNAL(accepted()), w, SLOT(save()));
  _dialog->setWidget(w);
  connect(_dialog, SIGNAL(accepted()), this, SLOT(dialogOK()));
  connect(_dialog, SIGNAL(finished(int)), this, SLOT(dialogCancel()));
  _dialog->show();
}

void RETRACEMENT::dialogCancel ()
{
  _dialog = 0;
}

void RETRACEMENT::dialogOK ()
{
  save();
  _plot->replot();
}

void RETRACEMENT::deleteChartObject ()
{
  ConfirmDialog *dialog = new ConfirmDialog(g_parent);
  dialog->setMessage(tr("Confirm chart object delete"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteChartObject2()));
  dialog->show();
}

void RETRACEMENT::deleteChartObject2 ()
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

int RETRACEMENT::deleteAll ()
{
  DataDataBase db("chartObjects");
  db.transaction();

  QHashIterator<QString, RetracementDraw *> it(_items);
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

void RETRACEMENT::load ()
{
/*
  if (_settings->getInt("RO"))
    return;

  DataDataBase db("chartObjects");
  if (db.load(_settings->data("ID"), _settings))
    qDebug() << "ChartObject::load: load error";
*/
}

void RETRACEMENT::save ()
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

void RETRACEMENT::update ()
{
  _plot->replot();
}

int RETRACEMENT::calculate (BarData *, Indicator *i, Setting *settings)
{
  Setting co;
  QString key = "-" + QString::number(i->chartObjectCount() + 1);
  co.setData("PLUGIN", _plugin);
  co.setData("TYPE", settings->data("TYPE"));
  co.setData("ID", key);
  co.setData("RO", 1);
  co.setData("COLOR", settings->data("COLOR"));
  co.setData("DATE", settings->data("DATE"));
  co.setData("DATE2", settings->data("DATE2"));
  co.setData("HIGH", settings->data("HIGH"));
  co.setData("LOW", settings->data("LOW"));
  co.setData("EXTEND", settings->data("EXTEND"));
  co.setData("LINE1", settings->data("LINE1"));
  co.setData("LINE2", settings->data("LINE2"));
  co.setData("LINE3", settings->data("LINE3"));
  co.setData("LINE4", settings->data("LINE4"));
  co.setData("LINE5", settings->data("LINE5"));
  co.setData("LINE6", settings->data("LINE6"));
  co.setData("Z", settings->data("Z"));
  i->addChartObject(co);

  return 0;
}

int RETRACEMENT::command (Command *command)
{
  // PARMS:
  // TYPE
  // NAME
  // INDICATOR
  // EXCHANGE
  // SYMBOL
  // DATE
  // DATE2
  // COLOR
  // HIGH
  // LOW
  // LINE1>
  // LINE2>
  // LINE3>
  // LINE4>
  // LINE5>
  // LINE6>

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
  RetracementDraw tco;
  Setting *co = tco.settings();
  int saveFlag = 0;
  switch (typeList.indexOf(type))
  {
    case 0: // RO
    {
      QString key = "-" + QString::number(i->chartObjectCount() + 1);
      co->setData("PLUGIN", _plugin);
      co->setData("TYPE", QString("Retracement"));
      co->setData("ID", key);
      co->setData("RO", 1);
      break;
    }
    case 1:
      co->setData("PLUGIN", _plugin);
      co->setData("TYPE", QString("Retracement"));
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

  // verify date2
  dt = QDateTime::fromString(command->parm("DATE2"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE2" << command->parm("DATE2");
    return 1;
  }
  co->setData("DATE2", command->parm("DATE2"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  co->setData("COLOR", command->parm("COLOR"));

  // verify high
  bool ok;
  command->parm("HIGH").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid HIGH" << command->parm("HIGH");
    return 1;
  }
  co->setData("HIGH", command->parm("HIGH"));

  // verify low
  command->parm("LOW").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LOW" << command->parm("LOW");
    return 1;
  }
  co->setData("LOW", command->parm("LOW"));

  // verify all the possible retracement values
  int line = 1;
  for (; line < 7; line++)
  {
    QString k = "LINE" + QString::number(line);
    command->parm(k).toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid LINE" << k;
      continue;
    }

    QString key = "LINE" + QString::number(line);
    co->setData(key, command->parm(k));
  }

  if (saveFlag)
    save();

  Setting set;
  co->copy(&set);
  i->addChartObject(set);

  command->setReturnCode("0");

  return 0;
}

void RETRACEMENT::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  RETRACEMENT *o = new RETRACEMENT;
  return ((Plugin *) o);
}
