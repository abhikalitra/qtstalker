/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "QuotePlugin.h"
#include "Setting.h"
#include <qstring.h>

class COT : public QuotePlugin
{
  Q_OBJECT

  public:
    COT ();
    virtual ~COT ();
    void update ();
    void parse ();
    void saveData (Setting *);
    void cancelUpdate ();
    QString getSymbol (QString);
    void loadSettings ();
    void saveSettings ();
    void prefDialog (QWidget *);

  public slots:
    void slotCopyFileDone (QString);
    void slotGetFileDone (bool);
    void timeoutError ();

  private:
    QString format;
    QString url;
};

extern "C"
{
  QuotePlugin * createQuotePlugin ();
}

