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
#include "PlotHistogramBar.h"
#include "PlotOHLC.h"
#include "PlotCandle.h"
#include "Strip.h"
#include "ChartObjectFactory.h"
#include "Globals.h"
#include "Script.h"
#include "ChartObjectDataBase.h"

#include <QSettings>
#include <QtDebug>
#include <QCursor>
#include <qwt_scale_div.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>


Plot::Plot (QString name, QMainWindow *mw) : QwtPlot (mw)
{
  _spacing = 8;
  _high = 0;
  _low = 0;
  _startPos = -1;
  _endPos = -1;
  _selected = 0;
  
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

  // setup the context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu()));

  _indicator = new Indicator(this);
  _indicator->setName(name);
  connect(_indicator, SIGNAL(signalPlot()), this, SLOT(updatePlot()));

  _menu = new PlotMenu(this);
  connect(_menu, SIGNAL(signalDateStatus(bool)), this, SLOT(showDate(bool)));
  connect(_menu, SIGNAL(signalLogStatus(bool)), this, SLOT(setLogScaling(bool)));
  connect(_menu, SIGNAL(signalNewChartObject(QString)), this, SLOT(chartObjectNew(QString)));
  connect(_menu, SIGNAL(signalDeleteAllChartObjects()), this, SLOT(deleteAllChartObjects()));

  _dock = new DockWidget(name.left(4), mw);
  _dock->setObjectName(name);
  _dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  _dock->setWidget(this);
  mw->addDockWidget(Qt::LeftDockWidgetArea, _dock);
  connect(_menu, SIGNAL(signalLockStatus(bool)), this, SLOT(lockChanged(bool)));
}

Plot::~Plot ()
{
  clear();
}

void Plot::clear ()
{
  if (_qwtCurves.count())
    qDeleteAll(_qwtCurves);
  _qwtCurves.clear();

  _indicator->clear();

  if (_chartObjects.count())
    qDeleteAll(_chartObjects);
  _chartObjects.clear();

  _selected = 0;

  QwtPlot::clear();
}

void Plot::setDates ()
{
  _dateScaleDraw->setDates();
}

void Plot::setIndicator ()
{
  _indicator->load();
  
  _menu->setIndicator(_indicator->name());

  _menu->setLock(_indicator->lock());
  _dock->statusChanged(_indicator->lock());
  
  _menu->setDate(_indicator->date());
  enableAxis(QwtPlot::xBottom, _indicator->date());
  
  _menu->setLog(_indicator->log());
  if (_indicator->log() == TRUE)
    setAxisScaleEngine(QwtPlot::yRight, new QwtLog10ScaleEngine);
  else
    setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);
}

Indicator * Plot::indicator ()
{
  return _indicator;
}

void Plot::updatePlot ()
{
  setDates();
  addCurves(_indicator->curves());
  addChartObjects(_indicator->chartObjects());

  // calculate end of chart pos
  _endPos = _dateScaleDraw->count();
  int page = width() / _spacing;
  _startPos = _endPos - page;
  if (_startPos < 0)
    _startPos = 0;

  setStartIndex(_startPos);

  if (isVisible())
    emit signalIndex(_startPos);
}

void Plot::addCurves (QHash<QString, Curve *> &curves)
{
  QHashIterator<QString, Curve *> it(curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    addCurve(it.key(), curve);
  }
}

void Plot::addCurve (QString id, Curve *curve)
{
  switch ((Curve::Type) curve->type())
  {
    case Curve::Candle:
    {
      PlotCandle *qcurve = new PlotCandle;
      qcurve->setData(curve);
      addCurve3(id, curve, qcurve);
      break;
    }
    case Curve::Dot:
    {
      QwtPlotCurve *qcurve = new QwtPlotCurve;
      qcurve->setStyle(QwtPlotCurve::Dots);
      addCurve2(curve, qcurve);
      addCurve3(id, curve, qcurve);
      break;
    }
    case Curve::Histogram:
    {
      QwtPlotCurve *qcurve = new QwtPlotCurve;
      qcurve->setRenderHint(QwtPlotItem::RenderAntialiased, TRUE);
      addCurve2(curve, qcurve);

      QColor c = curve->color();
      c.setAlpha(150);
      qcurve->setPen(c);
      qcurve->setBrush(c);

      addCurve3(id, curve, qcurve);
      break;
    }
    case Curve::HistogramBar:
    {
      PlotHistogramBar *qcurve = new PlotHistogramBar;
      qcurve->setData(curve);
      addCurve3(id, curve, qcurve);
      break;
    }
    case Curve::Line:
    {
      QwtPlotCurve *qcurve = new QwtPlotCurve;
      qcurve->setRenderHint(QwtPlotItem::RenderAntialiased, TRUE);
      qcurve->setStyle(QwtPlotCurve::Lines);
      addCurve2(curve, qcurve);
      addCurve3(id, curve, qcurve);
      break;
    }
    case Curve::OHLC:
    {
      PlotOHLC *qcurve = new PlotOHLC;
      qcurve->setData(curve);
      addCurve3(id, curve, qcurve);
      break;
    }
    default:
      qDebug() << "Plot::addCurve: invalid curve type" << (int) curve->type();
      return;
      break;
  }
}

void Plot::addCurve2 (Curve *curve, QwtPlotCurve *qcurve)
{
  QList<int> keys;
  curve->keys(keys);
  
  QwtArray<double> x;
  QwtArray<double> y;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    x.append(keys.at(loop));
    CurveBar *bar = curve->bar(keys.at(loop));
    y.append(bar->data());
  }
  
  qcurve->setData(x, y);
}

void Plot::addCurve3 (QString id, Curve *curve, QwtPlotCurve *qcurve)
{
  qcurve->setTitle(curve->label());
  qcurve->setPen(QPen(curve->color()));
  qcurve->setZ(curve->z());
  qcurve->setYAxis(QwtPlot::yRight);
  qcurve->attach(this);
  _qwtCurves.insert(id, qcurve);
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

  _indicator->setLog(d);
  _indicator->save();
  
  replot();
}

void Plot::showDate (bool d)
{
  enableAxis(QwtPlot::xBottom, d);
  _indicator->setDate(d);
  _indicator->save();
}

void Plot::lockChanged (bool d)
{
  _dock->statusChanged(d);
  _indicator->setLock(d);
  _indicator->save();
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
  if (! _qwtCurves.count())
  {
    _high = 0;
    _low = 0;
  }
  else
  {
    _high = -999999999.0;
    _low = 999999999.0;
  }

  QHashIterator<QString, Curve *> it(_indicator->curves());
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    
    double h, l;
    curve->highLowRange(_startPos, _endPos, h, l);
    if (h > _high)
      _high = h;
    if (l < _low)
      _low = l;
  }

  QHashIterator<QString, ChartObject *> it2(_chartObjects);
  while (it2.hasNext())
  {
    it2.next();
    double h, l;
    ChartObject *co = it2.value();
    if (! co->highLow(_startPos, _endPos, h, l))
      continue;

    if (h > _high)
      _high = h;
    if (l < _low)
      _low = l;
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

  setHighLow();

  setAxisScale(QwtPlot::xBottom, _startPos, _endPos);

  setYPoints();

  replot();
}

void Plot::setYPoints ()
{
  _plotScaleDraw->clearPoints();

  QHashIterator<QString, Curve *> it(_indicator->curves());
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
    if (curve->type() == Curve::HistogramBar
        || curve->type() == Curve::Candle
        || curve->type() == Curve::OHLC)
    {
      if (! flag)
        index--;
    }
    
    CurveBar *bar = curve->bar(index);
    if (! bar)
      continue;

    QColor color = bar->color();

    switch ((Curve::Type) curve->type())
    {
      case Curve::Dot:
      case Curve::Histogram:
      case Curve::Line:
        color = curve->color();
        break;
      default:
        break;
    }

    _plotScaleDraw->addPoint(color, bar->data());
  }
}

void Plot::showContextMenu ()
{
  if (_selected)
    return;
  
  if (_qwtCurves.count())
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
//  if (! _selected && button == Qt::RightButton) 
//  {
//    showContextMenu();
//    return;
//  }
  
  emit signalClick(button, p);
}

void Plot::mouseMove (QPoint p)
{
  if (! _dateScaleDraw->count())
    return;
  
  emit signalMove(p);

  Setting set;
  int index = (int) invTransform(QwtPlot::xBottom, p.x());

  QHashIterator<QString, ChartObject *> it2(_chartObjects);
  while (it2.hasNext())
  {
    it2.next();
    ChartObject *co = it2.value();
    if (co->isSelected(p))
    {
      co->info(set);
      emit signalInfoMessage(set);
      return;
    }
  }

  _dateScaleDraw->info(index, set);

  QHashIterator<QString, Curve *> it(_indicator->curves());
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    curve->info(index, set);
  }

  emit signalInfoMessage(set);
}

PlotMenu * Plot::plotMenu ()
{
  return _menu;
}

DockWidget * Plot::dockWidget ()
{
  return _dock;
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
}

//********************************************************************
//***************** CHART OBJECT FUNCTIONS ***************************
//********************************************************************

void Plot::chartObjectNew (QString type)
{
  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(type);
  if (! co)
    return;

  Setting *set = co->settings();
  set->setData("Exchange", g_barData->exchange());
  set->setData("Symbol", g_barData->symbol());
  set->setData("Indicator", _indicator->name());
  QString id = QUuid::createUuid().toString();
  id = id.remove(QString("{"), Qt::CaseSensitive);
  id = id.remove(QString("}"), Qt::CaseSensitive);
  id = id.remove(QString("-"), Qt::CaseSensitive);
  set->setData("ID", id);
  
  setupChartObject(co);

  _chartObjects.insert(id, co);

  co->create();
}

void Plot::deleteChartObject (QString id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  delete co;
  _chartObjects.remove(id);

  ChartObjectDataBase db;
  db.deleteChartObject(id);

  _selected = 0;
  setHighLow();
  setAxisScale(QwtPlot::xBottom, _startPos, _endPos);
  replot();
}

void Plot::deleteAllChartObjects ()
{
  ChartObjectDataBase db;
  db.transaction();
  
  QHashIterator<QString, ChartObject *> it(_chartObjects);
  while (it.hasNext())
  {
    it.next();
    
    ChartObject *co = it.value();
    delete co;
    _chartObjects.remove(it.key());

    db.deleteChartObject(it.key());
  }

  db.commit();

  _selected = 0;
  setHighLow();
  setAxisScale(QwtPlot::xBottom, _startPos, _endPos);
  replot();
}

void Plot::updateChartObject (QString id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  co->load();

  setHighLow();
  setAxisScale(QwtPlot::xBottom, _startPos, _endPos);
  replot();
}

void Plot::chartObjectSelected (QString id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  _selected = 1;
}

void Plot::chartObjectUnselected (QString id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  _selected = 0;
}

void Plot::chartObjectMoveStart (QString id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  connect(this, SIGNAL(signalMove(QPoint)), co, SLOT(move(QPoint)));
}

void Plot::chartObjectMoveEnd (QString id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  disconnect(this, SIGNAL(signalMove(QPoint)), co, SLOT(move(QPoint)));
}

void Plot::setupChartObject (ChartObject *co)
{
  co->setZ(10);
  co->attach(this);
  co->setParent(this);
  
  connect(co, SIGNAL(signalDelete(QString)), this, SLOT(deleteChartObject(QString)));
  connect(co, SIGNAL(signalUpdate(QString)), this, SLOT(updateChartObject(QString)));
  connect(co, SIGNAL(signalSelected(QString)), this, SLOT(chartObjectSelected(QString)));
  connect(co, SIGNAL(signalUnselected(QString)), this, SLOT(chartObjectUnselected(QString)));
  connect(co, SIGNAL(signalMoveStart(QString)), this, SLOT(chartObjectMoveStart(QString)));
  connect(co, SIGNAL(signalMoveEnd(QString)), this, SLOT(chartObjectMoveEnd(QString)));
  connect(this, SIGNAL(signalClick(int, QPoint)), co, SLOT(click(int, QPoint)));
}

void Plot::addChartObjects (QHash<QString, Setting> &l)
{
  ChartObjectFactory fac;
  QHashIterator<QString, Setting> it(l);
  while (it.hasNext())
  {
    it.next();
    Setting set = it.value();
    
    ChartObject *co = fac.chartObject(set.data("Type"));
    if (! co)
      return;

    co->setSettings(&set);
    setupChartObject(co);
    _chartObjects.insert(set.data("ID"), co);
  }
}
