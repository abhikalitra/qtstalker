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

#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <QStringList>
#include <QString>
#include <QTabWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLineEdit>

#include "ColorButton.h"
#include "FontButton.h"


class Preferences : public QDialog
{
  Q_OBJECT

  signals:
    void signalBackgroundColor (QColor);
    void signalBorderColor (QColor);
    void signalGridColor (QColor);
    void signalPlotFont (QFont);
    void signalAppFont (QFont);
    void signalLoadChart ();
    void signalRefreshChanged (int);
    void signalPS1Changed (int);
    void signalPS2Changed (int);

  public:
    Preferences (QWidget *);
    void createGeneralPage ();
//    void createDatabasePage ();
//    void createSQLPage ();
    void createColorPage ();
    void createFontPage ();

  public slots:
    void slotSave ();
    void slotModified ();
    void cancelPressed ();
    void slotRefreshModified ();

  private:
    QTabWidget *tabs;
    QDialogButtonBox *buttonBox;

    // flags
    int modified;
    int refreshModified;

    // main page
    QSpinBox *bs1Spinner;
    QSpinBox *bs2Spinner;
    QSpinBox *tabRows;
    QSpinBox *refreshSpinner;

    // color page
    ColorButton *backgroundColorButton;
    ColorButton *borderColorButton;
    ColorButton *gridColorButton;

    // font page
    FontButton *plotFontButton;
    FontButton *appFontButton;

    // db page
/*
    QComboBox *dbDriver;
    QLineEdit *dbHostName;
    QLineEdit *dbName;
    QLineEdit *dbUserName;
    QLineEdit *dbPassword;
*/

    // sql page
/*
    QLineEdit *dbAllSymbols;
    QLineEdit *dbSearchSymbols;
    QLineEdit *dbFirstDate;
    QLineEdit *dbLastDate;
    QLineEdit *dbGetSymbol;
*/
};

#endif
