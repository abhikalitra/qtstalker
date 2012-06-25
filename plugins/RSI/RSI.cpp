/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include <QtGui>

#include "ta_libc.h"
#include "RSI.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "Global.h"
#include "RSIDialog.h"



int
RSI::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "dialog" << "runIndicator" << "settings";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("indicator");
      rc = 1;
      break;
    case 1: // dialog
      rc = dialog(pd);
      break;
    case 2: // runIndicator
      rc = run(pd);
      break;
    case 3: // settings
      rc = settings(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
RSI::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "RSI::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "RSI::dialog: invalid settings";
    return 0;
  }
  
  RSIDialog *dialog = new RSIDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
RSI::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *input = pd->settings->get(QString("input"));
  if (! input)
    return 0;
  
  QVariant *period = pd->settings->get(QString("period"));
  if (! period)
    return 0;
  
  QVariant *label = pd->settings->get(QString("label"));
  if (! label)
    return 0;
  
  if (! getRSI(input->toString(), period->toInt(), label->toString()))
    return 0;
  
  // rsi
  QVariant *show = pd->settings->get(QString("rsiShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *style = pd->settings->get(QString("style"));
    if (! style)
      return 0;
  
    QVariant *width = pd->settings->get(QString("width"));
    if (! width)
      return 0;
  
    QVariant *var = pd->settings->get(QString("color"));
    if (! var)
      return 0;
    QColor color(var->toString());

    CurveLineType clt;
    Curve *rsi = new Curve(QString("CurveLine"));
    rsi->setColor(color);
    rsi->setLabel(label->toString());
    rsi->setStyle(clt.stringToIndex(style->toString()));
    rsi->fill(label->toString(), QString(), QString(), QString(), color);
    rsi->setPen(width->toInt());
    pd->curves << rsi;
  }
  
  // ma
  show = pd->settings->get(QString("maShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    Curve *ma = getMA(pd->settings);
    if (ma)
      pd->curves << ma;
  }
  
  // buy marker
  show = pd->settings->get(QString("buyMarkerShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("buyMarkerColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
    
    QVariant *price = pd->settings->get(QString("buyMarkerPrice"));
    if (! price)
      return 0;
    
    Marker *m = newMarker(color, price->toDouble());
    if (! m)
      return 0;
    
    pd->markers << m;
  }

  // sell marker
  show = pd->settings->get(QString("sellMarkerShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("sellMarkerColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
    
    QVariant *price = pd->settings->get(QString("sellMarkerPrice"));
    if (! price)
      return 0;
    
    Marker *m = newMarker(color, price->toDouble());
    if (! m)
      return 0;
    
    pd->markers << m;
  }
  
  return 1;
}

int
RSI::getRSI (QString ikey, int period, QString okey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "RSI::RSI: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double v;
    if (! bar->get(ikey, v))
      continue;
    
    input[dpos++] = (TA_Real) v;
  }

  rc = TA_RSI (0, dpos - 1, &input[0], period, &outBeg, &outNb, &out[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "RSI::getRSI: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(okey, out[outLoop]);
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

Curve *
RSI::getMA (Entity *settings)
{
  QVariant *type = settings->get(QString("maType"));
  if (! type)
    return 0;
  
  QVariant *period = settings->get(QString("maPeriod"));
  if (! period)
    return 0;
  
  QVariant *var = settings->get(QString("maColor"));
  if (! var)
    return 0;
  QColor color(var->toString());
  
  QVariant *label = settings->get(QString("maLabel"));
  if (! label)
    return 0;
  
  QVariant *style = settings->get(QString("maStyle"));
  if (! style)
    return 0;
  
  QVariant *width = settings->get(QString("maWidth"));
  if (! width)
    return 0;
  
  QVariant *rsiLabel = settings->get(QString("label"));
  if (! rsiLabel)
    return 0;

  if (! getMA(rsiLabel->toString(), label->toString(), type->toInt(), period->toInt()))
    return 0;
  
  CurveLineType clt;
  Curve *curve = new Curve(QString("CurveLine"));
  curve->setLabel(label->toString());
  curve->setStyle(clt.stringToIndex(style->toString()));
  curve->setPen(width->toInt());
  curve->setColor(color);
  curve->fill(label->toString(), QString(), QString(), QString(), color);

  return curve;
}

int
RSI::getMA (QString inKey, QString outKey, int type, int period)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "RSI::getMA: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double v;
    if (! bar->get(inKey, v))
      continue;
    
    input[dpos++] = (TA_Real) v;
  }

  rc = TA_MA(0, dpos - 1, &input[0], period, (TA_MAType) type, &outBeg, &outNb, &out[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "RSI::getMA: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(outKey, out[outLoop]);
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

Marker *
RSI::newMarker (QColor color, double price)
{
  QString id = QUuid::createUuid().toString();
  id.remove("{");
  id.remove("}");
  id.remove("-");
  
  Marker *m = new Marker(QString("MarkerHLine"));
  m->setID(id);  
  m->setReadOnly(TRUE);
  
  Entity *e = m->settings();
  
  QVariant *tset = e->get(QString("color"));
  tset->setValue(color.name());
  
  tset = e->get(QString("price"));
  tset->setValue(price);

  return m;
}

int
RSI::settings (PluginData *pd)
{
  BarType bt;
  CurveLineType clt;
  MAType mat;
  Entity *command = new Entity;
  
  command->set(QString("plugin"), new QVariant(QString("RSI")));
  command->set(QString("type"), new QVariant(QString("indicator")));
  
  command->set(QString("input"), new QVariant(bt.indexToString(BarType::_CLOSE)));
  command->set(QString("color"), new QVariant(QString("red")));
  command->set(QString("period"), new QVariant(14));
  command->set(QString("label"), new QVariant(QString("RSI")));
  command->set(QString("width"), new QVariant(1));
  command->set(QString("style"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("rsiShow"), new QVariant(TRUE));
  
  command->set(QString("maType"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("maStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("maWidth"), new QVariant(1));
  command->set(QString("maColor"), new QVariant(QString("yellow")));
  command->set(QString("maPeriod"), new QVariant(10));
  command->set(QString("maLabel"), new QVariant(QString("MA")));
  command->set(QString("maShow"), new QVariant(TRUE));
  
  command->set(QString("buyMarkerColor"), new QVariant(QString("green")));
  command->set(QString("buyMarkerPrice"), new QVariant(30.0));
  command->set(QString("buyMarkerShow"), new QVariant(TRUE));

  command->set(QString("sellMarkerColor"), new QVariant(QString("red")));
  command->set(QString("sellMarkerPrice"), new QVariant(70.0));
  command->set(QString("sellMarkerShow"), new QVariant(TRUE));
  
  pd->settings = command;
  
  return 1;
}

int
RSI::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(rsi, RSI);
