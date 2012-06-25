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


#include "ta_libc.h"
#include "CCI.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "Global.h"
#include "CCIDialog.h"
#include "MAType.h"

#include <QtGui>


int
CCI::command (PluginData *pd)
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
CCI::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "CCI::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "CCI::dialog: invalid settings";
    return 0;
  }
  
  CCIDialog *dialog = new CCIDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
CCI::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *period = pd->settings->get(QString("cciPeriod"));
  if (! period)
    return 0;
  
  QVariant *label = pd->settings->get(QString("cciLabel"));
  if (! label)
    return 0;
  
  if (! getCCI(period->toInt(), label->toString()))
    return 0;
  
  QVariant *show = pd->settings->get(QString("cciShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *style = pd->settings->get(QString("cciStyle"));
    if (! style)
      return 0;
  
    QVariant *width = pd->settings->get(QString("cciWidth"));
    if (! width)
      return 0;
  
    QVariant *var = pd->settings->get(QString("cciColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setLabel(label->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->setPen(width->toInt());
    c->setColor(color);
    c->fill(label->toString(), QString(), QString(), QString(), color);
    pd->curves << c;
  }

  // MA
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

Curve *
CCI::getMA (Entity *settings)
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
  
  QVariant *cciLabel = settings->get(QString("cciLabel"));
  if (! cciLabel)
    return 0;

  if (! getMA(cciLabel->toString(), label->toString(), type->toInt(), period->toInt()))
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
CCI::getMA (QString inKey, QString outKey, int type, int period)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "CCI::getMA: error on TA_Initialize";

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
    qDebug() << "CCI::getMA: TA-Lib error" << rc;
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

int
CCI::getCCI (int period, QString okey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "CCI::CCI: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  BarType bt;
  QString highKey = bt.indexToString(BarType::_HIGH);
  QString lowKey = bt.indexToString(BarType::_LOW);
  QString closeKey = bt.indexToString(BarType::_CLOSE);
  
  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double h;
    if (! bar->get(highKey, h))
      continue;
    
    double l;
    if (! bar->get(lowKey, l))
      continue;
    
    double c;
    if (! bar->get(closeKey, c))
      continue;
    
    high[dpos] = (TA_Real) h;
    low[dpos] = (TA_Real) l;
    close[dpos] = (TA_Real) c;
    dpos++;
  }

  rc = TA_CCI(0, dpos - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
  
  if (rc != TA_SUCCESS)
  {
      qDebug() << "CCI::getCCI: TA-Lib error" << rc;
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

Marker *
CCI::newMarker (QColor color, double price)
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
CCI::settings (PluginData *pd)
{
  Entity *command = new Entity;
  CurveLineType clt;
  MAType mat;
  
  command->set(QString("plugin"), new QVariant(QString("CCI")));
  command->set(QString("type"), new QVariant(QString("indicator")));
  
  command->set(QString("cciColor"), new QVariant(QString("red")));
  command->set(QString("cciPeriod"), new QVariant(10));
  command->set(QString("cciLabel"), new QVariant(QString("CCI")));
  command->set(QString("cciWidth"), new QVariant(1));
  command->set(QString("cciStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  
  command->set(QString("maType"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("maStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("maWidth"), new QVariant(1));
  command->set(QString("maColor"), new QVariant(QString("yellow")));
  command->set(QString("maPeriod"), new QVariant(10));
  command->set(QString("maLabel"), new QVariant(QString("MA")));
  
  command->set(QString("buyMarkerColor"), new QVariant(QString("green")));
  command->set(QString("buyMarkerPrice"), new QVariant(100.0));
  command->set(QString("buyMarkerShow"), new QVariant(TRUE));

  command->set(QString("sellMarkerColor"), new QVariant(QString("red")));
  command->set(QString("sellMarkerPrice"), new QVariant(-100.0));
  command->set(QString("sellMarkerShow"), new QVariant(TRUE));
  
  pd->settings = command;
  
  return 1;
}

int
CCI::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(cci, CCI);
