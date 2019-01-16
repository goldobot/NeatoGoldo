#include "C_NApi_SerialCom_Settings.h"


C_NApi_SerialCom_Settings::C_NApi_SerialCom_Settings(QWidget *parent) :
    QDialog(parent),
    m_ptUi(new Ui::C_NApi_SerialCom_Settings),
    m_ptIntValidator(new QIntValidator(0, 4000000, this))
{
    // Configure the HMI
    m_ptUi->setupUi(this);

    m_ptUi->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(m_ptUi->applyButton, &QPushButton::clicked,
            this, &C_NApi_SerialCom_Settings::SLOT_ApplySettings);
#if 0 /* FIXME : DEBUG : FDE */
    connect(m_ptUi->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &C_NApi_SerialCom_Settings::SLOT_ShowPortInfo);
    connect(m_ptUi->baudRateBox,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &C_NApi_SerialCom_Settings::SLOT_CheckCustomBaudRatePolicy);
    connect(m_ptUi->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &C_NApi_SerialCom_Settings::SLOT_CheckCustomDevicePathPolicy);
#else
    void (QComboBox::*my_signal1)(int) = &QComboBox::currentIndexChanged;
    connect(m_ptUi->serialPortInfoListBox, my_signal1,
            this, &C_NApi_SerialCom_Settings::SLOT_ShowPortInfo);
    connect(m_ptUi->baudRateBox,  my_signal1,
            this, &C_NApi_SerialCom_Settings::SLOT_CheckCustomBaudRatePolicy);
    connect(m_ptUi->serialPortInfoListBox, my_signal1,
            this, &C_NApi_SerialCom_Settings::SLOT_CheckCustomDevicePathPolicy);
#endif

    FillPortsParameters();
    FillPortsInfo();

    m_ptUi->serialPortInfoListBox->setCurrentIndex(1);

    UpdateSettings();
}

C_NApi_SerialCom_Settings::~C_NApi_SerialCom_Settings()
{
    delete m_ptUi;
}

C_NApi_SerialCom_Settings::Settings C_NApi_SerialCom_Settings::settings() const
{
    return m_currentSettings;
}

void C_NApi_SerialCom_Settings::SLOT_ShowPortInfo(int idx)
{
    if (idx == -1)
        return;

    const QStringList list = m_ptUi->serialPortInfoListBox->itemData(idx).toStringList();
    m_ptUi->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    m_ptUi->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    m_ptUi->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    m_ptUi->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    m_ptUi->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    m_ptUi->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void C_NApi_SerialCom_Settings::SLOT_ApplySettings()
{
    UpdateSettings();
    hide();
}

void C_NApi_SerialCom_Settings::SLOT_CheckCustomBaudRatePolicy(int idx)
{
    const bool isCustomBaudRate = !m_ptUi->baudRateBox->itemData(idx).isValid();
    m_ptUi->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_ptUi->baudRateBox->clearEditText();
        QLineEdit *edit = m_ptUi->baudRateBox->lineEdit();
        edit->setValidator(m_ptIntValidator);
    }
}

void C_NApi_SerialCom_Settings::SLOT_CheckCustomDevicePathPolicy(int idx)
{
    const bool isCustomPath = !m_ptUi->serialPortInfoListBox->itemData(idx).isValid();
    m_ptUi->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        m_ptUi->serialPortInfoListBox->clearEditText();
}

void C_NApi_SerialCom_Settings::FillPortsParameters()
{
    m_ptUi->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ptUi->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_ptUi->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_ptUi->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ptUi->baudRateBox->addItem(tr("Custom"));
    m_ptUi->baudRateBox->setCurrentIndex(3);

    m_ptUi->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ptUi->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ptUi->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ptUi->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ptUi->dataBitsBox->setCurrentIndex(3);

    m_ptUi->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    m_ptUi->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    m_ptUi->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    m_ptUi->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_ptUi->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);
    m_ptUi->parityBox->setCurrentIndex(0);

    m_ptUi->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ptUi->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ptUi->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    m_ptUi->stopBitsBox->setCurrentIndex(0);

    m_ptUi->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_ptUi->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_ptUi->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    m_ptUi->flowControlBox->setCurrentIndex(0);
}

void C_NApi_SerialCom_Settings::FillPortsInfo()
{
    m_ptUi->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
#if 0 /* FIXME : DEBUG : FDE */
        serialNumber = info.serialNumber();
#else
        serialNumber = "666";
#endif
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        m_ptUi->serialPortInfoListBox->addItem(list.first(), list);
    }

    m_ptUi->serialPortInfoListBox->addItem(tr("Custom"));
}

void C_NApi_SerialCom_Settings::UpdateSettings()
{
    m_currentSettings.name = m_ptUi->serialPortInfoListBox->currentText();

    if (m_ptUi->baudRateBox->currentIndex() == 4) {
        m_currentSettings.baudRate = m_ptUi->baudRateBox->currentText().toInt();
    } else {
        m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    m_ptUi->baudRateBox->itemData(m_ptUi->baudRateBox->currentIndex()).toInt());
    }
    m_currentSettings.stringBaudRate = QString::number(m_currentSettings.baudRate);

    m_currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                m_ptUi->dataBitsBox->itemData(m_ptUi->dataBitsBox->currentIndex()).toInt());
    m_currentSettings.stringDataBits = m_ptUi->dataBitsBox->currentText();

    m_currentSettings.parity = static_cast<QSerialPort::Parity>(
                m_ptUi->parityBox->itemData(m_ptUi->parityBox->currentIndex()).toInt());
    m_currentSettings.stringParity = m_ptUi->parityBox->currentText();

    m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                m_ptUi->stopBitsBox->itemData(m_ptUi->stopBitsBox->currentIndex()).toInt());
    m_currentSettings.stringStopBits = m_ptUi->stopBitsBox->currentText();

    m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                m_ptUi->flowControlBox->itemData(m_ptUi->flowControlBox->currentIndex()).toInt());
    m_currentSettings.stringFlowControl = m_ptUi->flowControlBox->currentText();

    m_currentSettings.localEchoEnabled = m_ptUi->localEchoCheckBox->isChecked();
}


#include "moc_C_NApi_SerialCom_Settings.cpp"
