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

#ifndef INDICATOR_PLUGIN_DIALOG_HPP
#define INDICATOR_PLUGIN_DIALOG_HPP

#include <QString>
#include <QHash>
#include <QFormLayout>

#include "Indicator.h"
#include "Dialog.h"

class IndicatorPluginDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone (Indicator);

  public:
    enum Type
    {
      _Color,
      _Plot,
      _Text,
      _Int,
      _Double,
      _Combo,
      _Input,
      _Check,
      _File,
      _MA
    };
    
    IndicatorPluginDialog (Indicator &);
    virtual ~IndicatorPluginDialog ();
    int addTab (QString);
    void addColor (int tab, int id, QString label, QString color);
    void addPlot (int tab, int id, QString label, QString item);
    void addText (int tab, int id, QString label, QString item);
    void addInt (int tab, int id, QString label, int value, int high, int low);
    void addDouble (int tab, int id, QString label, double value, double high, double low);
    void addCombo (int tab, int id, QString label, QStringList list, QString item);
    void addInput (int tab, int id, QString label, QString item);
    void addCheck (int tab, int id, QString label, int value);
    void addFile (int tab, int id, QString label, QString file);
    void addMA (int tab, int id, QString label, QString type);

  public slots:
    virtual void done ();

  protected:
    Indicator _indicator;
    QHash<int, QFormLayout *> _forms;
    QHash<int, QWidget *> _widgets;
    QHash<int, Type> _types;
};

#endif
