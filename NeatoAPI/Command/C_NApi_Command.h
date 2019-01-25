#ifndef C_NAPI_COMMAND_H
#define C_NAPI_COMMAND_H


#include <QTimer>

#include "NeatoAPI/SerialCom/C_NApi_SerialCom.h"

class C_NApi_Command : public QObject
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_Command();

    // Destructor
    ~C_NApi_Command();

signals:
    void SIG_ExecuteCmd(C_NApi_SerialCom::enum_MvtCmd cmd, int speed, double param);

};

#endif // C_NAPI_COMMAND_H
