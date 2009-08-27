/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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
#include "FileButton.h"
#include "Config.h"




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
    void signalReloadToolBars ();

  public:
    Preferences (QWidget *);
    ~Preferences ();
    void createGeneralPage ();
    void createDatabasePage ();
    void createColorPage ();
    void createFontPage ();
    void createMTPage(); // main tool bar
    void createCTPage(); // chart tool bar
    void loadSettings ();

  public slots:
    void slotSave ();
    void slotModified ();
    void cancelPressed ();

  private:
    int modified;
    Config config;
    QTabWidget *tabs;
    QDialogButtonBox *buttonBox;    
    QSpinBox *bs1Spinner;
    QSpinBox *bs2Spinner;
    QSpinBox *bs3Spinner;
    QSpinBox *tabRows;
    ColorButton *backgroundColorButton;
    ColorButton *borderColorButton;
    ColorButton *gridColorButton;
    FontButton *plotFontButton;
    FontButton *appFontButton;

    QComboBox *dbDriver;
    QLineEdit *dbHostName;
    QLineEdit *dbUserName;
    QLineEdit *dbPassword;
    QLineEdit *dateFormat;
    QLineEdit *dateColumn;
    QLineEdit *openColumn;
    QLineEdit *highColumn;
    QLineEdit *lowColumn;
    QLineEdit *closeColumn;
    QLineEdit *volumeColumn;
    QLineEdit *oiColumn;
    QLineEdit *indexTable;
    QLineEdit *symbolColumn;
    QLineEdit *nameColumn;
    QLineEdit *exchangeColumn;
    FileButton *dbFile;

    // on MainToolbar    
    QCheckBox *quitBtnCheck;
    QCheckBox *prefBtnCheck;
    QCheckBox *sidePanelBtnCheck;
    QCheckBox *gridBtnCheck;
    QCheckBox *scaleToScreenBtnCheck;
    QCheckBox *crosshairBtnCheck;
    QCheckBox *drawModeBtnCheck;
    QCheckBox *newIndicatorBtnCheck;
    QCheckBox *dataWindowBtnCheck;
    QCheckBox *mainQuoteBtnCheck;
    QCheckBox *helpButtonCheck;

    // on ChartToolbar
    QCheckBox *sliderCheck;
    QCheckBox *barsToLoadFieldCheck;
    QCheckBox *barSpSpinboxCheck;
    QCheckBox *cmps60BtnCheck;
    QCheckBox *cmps15BtnCheck;
    QCheckBox *cmps5BtnCheck;
    QCheckBox *cmpsDayBtnCheck;
    QCheckBox *cmpsWkyBtnCheck;
    QCheckBox *cmpsMtyBtnCheck;
    QCheckBox *cmpsComboBoxCheck;
    QCheckBox *recentComboBoxCheck;
};

#endif
