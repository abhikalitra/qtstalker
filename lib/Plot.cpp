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
#include "ChartObjectDialog.h"
#include "Config.h"
#include "ChartObjectFactory.h"
#include "ChartObjectDataBase.h"
#include "Globals.h"
#include "IndicatorDataBase.h"
#include "Dialog.h"

#include "../pics/loggrid.xpm"
#include "../pics/date.xpm"
#include "../pics/delete.xpm"
#include "../pics/edit.xpm"
#include "../pics/indicator.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"
#include "../pics/fib.xpm"
#include "../pics/horizontal.xpm"
#include "../pics/text.xpm"
#include "../pics/trend.xpm"
#include "../pics/vertical.xpm"

#include <QtDebug>
#include <QCursor>
#include <QColorDialog>
#include <QFontDialog>
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

  _coListMenu = new QMenu(this);
  _coListMenu->setTitle(tr("New Chart Object..."));
  QAction *a = _coListMenu->addAction(QPixmap(buyarrow_xpm), tr("&Buy"));
  a->setShortcut(Qt::ALT+Qt::Key_B);
  a->setData(QVariant(ChartObject::_Buy));
  a = _coListMenu->addAction(QPixmap(horizontal_xpm), tr("&HLine"));
  a->setShortcut(Qt::ALT+Qt::Key_H);
  a->setData(QVariant(ChartObject::_HLine));
  a = _coListMenu->addAction(QPixmap(fib_xpm), tr("&Retracement"));
  a->setShortcut(Qt::ALT+Qt::Key_R);
  a->setData(QVariant(ChartObject::_Retracement));
  a = _coListMenu->addAction(QPixmap(sellarrow_xpm), tr("&Sell"));
  a->setShortcut(Qt::ALT+Qt::Key_S);
  a->setData(QVariant(ChartObject::_Sell));
  a = _coListMenu->addAction(QPixmap(text_xpm), tr("Te&xt"));
  a->setShortcut(Qt::ALT+Qt::Key_X);
  a->setData(QVariant(ChartObject::_Text));
  a = _coListMenu->addAction(QPixmap(trend_xpm), tr("&TLine"));
  a->setShortcut(Qt::ALT+Qt::Key_T);
  a->setData(QVariant(ChartObject::_TLine));
  a = _coListMenu->addAction(QPixmap(vertical_xpm), tr("&VLine"));
  a->setShortcut(Qt::ALT+Qt::Key_V);
  a->setData(QVariant(ChartObject::_VLine));
  connect(_coListMenu, SIGNAL(triggered(QAction *)), this, SLOT(chartObjectMenuSelected(QAction *)));
  
  _chartMenu = new QMenu(this);
  _chartMenu->addAction(QPixmap(indicator_xpm), tr("&New Indicator..."), this, SIGNAL(signalNewIndicator()), Qt::ALT+Qt::Key_N);
  _chartMenu->addAction(QPixmap(edit_xpm), tr("Edit &Indicator..."), this, SLOT(editIndicator()), Qt::ALT+Qt::Key_I);
  _chartMenu->addAction(QPixmap(delete_xpm), tr("De&lete Indicator..."), this, SLOT(deleteIndicator()), Qt::ALT+Qt::Key_L);
  _chartMenu->addSeparator ();
  _chartMenu->addMenu(_coListMenu);
  _chartMenu->addAction(QPixmap(delete_xpm), tr("Delete &All Chart Objects"), this, SLOT(deleteAllChartObjects()), Qt::ALT+Qt::Key_A);
  _chartMenu->addSeparator ();
  _dateAction = _chartMenu->addAction(QPixmap(date), tr("&Date"), this, SLOT(toggleDate()), Qt::ALT+Qt::Key_D);
  _dateAction->setCheckable(TRUE);
  _logAction = _chartMenu->addAction(QPixmap(loggridicon), tr("Log &Scaling"), this, SLOT(toggleLog()), Qt::ALT+Qt::Key_S);
  _logAction->setCheckable(TRUE);
  _chartMenu->addSeparator ();
  _chartMenu->addAction(tr("&Background Color..."), this, SLOT(editBackgroundColor()), Qt::ALT+Qt::Key_B);
  _chartMenu->addAction(tr("&Font..."), this, SLOT(editFont()), Qt::ALT+Qt::Key_F);
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

void Plot::setDates (BarData &bd)
{
  _exchange = bd.getExchange();
  _symbol = bd.getSymbol();
  _dateScaleDraw->setDates(bd);
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
}

void Plot::setGridColor (QColor d)
{
  _grid->setMajPen(QPen(d, 0, Qt::DotLine));
}

void Plot::setCrossHairsColor (QColor d)
{
  _picker->setColor(d);
}

void Plot::setGrid (bool d)
{
  _grid->enableX(d);
  _grid->enableY(d);
}

void Plot::setFont (QFont d)
{
  setAxisFont(QwtPlot::yRight, d);
  setAxisFont(QwtPlot::xBottom, d);
}

void Plot::setLogScaling (bool d)
{
  if (d)
    setAxisScaleEngine(QwtPlot::yRight, new QwtLog10ScaleEngine);
  else
    setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);

  _logAction->setChecked(d);

  replot();
}

void Plot::showDate (bool d)
{
  enableAxis(QwtPlot::xBottom, d);
  _dateAction->setChecked(d);
}

void Plot::setCrossHairs (bool d)
{
  _picker->setCrossHairs(d);
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
  
  replot();
}

void Plot::showContextMenu ()
{
  _chartMenu->exec(QCursor::pos());
}

void Plot::editIndicator ()
{
  emit signalEditIndicator(_indicator);
}

void Plot::deleteIndicator ()
{
  emit signalDeleteIndicator(_indicator);
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
  db.getIndicator(i);
  
  int status = axisEnabled(QwtPlot::xBottom);
  if (status)
    status = 0;
  else
    status = 1;

  i.setDate(status);
  db.setIndicator(i);

  showDate(status);
}

void Plot::toggleLog ()
{
  IndicatorDataBase db;
  Indicator i;
  i.setName(_indicator);
  db.getIndicator(i);

  int status = i.getLog();
  if (status)
    status = 0;
  else
    status = 1;

  i.setLog(status);
  db.setIndicator(i);

  setLogScaling(status);
}

void Plot::editBackgroundColor ()
{
  QColorDialog *dialog = new QColorDialog(canvasBackground(), this);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Chart Background Color"));
  connect(dialog, SIGNAL(colorSelected(const QColor &)), this, SIGNAL(signalBackgroundColorChanged(QColor)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void Plot::editFont ()
{
  QFontDialog *dialog = new QFontDialog(axisFont(QwtPlot::xBottom), this);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Chart Font"));
  connect(dialog, SIGNAL(fontSelected(const QFont &)), this, SIGNAL(signalFontChanged(QFont)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
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

//********************************************************************
//***************** CHART OBJECT FUNCTIONS ***************************
//********************************************************************

void Plot::deleteAllChartObjects ()
{
  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Delete All Chart Objects"));
  dialog->setMessage(tr("Are you sure you want to delete all chart objects from this indicator?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteAllChartObjects2()));
  dialog->show();
}

void Plot::deleteAllChartObjects2 ()
{
  ChartObjectDataBase db;
  g_mutex.lock();
  db.deleteChartObjectsIndicator(_indicator);
  g_mutex.unlock();

  qDeleteAll(_chartObjects);
  _chartObjects.clear();

  updatePlot();
}

void Plot::chartObjectMenuSelected (QAction *a)
{
  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(a->data().toInt());
  if (! co)
    return;

  ChartObjectSettings set;
  co->settings(set);
  
  Config config;
  QString d = QString::number(config.getInt(Config::LastChartObjectID) + 1);
  config.setData(Config::LastChartObjectID, d);
  set.id = d.toInt();

  set.exchange = _exchange;
  set.symbol = _symbol;
  set.indicator = _indicator;

  co->setSettings(set);

  setupChartObject(co);

  _chartObjects.insert(set.id, co);

  co->create();
}

void Plot::deleteChartObject (int id)
{
  ChartObject *co = _chartObjects.value(id);
  if (! co)
    return;

  delete co;
  
  _chartObjects.remove(id);

  updatePlot();
}

void Plot::loadChartObjects ()
{
  ChartObjectDataBase db;
  db.getChartObjects(_exchange, _symbol, _indicator, _chartObjects);

  QList<int> keys;
  keys = _chartObjects.keys();
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    ChartObject *co = _chartObjects.value(keys.at(loop));
    setupChartObject(co);
  }
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
  QList<int> keys;
  keys = _chartObjects.keys();

  ChartObjectDataBase db;
  g_mutex.lock();
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
  g_mutex.unlock();
}

