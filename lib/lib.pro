TEMPLATE = lib
 
CONFIG += thread warn_on
  
QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += QuotePlugin.h
HEADERS += IndicatorPlugin.h
HEADERS += ChartPlugin.h
HEADERS += DbPlugin.h
HEADERS += Setting.h
HEADERS += FuturesData.h
HEADERS += PlotLine.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += PrefDialog.h
HEADERS += SymbolDialog.h
HEADERS += SymbolButton.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
HEADERS += FileButton.h
HEADERS += Toolbar.h
HEADERS += Bar.h
HEADERS += BarDate.h
HEADERS += Config.h
HEADERS += FormulaEdit.h
HEADERS += FormulaInput.h
HEADERS += Indicator.h
HEADERS += COPlugin.h
HEADERS += HelpWindow.h
HEADERS += MyLineEdit.h
HEADERS += MySpinBox.h
HEADERS += MyComboBox.h
HEADERS += MySlider.h
HEADERS += MacroKey.h
HEADERS += Macro.h
HEADERS += MyListBox.h
HEADERS += Plot.h
HEADERS += DatePlot.h
HEADERS += ScalePlot.h
HEADERS += IndicatorPlot.h
HEADERS += Navigator.h
HEADERS += BarEdit.h

   
SOURCES += QuotePlugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += ChartPlugin.cpp
SOURCES += DbPlugin.cpp
SOURCES += Setting.cpp
SOURCES += FuturesData.cpp
SOURCES += PlotLine.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += PrefDialog.cpp
SOURCES += SymbolDialog.cpp
SOURCES += SymbolButton.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
SOURCES += FileButton.cpp
SOURCES += Toolbar.cpp
SOURCES += Bar.cpp
SOURCES += BarDate.cpp
SOURCES += Config.cpp
SOURCES += FormulaEdit.cpp
SOURCES += FormulaInput.cpp
SOURCES += Indicator.cpp
SOURCES += COPlugin.cpp
SOURCES += HelpWindow.cpp
SOURCES += MyLineEdit.cpp
SOURCES += MySpinBox.cpp
SOURCES += MyComboBox.cpp
SOURCES += MySlider.cpp
SOURCES += MacroKey.cpp
SOURCES += Macro.cpp
SOURCES += MyListBox.cpp
SOURCES += Plot.cpp
SOURCES += DatePlot.cpp
SOURCES += ScalePlot.cpp
SOURCES += IndicatorPlot.cpp
SOURCES += Navigator.cpp
SOURCES += BarEdit.cpp

    
TARGET = qtstalker
VERSION = 0.30.0

LIBS += -ldb
     
# linux options
unix:linux-g++:LIBS += -ldl

target.path = /usr/lib
INSTALLS += target

