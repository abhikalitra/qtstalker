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
#include "ChartObjectBuy.h"
#include "ChartObjectHLine.h"
#include "ChartObjectRetracement.h"
#include "ChartObjectSell.h"
#include "ChartObjectText.h"
#include "ChartObjectTLine.h"
#include "ChartObjectVLine.h"
#include "IndicatorDataBase.h"

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
#include <QMessageBox>
#include <qwt_scale_div.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

Plot::Plot ()
{
  _spacing = 8;
  _high = 0;
  _low = 0;
  _startPos = -1;
  _endPos = -1;
  
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

  _linearScaleEngine = new QwtLinearScaleEngine;
  _logScaleEngine = new QwtLog10ScaleEngine;
  setAxisScaleEngine(QwtPlot::yRight, _linearScaleEngine);

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
  QAction *a = _coListMenu->addAction(QPixmap(buyarrow_xpm), tr("New &Buy"));
  a->setShortcut(Qt::ALT+Qt::Key_B);
  a->setData(QVariant(ChartObject::_Buy));
  a = _coListMenu->addAction(QPixmap(horizontal_xpm), tr("New &HLine"));
  a->setShortcut(Qt::ALT+Qt::Key_H);
  a->setData(QVariant(ChartObject::_HLine));
  a = _coListMenu->addAction(QPixmap(fib_xpm), tr("New &Retracement"));
  a->setShortcut(Qt::ALT+Qt::Key_R);
  a->setData(QVariant(ChartObject::_Retracement));
  a = _coListMenu->addAction(QPixmap(sellarrow_xpm), tr("New &Sell"));
  a->setShortcut(Qt::ALT+Qt::Key_S);
  a->setData(QVariant(ChartObject::_Sell));
  a = _coListMenu->addAction(QPixmap(text_xpm), tr("New Te&xt"));
  a->setShortcut(Qt::ALT+Qt::Key_X);
  a->setData(QVariant(ChartObject::_Text));
  a = _coListMenu->addAction(QPixmap(trend_xpm), tr("New &TLine"));
  a->setShortcut(Qt::ALT+Qt::Key_T);
  a->setData(QVariant(ChartObject::_TLine));
  a = _coListMenu->addAction(QPixmap(vertical_xpm), tr("New &VLine"));
  a->setShortcut(Qt::ALT+Qt::Key_V);
  a->setData(QVariant(ChartObject::_VLine));
  connect(_coListMenu, SIGNAL(triggered(QAction *)), this, SLOT(chartObjectMenuSelected(QAction *)));
  
  _chartMenu = new QMenu(this);
  _chartMenu->addAction(QPixmap(indicator_xpm), tr("&New Indicator"), this, SIGNAL(signalNewIndicator()), Qt::ALT+Qt::Key_N);
  _chartMenu->addAction(QPixmap(edit), tr("Edit &Indicator"), this, SLOT(editIndicator()), Qt::ALT+Qt::Key_I);
  _chartMenu->addAction(QPixmap(delete_xpm), tr("De&lete Indicator"), this, SLOT(deleteIndicator()), Qt::ALT+Qt::Key_L);
  _chartMenu->addSeparator ();
  _chartMenu->addMenu(_coListMenu);
  _chartMenu->addAction(QPixmap(delete_xpm), tr("Delete &All Chart Objects"), this, SLOT(deleteAllChartObjects()), Qt::ALT+Qt::Key_A);
  _chartMenu->addSeparator ();
  _dateAction = _chartMenu->addAction(QPixmap(date), tr("&Date"), this, SLOT(toggleDate()), Qt::ALT+Qt::Key_D);
  _dateAction->setCheckable(TRUE);
  _logAction = _chartMenu->addAction(QPixmap(loggridicon), tr("Log &Scaling"), this, SLOT(toggleLog()), Qt::ALT+Qt::Key_S);
  _logAction->setCheckable(TRUE);
  _chartMenu->addSeparator ();
  _chartMenu->addAction(tr("&Background Color"), this, SLOT(editBackgroundColor()), Qt::ALT+Qt::Key_B);
  _chartMenu->addAction(tr("&Font"), this, SLOT(editFont()), Qt::ALT+Qt::Key_F);
}

Plot::~Plot ()
{
  clear();

  delete _dateScaleDraw;
  delete _grid;
  delete _linearScaleEngine;
  delete _logScaleEngine;
  delete _picker;
}

void Plot::clear ()
{
  if (_qwtCurves.count())
    qDeleteAll(_qwtCurves);
  _qwtCurves.clear();

  if (_curves.count())
    qDeleteAll(_curves);
  _curves.clear();

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

void Plot::addCurves (QMap<int, Curve *> &curves)
{
  QList<int> keys;
  keys = curves.keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
    addCurve(keys.at(loop), curves.value(keys.at(loop)));
}

void Plot::addCurve (int id, Curve *curve)
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

void Plot::addCurve3 (int id, Curve *curve, QwtPlotCurve *qcurve)
{
  qcurve->setTitle(curve->label());
  qcurve->setPen(QPen(curve->color()));
  qcurve->setZ(curve->z());
  qcurve->setYAxis(QwtPlot::yRight);
  qcurve->attach(this);
  _qwtCurves.insert(id, qcurve);
  _curves.insert(id, curve);
}

void Plot::setBackgroundColor (QColor d)
{
  setCanvasBackground(d);
}

void Plot::setGridColor (QColor d)
{
  _grid->setMajPen(QPen(d, 0, Qt::DotLine));
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
    setAxisScaleEngine(QwtPlot::yRight, _logScaleEngine);
  else
    setAxisScaleEngine(QwtPlot::yRight, _linearScaleEngine);
}

void Plot::showDate (bool d)
{
  enableAxis(QwtPlot::xBottom, d);
}

void Plot::setCrosshair (bool d)
{
  if (d)
  {
//    _picker->setEnabled(TRUE);
  }
  else
  {
//    _picker->setEnabled(FALSE);
  }
}

void Plot::setHighLow ()
{
  _high = -99999999;
  _low = 99999999;

  QList<int> keys;
  keys = _curves.keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double h, l;
    Curve *curve = _curves.value(keys.at(loop));
    curve->highLowRange(_startPos, _endPos, h, l);
    if (h > _high)
      _high = h;
    if (l < _low)
      _low = l;
  }

  keys = _chartObjects.keys();

  for (loop = 0; loop < keys.count(); loop++)
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
  connect(dialog, SIGNAL(colorSelected(const QColor &)), this, SIGNAL(signalBackgroundColorChanged(QColor)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void Plot::editFont ()
{
  QFontDialog *dialog = new QFontDialog(axisFont(QwtPlot::xBottom), this);
  connect(dialog, SIGNAL(fontSelected(const QFont &)), this, SIGNAL(signalFontChanged(QFont)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void Plot::mouseClick (int button, QPoint p)
{
  switch (button)
  {
    case Qt::LeftButton:
    {
      // check if we clicked on a chart object
      QList<int> keys;
      keys = _chartObjects.keys();

      int loop = 0;
      for (; loop < keys.count(); loop++)
      {
        ChartObject *co = _chartObjects.value(keys.at(loop));
        if (co->isSelected(p))
        {
          ChartObjectDialog *dialog = co->dialog();
          connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(updateChartObject(ChartObjectSettings)));
          connect(dialog, SIGNAL(signalDelete(ChartObjectSettings)), this, SLOT(deleteChartObject(ChartObjectSettings)));
          connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
          dialog->show();
          return;
        }
      }

      break;
    }
    case Qt::RightButton:
      showContextMenu();
      break;
    default:
      break;
  }
}

void Plot::mouseMove (QPoint p)
{
  if (! _dateScaleDraw->count())
    return;
  
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

  keys = _curves.keys();
  
  Strip strip;
  for (loop = 0; loop < keys.count(); loop++)
  {
    Curve *curve = _curves.value(keys.at(loop));
    curve->info(index, set);
  }

  emit signalInfoMessage(set);
}

//********************************************************************
//***************** CHART OBJECT FUNCTIONS ***************************
//********************************************************************

void Plot::deleteAllChartObjects ()
{
  int rc = QMessageBox::warning(this,
                                tr("Qtstalker: Warning"),
                                tr("Are you sure you want to delete all chart objects from this indicator?"),
                                QMessageBox::Yes,
                                QMessageBox::No,
                                QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  ChartObjectDataBase db;
  g_mutex.lock();
  db.deleteChartObjectsIndicator(_indicator);
  g_mutex.unlock();

  QList<int> keys;
  keys = _chartObjects.keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    ChartObject *co = _chartObjects.value(keys.at(loop));
    co->detach();
  }

  qDeleteAll(_chartObjects);
  _chartObjects.clear();

  updatePlot();
}

void Plot::chartObjectMenuSelected (QAction *a)
{
  ChartObjectDialog *dialog = 0;
  
  switch ((ChartObject::Type) a->data().toInt())
  {
    case ChartObject::_Buy:
    {
      ChartObjectBuy co;
      dialog = co.dialog();
      break;
    }
    case ChartObject::_HLine:
    {
      ChartObjectHLine co;
      dialog = co.dialog();
      break;
    }
    case ChartObject::_Retracement:
    {
      ChartObjectRetracement co;
      dialog = co.dialog();
      break;
    }
    case ChartObject::_Sell:
    {
      ChartObjectSell co;
      dialog = co.dialog();
      break;
    }
    case ChartObject::_Text:
    {
      ChartObjectText co;
      dialog = co.dialog();
      break;
    }
    case ChartObject::_TLine:
    {
      ChartObjectTLine co;
      dialog = co.dialog();
      break;
    }
    case ChartObject::_VLine:
    {
      ChartObjectVLine co;
      dialog = co.dialog();
      break;
    }
    default:
      return;
      break;
  }
  
  dialog->enableDeleteButton(0);
  connect(dialog, SIGNAL(signalDone(ChartObjectSettings)), this, SLOT(newChartObject(ChartObjectSettings)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void Plot::newChartObject (ChartObjectSettings set)
{
  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(set.type);
  if (! co)
    return;

  Config config;
  QString d = QString::number(config.getInt(Config::LastChartObjectID) + 1);
  config.setData(Config::LastChartObjectID, d);
  set.id = d.toInt();

  set.exchange = _exchange;
  set.symbol = _symbol;
  set.indicator = _indicator;

  co->setSettings(set);
  co->setZ(10);
  co->attach(this);

  _chartObjects.insert(set.id, co);

  updatePlot();
}

void Plot::updateChartObject (ChartObjectSettings set)
{
  ChartObject *co = _chartObjects.value(set.id);
  if (! co)
    return;

  co->setSettings(set);

  updatePlot();
}

void Plot::deleteChartObject (ChartObjectSettings set)
{
  ChartObject *co = _chartObjects.value(set.id);
  if (! co)
    return;

  co->detach();
  delete co;
  _chartObjects.remove(set.id);

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
    co->setZ(10);
    co->attach(this);
  }
}





/*

// code to have crosshairs always on
class PickerMachine: public QwtPickerMachine
{
public:
    virtual QwtPickerMachine::CommandList transition(
        const QwtEventPattern &, const QEvent *e)
    {
        QwtPickerMachine::CommandList cmdList;
        if ( e->type() == QEvent::MouseMove )
            cmdList += Move;

        return cmdList;
    }
};

class Picker: public QwtPlotPicker
{
public:
    Picker(QwtPlotCanvas *canvas):
        QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, canvas)
    {
        setRubberBand(QwtPlotPicker::CrossRubberBand);
        setRubberBandPen(QColor(Qt::green));
        setRubberBand(QwtPicker::CrossRubberBand);

        canvas->setMouseTracking(true);
    }

    void widgetMouseMoveEvent(QMouseEvent *e)
    {
        if ( !isActive() )
        {
            setSelectionFlags(QwtPicker::PointSelection);

            begin();
            append(e->pos());
        }

        QwtPlotPicker::widgetMouseMoveEvent(e);
    }

    void widgetLeaveEvent(QEvent *)
    {
        end();
    }

    virtual QwtPickerMachine *stateMachine(int) const
    {
        return new PickerMachine;
    }
};

*/
