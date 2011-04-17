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
#include "DataDataBase.h"
#include "PluginFactory.h"

#include <QSettings>
#include <QtDebug>
#include <QCursor>
#include <QUuid>
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
  _antiAlias = TRUE;
  
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
  connect(_indicator, SIGNAL(signalClear()), this, SLOT(clear()));

  _menu = new PlotMenu(this);
  connect(_menu, SIGNAL(signalDateStatus(bool)), this, SLOT(showDate(bool)));
  connect(_menu, SIGNAL(signalLogStatus(bool)), this, SLOT(setLogScaling(bool)));
  connect(_menu, SIGNAL(signalNewChartObject(QString)), this, SLOT(chartObjectNew(QString)));
  connect(_menu, SIGNAL(signalDeleteAllChartObjects()), this, SLOT(deleteAllChartObjects()));
  connect(_menu, SIGNAL(signalEditIndicator()), _indicator, SLOT(dialog()));

  _dock = new DockWidget(name, mw);
  _dock->setObjectName(name);
  _dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  _dock->setWidget(this);
  _dock->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  mw->addDockWidget(Qt::LeftDockWidgetArea, _dock);
  connect(_menu, SIGNAL(signalLockStatus(bool)), this, SLOT(lockChanged(bool)));
}

Plot::~Plot ()
{
  clear();
}

void Plot::clear ()
{
  if (_qcurves.count())
    qDeleteAll(_qcurves);
  _qcurves.clear();

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
  addCurves();
  addChartObjects(_indicator->chartObjects());

  // calculate end of chart pos
  _endPos = _dateScaleDraw->count();
  int page = width() / _spacing;
  _startPos = _endPos - page;
  if (_startPos < 0)
    _startPos = 0;

  setStartIndex(_startPos);

  emit signalIndex(_startPos);
}

void Plot::addCurves ()
{
  QHash<QString, Curve *> l = _indicator->curves();
  QHashIterator<QString, Curve *> it(l);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    Plugin *plug = _qcurves.value(curve->type());
    if (! plug)
    {
      PluginFactory fac;
      plug = fac.plugin(curve->type());
      if (! plug)
      {
        qDebug() << "Plot::addCurves: no plugin" << curve->type();
        return;
      }

      plug->setParent((void *) this);
      _qcurves.insert(curve->type(), plug);
    }
    plug->setCurve(curve);
  }
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
  if (! _qcurves.count())
  {
    _high = 0;
    _low = 0;
  }
  else
  {
    _high = -999999999.0;
    _low = 999999999.0;
  }

  QHashIterator<QString, Plugin *> it(_qcurves);
  while (it.hasNext())
  {
    it.next();
    Plugin *plug = it.value();

    Setting set, set2;
    set.setData("REQUEST", QString("HIGH_LOW"));
    set.setData("START", _startPos);
    set.setData("END", _endPos);
    if (plug->request(&set, &set2))
      continue;

    double d = set2.getDouble("HIGH");
    if (d > _high)
      _high = d;

    d = set2.getDouble("LOW");
    if (d < _low)
      _low = d;
  }

  QHashIterator<QString, Plugin *> it2(_chartObjects);
  while (it2.hasNext())
  {
    it2.next();

    Setting request, data;
    request.setData("REQUEST", QString("HIGH_LOW"));
    request.setData("START", _startPos);
    request.setData("END", _endPos);
    if (it2.value()->request(&request, &data))
      continue;

    double d = data.getDouble("HIGH");
    if (d > _high)
      _high = d;
    
    d = data.getDouble("LOW");
    if (d < _low)
      _low = d;
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

//qDebug() << "Plot::setStartIndex" << _startPos << width() << _spacing << _endPos;

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
    if (curve->type() == "HistogramBar"
        || curve->type() == "Candle"
        || curve->type() == "Bar")
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
  
  if (_qcurves.count())
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
  emit signalClick(button, p);
}

void Plot::mouseMove (QPoint p)
{
  if (! _dateScaleDraw->count())
    return;
  
  emit signalMove(p);

  Setting set;
  int index = (int) invTransform(QwtPlot::xBottom, p.x());

  QHashIterator<QString, Plugin *> it2(_chartObjects);
  while (it2.hasNext())
  {
    it2.next();

    Setting request, data;
    request.setData("REQUEST", QString("IS_SELECTED"));
    request.setData("X", p.x());
    request.setData("Y", p.y());
    if (it2.value()->request(&request, &data))
      continue;
    
    request.setData("REQUEST", QString("INFO"));
    if (it2.value()->request(&request, &set))
      continue;
      
    emit signalInfoMessage(set);
    return;
  }

  _dateScaleDraw->info(index, set);

  QHashIterator<QString, Plugin *> it(_qcurves);
  while (it.hasNext())
  {
    it.next();

    Setting request;
    request.setData("REQUEST", QString("INFO"));
    request.setData("INDEX", index);
    it.value()->request(&request, &set);
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

  // set anti aliasing
  _antiAlias = settings.value("antialias", TRUE).toBool();
}

//********************************************************************
//***************** CHART OBJECT FUNCTIONS ***************************
//********************************************************************

void Plot::chartObjectNew (QString type)
{
  Plugin *plug = _chartObjects.value(type);
  if (! plug)
  {
    PluginFactory fac;
    plug = fac.plugin(type);
    if (! plug)
    {
      qDebug() << "Plot::chartObjectNew: no plugin";
      return;
    }

    plug->setParent((void *) this);
    connect(plug, SIGNAL(signalSelected()), this, SLOT(chartObjectSelected()));
    connect(plug, SIGNAL(signalUnselected()), this, SLOT(chartObjectUnselected()));
    connect(this, SIGNAL(signalClick(int, QPoint)), plug, SLOT(click(int, QPoint)));
    connect(this, SIGNAL(signalMove(QPoint)), plug, SLOT(move(QPoint)));
    _chartObjects.insert(type, plug);
  }

  Setting parms, data;
  parms.setData("REQUEST", QString("CREATE"));

  QStringList l;
  l << _indicator->name() << g_barData->exchange() << g_barData->symbol();
  parms.setData("KEY", l.join("_"));
  
  if (plug->request(&parms, &data))
  {
    qDebug() << "Plot::chartObjectNew: request error";
    return;
  }
}

void Plot::deleteAllChartObjects ()
{
  Setting parms, data;
  parms.setData("REQUEST", QString("DELETE_ALL"));
  
  QHashIterator<QString, Plugin *> it(_chartObjects);
  while (it.hasNext())
  {
    it.next();
    it.value()->request(&parms, &data);  
  }

  setHighLow();
  setAxisScale(QwtPlot::xBottom, _startPos, _endPos);
  replot();
}

void Plot::chartObjectSelected ()
{
  _selected = 1;
}

void Plot::chartObjectUnselected ()
{
  _selected = 0;
}

void Plot::addChartObjects (QHash<QString, Setting> &l)
{
  // clear old objects
  Setting parms, data;
  parms.setData("REQUEST", QString("CLEAR"));

  QHashIterator<QString, Plugin *> it(_chartObjects);
  while (it.hasNext())
  {
    it.next();
    it.value()->request(&parms, &data);
  }

  // add the objects
  QHashIterator<QString, Setting> it2(l);
  while (it2.hasNext())
  {
    it2.next();
    Setting set = it2.value();
    addChartObject(&set);
  }
}

void Plot::addChartObject (Setting *parms)
{
  Plugin *plug = _chartObjects.value(parms->data("PLUGIN"));
  if (! plug)
  {
    PluginFactory fac;
    plug = fac.plugin(parms->data("PLUGIN"));
    if (! plug)
    {
      qDebug() << "Plot::addChartObject: no plugin" << parms->data("PLUGIN");
      return;
    }

    plug->setParent((void *) this);
    connect(plug, SIGNAL(signalSelected()), this, SLOT(chartObjectSelected()));
    connect(plug, SIGNAL(signalUnselected()), this, SLOT(chartObjectUnselected()));
    connect(this, SIGNAL(signalClick(int, QPoint)), plug, SLOT(click(int, QPoint)));
    connect(this, SIGNAL(signalMove(QPoint)), plug, SLOT(move(QPoint)));
    _chartObjects.insert(parms->data("PLUGIN"), plug);
  }

  Setting request;
  request.setData("REQUEST", QString("ADD"));

  if (plug->request(&request, parms))
    qDebug() << "Plot::addChartObject: request error";
}
