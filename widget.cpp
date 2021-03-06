#include "widget.h"
#include "ui_widget.h"

#include <QSerialPortInfo>
#include<QMessageBox>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    enmuComPort();
    connect(serialPort, SIGNAL(readyRead()),
               this, SLOT(readDataSlot()));
}

Widget::~Widget()
{
    delete ui;
}
//枚举串口
void Widget::enmuComPort()
{
    QStringList serialPortInfoList;
    seialList.clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << info.portName() + " " + info.manufacturer()
                    + " " + info.description();
        serialPortInfoList.append(info.portName() + " " + info.manufacturer());
        seialList.append(info.portName());
    }
    ui->comboBox->clear();
    ui->comboBox->insertItems(0, serialPortInfoList);
}


void Widget::on_pushButton_clicked()
{
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits=QSerialPort::Data6;

        QSerialPort::StopBits stopBits=QSerialPort::OneStop;
        QSerialPort::Parity parity=QSerialPort::NoParity;
        QSerialPort::FlowControl flowControl=QSerialPort::NoFlowControl;
        /* 从界面获取串口号*/
       if(seialList.size()>0)
           name=seialList.at(ui->comboBox->currentIndex());//根据iNDEX找到对应的串口号
       else
           return;
       //从界面获取波特率
       baudRate=ui->comboBox_2->currentText().toInt();
       //从界面获取数据位
       switch(ui->comboBox_3->currentIndex())
       {
       case 0:
              dataBits=QSerialPort::Data5;
              break;
       case 1:
              dataBits=QSerialPort::Data6;
              break;
       case 2:
              dataBits=QSerialPort::Data7;
              break;
       case 3:
              dataBits=QSerialPort::Data8;
              break;
       default:
               break;
      }

      //获取停止位
/* 从界面获取停止位 */
    switch (ui->comboBox_4->currentIndex())
    {
    case 0:
        stopBits = QSerialPort::OneStop;
        break;
    case 1:
        stopBits = QSerialPort::OneAndHalfStop;
        break;
    case 2:
        stopBits = QSerialPort::TwoStop;
        break;
    default:
        break;
    }
    /* 从界面获取奇偶校验位 */
        switch (ui->comboBox_5->currentIndex())
        {
        case 0:
            parity = QSerialPort::NoParity;
            break;
        case 1:
            parity = QSerialPort::EvenParity;
            break;
        case 2:
            parity = QSerialPort::OddParity;
            break;
        case 3:
            parity = QSerialPort::SpaceParity;
            break;
        case 4:
            parity = QSerialPort::MarkParity;
            break;
        default:
            break;
        }

        /* 从界面获取流控位 */
        switch (ui->comboBox_6->currentIndex())
        {
        case 0:
            flowControl = QSerialPort::NoFlowControl;
            break;
        case 1:
            flowControl = QSerialPort::HardwareControl;
            break;
        case 2:
            flowControl = QSerialPort::SoftwareControl;
            break;
        default:
            break;
        }
    //配置串口参数
        serialPort->setPortName(name);
        serialPort->setBaudRate(baudRate);
        serialPort->setDataBits(dataBits);
        serialPort->setParity(parity);
        serialPort->setStopBits(stopBits);
        serialPort->setFlowControl(flowControl);

        if(!serialPort->isOpen())
        {
            if(!serialPort->open(QIODevice::ReadWrite))
            {
                QMessageBox::warning(this,QString::fromLocal8Bit("打开失败"),
                                     name+serialPort->errorString()
                                     );
                return ;
            }else
            {
               ui->pushButton->setText("关闭串口");
             }
        }else{
            serialPort->close();
            ui->pushButton->setText("打开串口");
        }
}

//发送数据
void Widget::on_pushButton_2_clicked()
{
    QString sendStr =ui->textEdit_2->toPlainText(); //接受纯文本
    QByteArray sendarry=sendStr.toLocal8Bit();
    if(serialPort->isWritable())
    {
        serialPort->write(sendarry);
    }
}

//接受数据
void Widget::readDataSlot()
{
//    QByteArray recvArry;
//    QString recvStr;
//    if(serialPort->isReadable())
//    {
//        recvArry = serialPort->readAll();
//        recvStr = QString::fromLocal8Bit(recvArry);
//        ui->textEdit->insertPlainText(recvStr);
//    }
}

char Widget::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
    return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
    return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
    return ch-'a'+10;
    else return (-1);
}


QByteArray Widget::QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
        i++;
        continue;
        }
        i++;
        if(i >= len)
        break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
        break;
        else
        hexdata = hexdata*16+lowhexdata;
        i++;
        //senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}
