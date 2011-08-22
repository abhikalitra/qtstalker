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

#include "Plot.h"
#include "Strip.h"
#include "Globals.h"
#include "ChartObjectDataBase.h"
#include "ConfirmDialog.h"
#include "ChartObjectFactory.h"

#include "../pics/delete.xpm"
#include "../pics/edit.xpm"

#include <QSettings>
#include <QtDebug>
#include <QCursor>
#include <QUuid>
#include <qwt_scale_div.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>
#include <QMainWindow>

Plot::Plot (QString name, QWidget *mw) : QwtPlot (mw)
{
  _name = name;
  _spacing = 8;
  _high = 0;
  _low = 0;
  _startPos = -1;
  _endPos = -1;
  _selected = 0;
  _antiAlias = TRUE;
  _chartObjectDialog = 0;
  _row = 0;
  _col = 0;

  setMinimumHeight(20);

  setCanvasBackground(QColor(Qt::black));
  setMargin(0);
  enableAxis(QwtPlot::yRight, TRUE);
  enableAxis(QwtPlot::yLeft, FALSE);

  // add custom date scale drawing class to plot
  _dateScaleDraw = new DateScaleDraw;
  setAxisScaleDraw(QwtPlot::xBottom, _dateScaleDraw);

  // add custom date scale drawing class to plot
  _plotScaleDraw = new PlotScaleDraw;
  setAxisScaleDraw(QwtPlot::yRight, _plotScaleDraw);

  _grid = new QwtPlotGrid;
//  _grid->enableXMin(FALSE);
  _grid->enableX(FALSE);
  _grid->enableYMin(FALSE);
  _grid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
  _grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  _grid->setYAxis(QwtPlot::yRight);
  _grid->attach(this);

  // try to set the scale width to a sane size to keep plots aligned
  QwtScaleWidget *sw = axisWidget(QwtPlot::yRight);
  sw->scaleDraw()->setMinimumExtent(60);

  // setup the mouse events handler
  _picker = new PlotPicker(this);
  connect(_picker, SIGNAL(signalMouseMove(QPoint)), this, SLOT(mouseMove(QPoint)));
  connect(_picker, SIGNAL(signalMouseClick(int, QPoint)), this, SLOT(mouseClick(int, QPoint)));
  connect(_picker, SIGNAL(signalMouseDoubleClick(int, QPoint)), this, SLOT(mouseDoubleClick(int, QPoint)));
//  connect(_picker, SIGNAL(signalMouseRelease(int, QPoint)), this, SLOT(mouseRelease(int, QPoint)));

  // setup the context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu()));

  _menu = new PlotMenu(this, _name);
  connect(_menu, SIGNAL(signalNewChartObject(QString)), this, SLOT(chartObjectNew(QString)));
  connect(_menu, SIGNAL(signalDeleteAllChartObjects()), this, SLOT(deleteAllChartObjects()));

  // chart object menu
  _chartObjectMenu = new QMenu(this);
  _chartObjectMenu->addAction(QIcon(edit_xpm), QObject::tr("&Edit"), this, SLOT(chartObjectDialog()), QKeySequence(Qt::ALT+Qt::Key_E));
  _chartObjectMenu->addAction(QIcon(delete_xpm), QObject::tr("&Delete"), this, SLOT(deleteChartObject()), QKeySequence(Qt::ALT+Qt::Key_D));

//  grabGesture(Qt::TapAndHoldGesture);
//  grabGesture(Qt::SwipeGesture);
}

Plot::~Plot ()
{
  clear();
}

void Plot::clear ()
{
  if (_curves.count())
    qDeleteAll(_curves);
  _curves.clear();

  if (_chartObjects.count())
    qDeleteAll(_chartObjects);
  _chartObjects.clear();

  _selected = 0;

  if (_chartObjectDialog)
    delete _chartObjectDialog;
  _chartObjectDialog= 0;

  _dateScaleDraw->clear();

  QwtPlot::clear();
}

void Plot::setDates ()
{
  _dateScaleDraw->setDates();
}

void Plot::updatePlot ()
{
  setDates();

  // calculate end of chart pos
  _endPos = _dateScaleDraw->count();
  int page = width() / _spacing;
  _startPos = _endPos - page;
  if (_startPos < 0)
    _startPos = 0;

  setStartIndex(_startPos);

  emit signalIndex(_startPos);
}

void Plot::setCurve (Curve *curve)
{
  curve->itemChanged();
  curve->setYAxis(QwtPlot::yRight);
  curve->attach(this);
  _curves.insert(curve->label(), curve);

  _dateScaleDraw->setDates(curve);
}

void Plot::dates (QList<QDateTime> &d)
{
  d = _dateScaleDraw->dates();
}

void Plot::setBackgroundColor (QColor d)
{
  setCanvasBackground(d);
  replot();
}

void Plot::setGridColor (QColor d)
{
  _grid->setMajPen(QPen(d, 0, Qt::DotLine));
  replot();
}

void Plot::setCrossHairsColor (QColor d)
{
  _picker->setColor(d);
  replot();
}

void Plot::setGrid (bool d)
{
//  _grid->enableX(d);
  _grid->enableY(d);
  replot();
}

void Plot::setFont (QFont d)
{
  setAxisFont(QwtPlot::yRight, d);
  setAxisFont(QwtPlot::xBottom, d);
  replot();
}

void Plot::setLogScaling (bool d)
{
  if (d == TRUE)
    setAxisScaleEngine(QwtPlot::yRight, new QwtLog10ScaleEngine);
  else
    setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);

  replot();
}

void Plot::showDate (bool d)
{
  enableAxis(QwtPlot::xBottom, d);
}

void Plot::setCrossHairs (bool d)
{
  _picker->setCrossHairs(d);
  replot();
}

void Plot::setBarSpacing (int d)
{
  _spacing = d;
  setStartIndex(_startPos);
}

void Plot::setHighLow ()
{
  if (! _curves.count())
  {
    _high = 0;
    _low = 0;
  }
  else
  {
    _high = -999999999.0;
    _low = 999999999.0;
  }

  QHashIterator<QString, Curve *> it(_curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();

    double h, l;
    if (curve->highLowRange(_startPos, _endPos, h, l))
      continue;

    if (h > _high)
      _high = h;

    if (l < _low)
      _low = l;
  }

  QHashIterator<QString, ChartObject *> it2(_chartObjects);
  while (it2.hasNext())
  {
    it2.next();
    double d, d2;
    if (it2.value()->highLow(_startPos, _endPos, d, d2))
      continue;

    if (d > _high)
      _high = d;

    if (d2 < _low)
      _low = d2;
  }

  setAxisScale(QwtPlot::yRight, _low, _high);
}

void Plot::setStartIndex (int index)
{
  _startPos = index;
  int page = width() / _spacing;
  _endPos = _startPos + page;
  if (_endPos > (_dateScaleDraw->count() + _dateScaleDraw->count()))
    _endPos = _dateScaleDraw->count() + _dateScaleDraw->count();

//qDebug() << "Plot::setStartIndex" << _startPos << _endPos;

  setHighLow();

  setAxisScale(QwtPlot::xBottom, _startPos, _endPos);

  setYPoints();

  replot();
}

void Plot::setYPoints ()
{
  _plotScaleDraw->clearPoints();

  QHashIterator<QString, Curve *> it(_curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();

    int min = 0;
    int max = 0;
    curve->keyRange(min, max);
    int index = _endPos;
    int flag = 0;
    if (index > max)
    {
      index = max;
      flag++;
    }

    // fix for Y AND X axis misalignment
    if (curve->type() == "Histogram Bar"
        || curve->type() == "Candle"
        || curve->type() == "OHLC")
    {
      if (! flag)
        index--;
    }

    CurveBar *bar = curve->bar(index);
    if (! bar)
      continue;

    _plotScaleDraw->addPoint(bar->color(), bar->data());
  }
}

void Plot::showContextMenu ()
{
  if (_selected)
    return;

  if (_curves.count())
    _menu->setCOMenuStatus(TRUE);
  else
    _menu->setCOMenuStatus(FALSE);

  _menu->exec(QCursor::pos());
}

int Plot::index ()
{
  return _startPos;
}

void Plot::mouseClick (int button, QPoint p)
{
  if (! _dateScaleDraw->count())
    return;

  if (_chartObjectDialog)
    return;

  if (_selected)
  {
    _selected->click(button, p);
    return;
  }

  QHashIterator<QString, ChartObject *> it(_chartObjects);
  while (it.hasNext())
  {
    it.next();

    if (it.value()->readOnly() == TRUE)
      continue;

    if (! it.value()->isSelected(p))
      continue;

    _selected = it.value();
    _selected->click(button, p);
    return;
  }
}

void Plot::mouseDoubleClick (int, QPoint)
{
/*
  if (! _dateScaleDraw->count())
    return;

  if (_chartObjectDialog)
    return;

  if (_selected)
    return;

  int pos = (int) invTransform(QwtPlot::xBottom, p.x());
  int page = width() / _spacing;
  int center = _startPos + (page / 2);
//  if (pos > center)
//    nextPage();
//  else
//  {
//    if (pos < center)
//      previousPage();
//  }
//qDebug() << "Plot::mouseDoubleClick:" << pos;
*/
}

void Plot::mouseMove (QPoint p)
{
  if (! _dateScaleDraw->count())
    return;

  if (_selected)
    _selected->move(p);

  int index = (int) invTransform(QwtPlot::xBottom, p.x());

  Message set;
  QHashIterator<QString, ChartObject *> it2(_chartObjects);
  while (it2.hasNext())
  {
    it2.next();
    if (! it2.value()->isSelected(p))
      continue;

    if (it2.value()->info(set))
      continue;

    emit signalInfoMessage(set);

    return;
  }

  _dateScaleDraw->info(index, set);

  QHashIterator<QString, Curve *> it(_curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    curve->info(index, &set);
  }

  emit signalInfoMessage(set);
}

PlotMenu * Plot::plotMenu ()
{
  return _menu;
}

void Plot::loadSettings ()
{
  QSettings settings(g_localSettings);
  QColor color(settings.value("plot_background_color", "black").toString());
  setBackgroundColor(color);

  QStringList l = settings.value("plot_font").toString().split(",", QString::SkipEmptyParts);
  if (l.count())
  {
    QFont font;
    font.fromString(l.join(","));
    setFont(font);
  }

  // set crosshairs status
  setCrossHairs(settings.value("crosshairs", 0).toInt());

  // set grid status
  setGrid(settings.value("grid", TRUE).toBool());

  // set crosshairs color
  color.setNamedColor(settings.value("crosshairs_color", "white").toString());
  setCrossHairsColor(color);

  // set anti aliasing
  _antiAlias = settings.value("antialias", TRUE).toBool();
}

void Plot::unselect ()
{
  _selected = 0;
}

void Plot::select (QString id)
{
  _selected = _chartObjects.value(id);
}

void Plot::setScriptFile (QString d)
{
  _scriptFile = d;
}

QString Plot::scriptFile ()
{
  return _scriptFile;
}

void Plot::setRow (int d)
{
  _row = d;
}

int Plot::row ()
{
  return _row;
}

void Plot::setCol (int d)
{
  _col = d;
}

int Plot::col ()
{
  return _col;
}

void Plot::setName (QString d)
{
  _name = d;
}

QString Plot::name ()
{
  return _name;
}

QHash<QString, Curve *> Plot::curves ()
{
  return _curves;
}

//********************************************************************
//***************** CHART OBJECT FUNCTIONS ***************************
//********************************************************************

void Plot::chartObjectNew (QString type)
{
  ChartObjectFactory fac;
  _selected = fac.chartObject(type);
  if (! _selected)
  {
    qDebug() << "Plot::chartObjectNew: invalid type" << type;
    return;
  }

  ChartObjectDataBase db;
  int t = db.lastId();
  t++;
  QString id = QString::number(t);
  _selected->setID(id);

  _chartObjects.insert(id, _selected);

  _selected->setScript(_scriptFile);
  _selected->setSymbol(g_currentSymbol.key());
  _selected->setPlotName(_name);

  _selected->attach(this);

  _selected->setModified(1);
  _selected->save();

  _selected->create();
}

void Plot::deleteAllChartObjects ()
{
  QStringList l;
  QHashIterator<QString, ChartObject *> it(_chartObjects);
  while (it.hasNext())
  {
    it.next();
    ChartObject *co = it.value();

    if (co->readOnly())
      continue;

    QString id = co->ID();
    l << id;
    co->setModified(0);

    delete co;
    _chartObjects.remove(id);
  }

  _selected = 0;
  _chartObjectDialog = 0;

  ChartObjectDataBase db;
  db.transaction();
  db.remove(l);
  db.commit();

  setStartIndex(_startPos);
}

void Plot::setChartObjects (QHash<QString, ChartObject *> l)
{
  qDeleteAll(_chartObjects);
  _chartObjects.clear();

  // add the objects
  QHashIterator<QString, ChartObject *> it(l);
  while (it.hasNext())
  {
    it.next();
    addChartObject(it.value());
  }
}

void Plot::addChartObject (ChartObject *co)
{
  _chartObjects.insert(co->ID(), co);
  co->attach(this);
}

void Plot::showChartObjectMenu ()
{
  if (! _selected)
    return;

  if (_chartObjectDialog)
    return;

  _chartObjectMenu->exec(QCursor::pos());
}

void Plot::chartObjectDialog ()
{
  if (! _selected)
    return;

  if (_chartObjectDialog)
    return;

  _chartObjectDialog = new CommandDialog(this);
  _chartObjectDialog->setWidgets(_selected->settings());
  connect(_chartObjectDialog, SIGNAL(accepted()), this, SLOT(chartObjectDialog2()));
  connect(_chartObjectDialog, SIGNAL(rejected()), this, SLOT(chartObjectDialog3()));
  _chartObjectDialog->show();
}

void Plot::chartObjectDialog2 ()
{
  if (! _selected)
    return;

  _selected->setModified(1);

  _chartObjectDialog = 0;

  replot();
}

void Plot::chartObjectDialog3 ()
{
  _chartObjectDialog = 0;
}

void Plot::deleteChartObject ()
{
  if (! _selected)
    return;

  ConfirmDialog *dialog = new ConfirmDialog(g_parent);
  dialog->setMessage(QObject::tr("Confirm chart object delete"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteChartObject2()));
  dialog->show();
}

void Plot::deleteChartObject2 ()
{
  if (! _selected)
    return;

  QStringList l;
  l << _selected->ID();

  ChartObjectDataBase db;
  db.transaction();
  db.remove(l);
  db.commit();

  _selected->setModified(0);
  _chartObjects.remove(_selected->ID());
  delete _selected;
  _selected = 0;

  setHighLow();

  setAxisScale(QwtPlot::xBottom, _startPos, _endPos);

  replot();
}
