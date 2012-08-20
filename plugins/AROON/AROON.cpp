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
#include "AROON.h"
#include "CurveLineType.h"
#include "Global.h"
#include "BarType.h"
#include "AROONDialog.h"

int
AROON::command (PluginData *pd)
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
AROON::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "AROON::dialog: invalid parent";
    return 0;
  }

  if (! pd->settings)
  {
    qDebug() << "AROON::dialog: invalid settings";
    return 0;
  }

  AROONDialog *dialog = new AROONDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;

  return 1;
}

int
AROON::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;

  QVariant *period = pd->settings->get(QString("period"));
  if (! period)
    return 0;

  QVariant *ulabel = pd->settings->get(QString("upLabel"));
  if (! ulabel)
    return 0;

  QVariant *dlabel = pd->settings->get(QString("downLabel"));
  if (! dlabel)
    return 0;

  if (! getAROON(period->toInt(), ulabel->toString(), dlabel->toString()))
    return 0;

  // up
  QVariant *show = pd->settings->get(QString("upShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("upColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *style = pd->settings->get(QString("upStyle"));
    if (! style)
      return 0;

    QVariant *width = pd->settings->get(QString("upWidth"));
    if (! width)
      return 0;

    // up
    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(ulabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(ulabel->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  // down
  show = pd->settings->get(QString("downShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("downColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *style = pd->settings->get(QString("downStyle"));
    if (! style)
      return 0;

    QVariant *width = pd->settings->get(QString("downWidth"));
    if (! width)
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(dlabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(dlabel->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  return 1;
}

int
AROON::getAROON (int period, QString ukey, QString dkey)
{
  if (! g_symbol)
    return 0;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "AROON::AROON: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  TA_Real high[MAX_SIZE];
  TA_Real low[MAX_SIZE];
  TA_Real out[MAX_SIZE];
  TA_Real out2[MAX_SIZE];
  TA_Integer outBeg;
  TA_Integer outNb;

  BarType bt;
  QString highKey = bt.indexToString(BarType::_HIGH);
  QString lowKey = bt.indexToString(BarType::_LOW);

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

    high[dpos] = (TA_Real) h;
    low[dpos] = (TA_Real) l;
    dpos++;
  }

  rc = TA_AROON(0, dpos - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0], &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "AROON::getAROON: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(dkey, out[outLoop]);
    bar->set(ukey, out2[outLoop]);
    keyLoop--;
    outLoop--;
  }

  return 1;
}

int
AROON::settings (PluginData *pd)
{
  CurveLineType clt;
  Entity *command = new Entity;
  command->set(QString("plugin"), new QVariant(QString("AROON")));
  command->set(QString("type"), new QVariant(QString("indicator")));

  command->set(QString("period"), new QVariant(10));

  command->set(QString("upColor"), new QVariant(QString("green")));
  command->set(QString("upLabel"), new QVariant(tr("Up")));
  command->set(QString("upStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("upWidth"), new QVariant(1));
  command->set(QString("upShow"), new QVariant(TRUE));

  command->set(QString("downColor"), new QVariant(QString("red")));
  command->set(QString("downLabel"), new QVariant(tr("Down")));
  command->set(QString("downStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("downWidth"), new QVariant(1));
  command->set(QString("downShow"), new QVariant(TRUE));

  pd->settings = command;

  return 1;
}

int
AROON::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(aroon, AROON);
