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
#include "FuturesData.h"
#include "Setting.h"
#include <qstring.h>
#include <qcombobox.h>

class CME : public QuotePlugin
{
  Q_OBJECT

  public:
    CME ();
    virtual ~CME ();
    void update ();
    void parseToday ();
    void parseHistory ();
    void saveTodayData (QStringList);
    void parse (Setting *);
    void cancelUpdate ();
    void loadSettings ();
    void saveSettings ();
    void prefDialog (QWidget *);

  public slots:
    void methodChanged (const QString &);
    void startDownload ();
    void fileDone (QString);
    void timeoutError ();

  private:
    FuturesData fd;
    QStringList urlList;
    int symbolLoop;
    QString file2;
    QString method;
    QString currentSymbol;
    QComboBox *symbolCombo;
    QStringList symbolList;
    QStringList downloadSymbolList;
    bool allSymbols;
    int downloadIndex;
    bool cancelFlag;
};

extern "C"
{
  QuotePlugin * createQuotePlugin ();
}

