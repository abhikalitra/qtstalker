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
#include <qstring.h>
#include "Setting.h"

class Yahoo : public QuotePlugin
{
  Q_OBJECT

  public:
    Yahoo ();
    virtual ~Yahoo ();
    void update ();
    QString parseDate (QString &);
    void parseHistory ();
    void parseQuote ();
    void loadSettings ();
    void saveSettings ();
    void prefDialog (QWidget *);
    void printErrorList ();
    void cancelUpdate ();
    void parseFundamental ();
    void loadAllSymbols ();
    void createHistoryUrls (QString &);
    void createAutoHistoryUrls (QString &, QString &);
    void createQuoteUrls (QString &);
    void createFundamentalUrls (QString &);
      
  public slots:
    void startDownload ();
    void fileDone (bool);
    void timeoutError ();

  private:
    QStringList symbolList;
    bool adjustment;
    QDateTime sdate;
    QDateTime edate;
    QString method;
    QPtrList<Setting> url;
    Setting *currentUrl;
    bool allSymbols;
    QString dataPath;
    QStringList fileList;
    QStringList errorList;
};

extern "C"
{
  QuotePlugin * createQuotePlugin ();
}

