#ifndef C_NAPI_SOCKETCOM_RPLIDAR_PROTOCOL_H
#define C_NAPI_SOCKETCOM_RPLIDAR_PROTOCOL_H

#include "stdlib.h"
#include "math.h"

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QEventLoop>
#include <QTimer>

#include "NeatoAPI/Constants.h"

#if 1 /* FIXME : DEBUG : HACK GOLDO */
typedef struct _my_point_t {
    float x;
    float y;
} my_point_t;
#endif

class C_NApi_SocketComRplidar_Protocol: public QObject
{
    Q_OBJECT

public:
    // Constructor
    C_NApi_SocketComRplidar_Protocol(void)
    {
    };

    // Destructor
    ~C_NApi_SocketComRplidar_Protocol(){};

    // Add data to the previous data
    // then check if the message is completed
    bool AddDataAndCheckMsgCompleted(const QByteArray &data);
    void ClearReceivedData(void);

    inline void delay__ms(int waitTime__ms)
    {
        QEventLoop loop;
        QTimer t;
        t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
        t.start(waitTime__ms);
        loop.exec();
    }

signals:
    // New Lds Data
    //void SIG_NewRplidarData(uint32_t tabMeasures__mm[360]);
    void SIG_AddNewRplidarData(uint16_t * ptMeasuresData);

private:
    // Cumulated data
    QByteArray m_storedData;

    // Begin message marker
    QByteArray m_BEGIN_MSG_MARKER = QByteArray("RPLIDAR", 7);

    int m_NbProcessedBytes = 0;
    uint32_t m_TabDist__mm[360];

#if 1 /* FIXME : DEBUG : HACK GOLDO */
    my_point_t m_P[400];
#endif
};

#endif // C_NAPI_SOCKETCOM_RPLIDAR_PROTOCOL_H
