#include "QuotePlugin.h"
#include <qstring.h>

class FG : public QuotePlugin
{
  Q_OBJECT

  public:
    FG ();
    virtual ~FG ();
    void download ();
    QString translateUS (QString);
    QString translateTY (QString);

  public slots:
    void parse ();
};

extern "C"
{
  Plugin * create ();
};
