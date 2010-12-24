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
//#include "ChartObjectDialog.h"
#include "ChartObjectFactory.h"
#include "Globals.h"
#include "Script.h"
#include "IndicatorDataBase.h"

#include <QSettings>
#include <QtDebug>
#include <QCursor>
#include <qwt_scale_div.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>


Plot::Plot ()
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
  _grid->enableXMin(FALSE);
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

  _menu = new PlotMenu(this);
  connect(_menu, SIGNAL(signalToggleDate()), this, SLOT(toggleDate()));
  connect(_menu, SIGNAL(signalToggleLog()), this, SLOT(toggleLog()));
  connect(_menu, SIGNAL(signalNewChartObject(int)), this, SLOT(chartObjectNew(int)));
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

  if (_curves.count())
    qDeleteAll(_curves);
  _curves.clear();

  saveChartObjects();

  if (_chartObjects.count())
    qDeleteAll(_chartObjects);
  _chartObjects.clear();

  QwtPlot::clear();
}

void Plot::setDates ()
{
  _dateScaleDraw->setDates();
}

void Plot::setIndicator (QString &d)
{
  _indicator = d;
}

void Plot::updatePlot ()
{
  setHighLow();
  replot();
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
    case Curve::Horizontal:
    {
      CurveBar *bar = curve->bar(0);
      if (! bar)
        break;

      _curves.insert(id, curve);

      QwtPlotMarker *line = new QwtPlotMarker;
      line->setYValue(bar->data());
      line->setLineStyle(QwtPlotMarker::HLine);
      line->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
      line->setLinePen(QPen(bar->color(), 0, Qt::SolidLine));
      line->setYAxis(QwtPlot::yRight);
      line->setZ(curve->z());
      line->attach(this);
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
  _curves.insert(id, curve);
}

void Plot::curves (QHash<QString, Curve *> &d)
{
  d = _curves;
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
  _grid->enableX(d);
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
  if (d)
    setAxisScaleEngine(QwtPlot::yRight, new QwtLog10ScaleEngine);
  else
    setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);

  _menu->setLogStatus(d);

  replot();
}

void Plot::showDate (bool d)
{
  enableAxis(QwtPlot::xBottom, d);
  _menu->setDateStatus(d);
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
    curve->highLowRange(_startPos, _endPos, h, l);
    if (h > _high)
      _high = h;
    if (l < _low)
      _low = l;
  }

  QList<int> keys;
  keys = _chartObjects.keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double h, l;
    ChartObject *co = _chartObjects.value(keys.at(loop));
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

//  setYPoints();
  
  replot();
}

void Plot::showContextMenu ()
{
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

void Plot::toggleDate ()
{
  IndicatorDataBase db;
  Indicator i;
  i.setName(_indicator);
  db.load(&i);
  
  int status = axisEnabled(QwtPlot::xBottom);
  if (status)
    status = 0;
  else
    status = 1;

  i.setDate(status);

  db.save(&i);

  showDate(status);
}

void Plot::toggleLog ()
{
  IndicatorDataBase db;
  Indicator i;
  i.setName(_indicator);
  db.load(&i);

  int status = i.getLog();
  if (status)
    status = 0;
  else
    status = 1;

  i.setLog(status);
  
  db.save(&i);

  setLogScaling(status);
}

void Plot::mouseClick (int button, QPoint p)
{
  if (! _selected && button == Qt::RightButton) 
  {
    showContextMenu();
    return;
  }
  
  emit signalClick(button, p);
}

void Plot::mouseMove (QPoint p)
{
  if (! _dateScaleDraw->count())
    return;
  
  emit signalMove(p);

  Setting set;
  int index = (int) invTransform(QwtPlot::xBottom, p.x());

  QList<int> keys;
  keys = _chartObjects.keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    ChartObject *co = _chartObjects.value(keys.at(loop));
    if (co->isSelected(p))
    {
      co->info(set);
      emit signalInfoMessage(set);
      return;
    }
  }

  _dateScaleDraw->info(index, set);

  Strip strip;
  QHashIterator<QString, Curve *> it(_curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    curve->info(index, set);
  }

  emit signalInfoMessage(set);
}

/*
void Plot::setYPoints ()
{
  _plotScaleDraw->clearPoints();

  int page = width() / _spacing;
  int index = _startPos + page;
  if (index > _dateScaleDraw->count())
    index = _dateScaleDraw->count() - 1;

  QHashIterator<QString, Curve *> it(_curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();

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
*/

//********************************************************************
//***************** CHART OBJECT FUNCTIONS ***************************
//********************************************************************

/*
void Plot::deleteAllChartObjects ()
{
  Dialog *dialog = new Dialog;
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Delete All Chart Objects"));
  dialog->setMessage(tr("Delete all chart objects from this indicator?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteAllChartObjects2()));
  dialog->show();
}

void Plot::deleteAllChartObjects2 ()
{
// FIXME  
  ChartObjectDataBase db;
  db.transaction();
  db.deleteChartObjectsIndicator(_indicator);
  db.commit();

  qDeleteAll(_chartObjects);
  _chartObjects.clear();

  _selected = 0;

  updatePlot();
}
*/

void Plot::chartObjectNew (int type)
{
  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(type);
  if (! co)
    return;

  Setting *set = co->settings();

  QSettings settings(g_settingsFile);
  int id = settings.value("chart_object_last_id", -1).toInt();
  id++;
  settings.setValue("chart_object_last_id", id);
  settings.sync();
  
  set->setData("Exchange", g_barData->exchange());
  set->setData("Symbol", g_barData->symbol());
  set->setData("Indicator", _indicator);
  set->setData("ID", id);

  setupChartObject(co);

  _chartObjects.insert(id, co);

  co->create();
}

void Plot::deleteChartObject (int)
{
// FIXME
/*
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  delete co;
  
  _chartObjects.remove(id);

  _selected = 0;

  updatePlot();
*/
}

void Plot::loadChartObjects ()
{
// FIXME
/*
  ChartObjectDataBase db;
  db.getChartObjects(g_barData.getExchange(), g_barData.getSymbol(), _indicator, _chartObjects);

  QList<int> keys;
  keys = _chartObjects.keys();
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    ChartObject *co = _chartObjects.value(keys.at(loop));
    setupChartObject(co);
  }
*/
}

void Plot::chartObjectSelected (int id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  _selected = 1;
}

void Plot::chartObjectUnselected (int id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  _selected = 0;
}

void Plot::chartObjectMoveStart (int id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  connect(this, SIGNAL(signalMove(QPoint)), co, SLOT(move(QPoint)));
}

void Plot::chartObjectMoveEnd (int id)
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
  
  connect(co, SIGNAL(signalSelected(int)), this, SLOT(chartObjectSelected(int)));
  connect(co, SIGNAL(signalUnselected(int)), this, SLOT(chartObjectUnselected(int)));
  connect(co, SIGNAL(signalDelete(int)), this, SLOT(deleteChartObject(int)));
  connect(co, SIGNAL(signalMoveStart(int)), this, SLOT(chartObjectMoveStart(int)));
  connect(co, SIGNAL(signalMoveEnd(int)), this, SLOT(chartObjectMoveEnd(int)));
  connect(this, SIGNAL(signalClick(int, QPoint)), co, SLOT(click(int, QPoint)));
}

void Plot::saveChartObjects ()
{
// FIXME
/*
  QList<int> keys;
  keys = _chartObjects.keys();

  ChartObjectDataBase db;
  db.transaction();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    ChartObject *co = _chartObjects.value(keys.at(loop));
    if (co->isModified())
    {
      ChartObjectSettings set;
      co->settings(set);
      db.setChartObject(set);
    }
  }

  db.commit();
*/
}
