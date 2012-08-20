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
#include "ADX.h"
#include "CurveLineType.h"
#include "Global.h"
#include "ADXDialog.h"
#include "BarType.h"

int
ADX::command (PluginData *pd)
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
ADX::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "ADX::dialog: invalid parent";
    return 0;
  }

  if (! pd->settings)
  {
    qDebug() << "ADX::dialog: invalid settings";
    return 0;
  }

  ADXDialog *dialog = new ADXDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;

  return 1;
}

int
ADX::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;

  // period
  QVariant *period = pd->settings->get(QString("period"));
  if (! period)
    return 0;

  // MDI
  QVariant *show = pd->settings->get(QString("mdiShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("mdiColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *label = pd->settings->get(QString("mdiLabel"));
    if (! label)
      return 0;

    QVariant *style = pd->settings->get(QString("mdiStyle"));
    if (! label)
      return 0;

    QVariant *width = pd->settings->get(QString("mdiWidth"));
    if (! width)
      return 0;

    if (! getADX(0,
                 period->toInt(),
                 label->toString()))
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(label->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(label->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  // PDI
  show = pd->settings->get(QString("pdiShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("pdiColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *label = pd->settings->get(QString("pdiLabel"));
    if (! label)
      return 0;

    QVariant *style = pd->settings->get(QString("pdiStyle"));
    if (! label)
      return 0;

    QVariant *width = pd->settings->get(QString("pdiWidth"));
    if (! width)
      return 0;

    if (! getADX(1,
                 period->toInt(),
                 label->toString()))
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(label->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(label->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  // ADX
  show = pd->settings->get(QString("adxShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("adxColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *label = pd->settings->get(QString("adxLabel"));
    if (! label)
      return 0;

    QVariant *style = pd->settings->get(QString("adxStyle"));
    if (! label)
      return 0;

    QVariant *width = pd->settings->get(QString("adxWidth"));
    if (! width)
      return 0;

    if (! getADX(2,
                 period->toInt(),
                 label->toString()))
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(label->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(label->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  // ADXR
  show = pd->settings->get(QString("adxrShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("adxrColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *label = pd->settings->get(QString("adxrLabel"));
    if (! label)
      return 0;

    QVariant *style = pd->settings->get(QString("adxrStyle"));
    if (! label)
      return 0;

    QVariant *width = pd->settings->get(QString("adxrWidth"));
    if (! width)
      return 0;

    if (! getADX(3,
                 period->toInt(),
                 label->toString()))
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(label->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(label->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  return 1;
}

int
ADX::getADX (int type, int period, QString key)
{
  if (! g_symbol)
    return 0;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "ADX::ADX: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  const int size = keys.size();

  if (size ==0) return 0;

  TA_Real high[MAX_SIZE];
  TA_Real low[MAX_SIZE];
  TA_Real close[MAX_SIZE];
  TA_Real out[MAX_SIZE];
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

  switch (type)
  {
    case 0: // MDI
      rc = TA_MINUS_DI(0, dpos - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1: // PDI
      rc = TA_PLUS_DI(0, dpos - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2: // ADX
      rc = TA_ADX(0, dpos - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3: // ADXR
      rc = TA_ADXR(0, dpos - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      return 0;
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "ADX::getADX: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(key, out[outLoop]);
    keyLoop--;
    outLoop--;
  }

  return 1;
}

int
ADX::settings (PluginData *pd)
{
  CurveLineType clt;
  Entity *command = new Entity;
  command->set(QString("plugin"), new QVariant(QString("ADX")));
  command->set(QString("type"), new QVariant(QString("indicator")));

  command->set(QString("period"), new QVariant(14));

  command->set(QString("mdiColor"), new QVariant(QString("red")));
  command->set(QString("mdiLabel"), new QVariant(QString("MDI")));
  command->set(QString("mdiStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("mdiWidth"), new QVariant(1));
  command->set(QString("mdiShow"), new QVariant(TRUE));

  command->set(QString("pdiColor"), new QVariant(QString("green")));
  command->set(QString("pdiLabel"), new QVariant(QString("PDI")));
  command->set(QString("pdiStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("pdiWidth"), new QVariant(1));
  command->set(QString("pdiShow"), new QVariant(TRUE));

  command->set(QString("adxColor"), new QVariant(QString("yellow")));
  command->set(QString("adxLabel"), new QVariant(QString("ADX")));
  command->set(QString("adxStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("adxWidth"), new QVariant(1));
  command->set(QString("adxShow"), new QVariant(TRUE));

  command->set(QString("adxrColor"), new QVariant(QString("blue")));
  command->set(QString("adxrLabel"), new QVariant(QString("ADXR")));
  command->set(QString("adxrStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("adxrWidth"), new QVariant(1));
  command->set(QString("adxrShow"), new QVariant(TRUE));

  pd->settings = command;

  return 1;
}

int
ADX::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(adx, ADX);
