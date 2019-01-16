#include "C_NApi_SocketComRplidar_Protocol.h"

bool C_NApi_SocketComRplidar_Protocol::AddDataAndCheckMsgCompleted(const QByteArray &newData)
{
    // Add new data to the previous data
    m_storedData.append(newData);

    // There is no header ..
    while(m_storedData.count() >= 360*4*2)
    {
        // Check if the msg can be used
        // 0 bytes of header
        // 360*4*2 bytes for 360 measures
        // 0 bytes of checksum
        if(m_storedData.count() >= 360*4*2) {
            // Copy the usefull message part
            uint16_t data[360 + 1];

            memcpy(m_P, m_storedData.constData(), sizeof(m_P));

            for (int i=0; i<361; i++) {
                data[i] = 0;
            }

            for (int i=0; i<360; i++) {
                uint16_t i_R = 0;

                double d_X = m_P[i].x;
                double d_Y = m_P[i].y;
                double d_R = sqrt(d_X*d_X + d_Y*d_Y);

                i_R = nearbyint(d_R);

                if(i_R != 0) {
                    double d_alpha = 0.0;
                    int i_alpha = 360;

                    if((d_X>0.0) || (d_X<0.0)) {
                        d_alpha = atan(d_Y/d_X);
                    } else {
                        if((d_Y>0.0))
                            d_alpha = +M_PI/2.0;
                        else
                            d_alpha = -M_PI/2.0;
                    }

                    if(d_X<0.0) {
                        d_alpha += M_PI;
                    }

                    if(d_alpha<0.0) {
                        d_alpha += 2.0*M_PI;
                    }

                    i_alpha = nearbyint(180.0*d_alpha/M_PI);

                    //data[i_alpha] = i_R;
                    //data[360-i_alpha] = i_R; /* FIXME : TODO : WHY?? */
                    if ((i_R<10) || (i_R>3000))
                        data[360-i_alpha] = 4000;
                    else
                        data[360-i_alpha] = i_R;
                }
            }

            // Remove the copied data from the original buffer
            m_storedData.remove(0, 360*4*2);

            // Use the data
            {
                emit SIG_AddNewRplidarData(data);
            }
        }
        else
        {
            // Not enough data, wait next incoming data
            return true;
        }
    }

    // All is useless
    m_storedData.clear();

    return true;
}

void C_NApi_SocketComRplidar_Protocol::ClearReceivedData(void)
{
    m_storedData.clear();
}

#include "moc_C_NApi_SocketComRplidar_Protocol.cpp"
