/*
 *  TestSettings
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



#ifndef TESTSETTINGS_H
#define TESTSETTINGS_H

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QStringList>
#include <QString>
#include <QSpinBox>
#include <QToolButton>
#include <QGroupBox>
#include <QWidget>

#include "IndicatorBase.h"

class TestSettings : public QWidget
{
  Q_OBJECT

  public:
    TestSettings ();
    ~TestSettings ();
    void getScript (QString &);
    void setScript (QString &);
    bool getScriptCheck ();
    void setScriptCheck (bool);
    void getShellCommand (QString &);
    void setShellCommand (QString &);
    bool getLongCheck ();
    void setLongCheck (bool);
    bool getShortCheck ();
    void setShortCheck (bool);
    void getSymbol (QString &);
    void setSymbol (QString &);
    int getEnterField ();
    void setEnterField (int);
    int getExitField ();
    void setExitField (int);
    int getBars ();
    void setBars (int);
    int getBarLength ();
    void setBarLength (int);
    double getEntryComm ();
    void setEntryComm (double);
    double getExitComm ();
    void setExitComm (double);
    int getDelay ();
    void setDelay (int);
    double getAccount ();
    void setAccount (double);
    double getVolumePercentage ();
    void setVolumePercentage (double);
    double getTrailing ();
    void setTrailing (double);
    bool getTrailingCheck ();
    void setTrailingCheck (bool);
    void getEnterLongCombo (QString &);
    void setEnterLongCombo (QString &);
    void getExitLongCombo (QString &);
    void setExitLongCombo (QString &);
    void getEnterShortCombo (QString &);
    void setEnterShortCombo (QString &);
    void getExitShortCombo (QString &);
    void setExitShortCombo (QString &);
    IndicatorBase * getEnterLongIndicator ();
    IndicatorBase * getExitLongIndicator ();
    IndicatorBase * getEnterShortIndicator ();
    IndicatorBase * getExitShortIndicator ();
    void getEnterLongSettings (QString &d);
    void setEnterLongSettings (QString &d);
    void getExitLongSettings (QString &d);
    void setExitLongSettings (QString &d);
    void getEnterShortSettings (QString &d);
    void setEnterShortSettings (QString &d);
    void getExitShortSettings (QString &d);
    void setExitShortSettings (QString &d);

  public slots:
    void scriptButtonPressed ();
    void symbolButtonPressed ();
    void enterLongButtonPressed ();
    void exitLongButtonPressed ();
    void enterShortButtonPressed ();
    void exitShortButtonPressed ();
    void enterLongComboChanged ();
    void exitLongComboChanged ();
    void enterShortComboChanged ();
    void exitShortComboChanged ();
    void scriptCheckChanged (bool);

  private:
    QStringList priceList;
    QStringList barLengthList;
    QStringList indicatorList;

    QLineEdit *symbol;
    QGroupBox *scriptCheck;
    QGroupBox *longCheck;
    QGroupBox *shortCheck;
    QLineEdit *script;
    QLineEdit *shellCommand;
    QToolButton *scriptButton;
    QToolButton *enterLongButton;
    QToolButton *exitLongButton;
    QToolButton *enterShortButton;
    QToolButton *exitShortButton;
    QComboBox *enterField;
    QComboBox *exitField;
    QCheckBox *trailingCheck;
    QDoubleSpinBox *trailing;
    QSpinBox *bars;
    QSpinBox *delay;
    QDoubleSpinBox *entryComm;
    QDoubleSpinBox *exitComm;
    QComboBox *barLength;
    QDoubleSpinBox *account;
    QDoubleSpinBox *volumePercentage;
    QComboBox *enterLongCombo;
    QComboBox *exitLongCombo;
    QComboBox *enterShortCombo;
    QComboBox *exitShortCombo;
    IndicatorBase *enterLongIndicator;
    IndicatorBase *exitLongIndicator;
    IndicatorBase *enterShortIndicator;
    IndicatorBase *exitShortIndicator;
};

#endif

