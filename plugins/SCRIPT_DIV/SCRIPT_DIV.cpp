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

#include "SCRIPT_DIV.h"
#include "BarType.h"
#include "Global.h"


DialogUser *
SCRIPT_DIV::dialog (QWidget *p, Entity *settings)
{
  DialogUser *dialog = new DialogUser(p);
  dialog->setGUI(settings);
  return dialog;
}

int
SCRIPT_DIV::run (Entity *settings, QList<Curve *> &, QList<Marker *> &)
{
  if (! g_symbol)
    return 0;
  
  Setting *input = settings->get(QString("input"));
  if (! input)
    return 0;
  
  Setting *input2 = settings->get(QString("input2"));
  if (! input2)
    return 0;
  
  Setting *label = settings->get(QString("label"));
  if (! label)
    return 0;
  
  if (! getDIV(input->toString(), input2->toString(), label->toString()))
    return 0;

  return 1;
}

int
SCRIPT_DIV::getDIV (QString ikey, QString i2key, QString okey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "SCRIPT_DIV::getDIV: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double v;
    if (! bar->get(ikey, v))
    {
      // is key a number?
      bool ok;
      v = ikey.toDouble(&ok);
      if (! ok)
        continue;
    }
    
    double v2;
    if (! bar->get(i2key, v2))
    {
      // is key a number?
      bool ok;
      v2 = i2key.toDouble(&ok);
      if (! ok)
        continue;
    }
    
    input[dpos] = (TA_Real) v;
    input2[dpos] = (TA_Real) v2;
    dpos++;
  }

  rc = TA_DIV (0, dpos - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "SCRIPT_DIV::getDIV: TA-Lib error" << rc;
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

Entity *
SCRIPT_DIV::settings ()
{
  Entity *command = new Entity;
  int order = 0;
  
  // plugin
  Setting *set = new Setting;
  set->setString(QString("SCRIPT_DIV"), QString(), -1);
  command->set(QString("plugin"), set);
  
  // entity type
  set = new Setting;
  set->setString(QString("indicator"), QString(), -1);
  command->set(QString("type"), set);

  // input
  BarType bt;
  set = new Setting;
  set->setString(bt.indexToString(BarType::_CLOSE), tr("Input"), order++);
  command->set(QString("input"), set);

  // input 2
  set = new Setting;
  set->setString(bt.indexToString(BarType::_CLOSE), tr("Input 2"), order++);
  command->set(QString("input2"), set);
  
  // label
  set = new Setting;
  set->setString(QString("DIV"), tr("Label"), order++);
  command->set(QString("label"), set);

  return command;
}


// do not remove
Q_EXPORT_PLUGIN2(script_div, SCRIPT_DIV);
