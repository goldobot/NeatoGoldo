#include "C_NApi_SocketComRplidar_Settings.h"


C_NApi_SocketComRplidar_Settings::C_NApi_SocketComRplidar_Settings(QWidget *parent) :
    QDialog(parent),
    m_ptUi(new Ui::C_NApi_SocketComRplidar_Settings),
    m_ptIntValidator(new QIntValidator(0, 4000000, this))
{
    // Configure the HMI
    m_ptUi->setupUi(this);

    /* FIXME : TODO */

    FillSocketsParameters();
    FillSocketsInfo();

    UpdateSettings();
}

C_NApi_SocketComRplidar_Settings::~C_NApi_SocketComRplidar_Settings()
{
    delete m_ptUi;
}

C_NApi_SocketComRplidar_Settings::Settings C_NApi_SocketComRplidar_Settings::settings() const
{
    return m_currentSettings;
}

void C_NApi_SocketComRplidar_Settings::SLOT_ShowSocketInfo(int idx)
{
    /* FIXME : TODO */
}

void C_NApi_SocketComRplidar_Settings::SLOT_ApplySettings()
{
    UpdateSettings();
    hide();
}

void C_NApi_SocketComRplidar_Settings::FillSocketsParameters()
{
    m_ptUi->UDPPort->setText(QStringLiteral("1412"));
    /* FIXME : TODO */
}

void C_NApi_SocketComRplidar_Settings::FillSocketsInfo()
{
    /* FIXME : TODO */
}

void C_NApi_SocketComRplidar_Settings::UpdateSettings()
{
    /* FIXME : TODO */
}


#include "moc_C_NApi_SocketComRplidar_Settings.cpp"
