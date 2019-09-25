#include "Widget.h"
#include "ui_Widget.h"
#include "DuJoystickManager.h"
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <iostream>
#include <QDateTime>
#include <QtDebug>
#include "qdebug.h"
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QProcess>
#include <QMediaPlayer>
#include <QSerialPortInfo>
#include <QFileDialog>
#include <QtCore>
#include <QDir>
#include <QDate>
#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <QImageWriter>
#include <QMediaService>
#include <QMediaRecorder>
//#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),
    m_serial(new QSerialPort(this))
{
    ui->setupUi(this);
    mJoyManager = new DuJoystickManager(this);

    timerCam = new QTimer(this);
    timerStick = new QTimer(this);
    timerDraw = new QTimer(this);
    timerRec = new QTimer(this);

    ui->labelRoll->setPixmap(QPixmap(":/new/prefix1/side2.png"));
    ui->btnCamRec->setEnabled(false);
    //ui->btnRstServo->styleSheet();
    videoData.camRecState = false;
    rovData.rstServo = false;
    rovData.comPortStatus = false;
    rovData.camIdx = 0;

    //ui->textEditDcFrontLeft->setText("100");
    //Servo scale default
    rovData.scaleFrontLeft = 100;
    rovData.scaleFrontRight = 100;
    rovData.scaleBackLeft = 100;
    rovData.scaleBackRight = 100;

    #ifdef COMPORT_USED
    //init_port();
    #endif

    mJoyManager->start();

    connect(mJoyManager, &DuJoystickManager::SDL_joyButtonDown, this, &Widget::manageJoystick);
    connect(timerRec, SIGNAL(timeout()), this, SLOT(update_rec_timer()));
    connect(timerStick, SIGNAL(timeout()), this, SLOT(scanCmd()));
    connect(timerDraw, SIGNAL(timeout()), this, SLOT(drawGraph()));
    //connect(m_serial, &QSerialPort::errorOccurred, this, &Widget::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &Widget::readData);
    connect(ui->comboBoxSerialPort, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &Widget::showPortInfo);

    //connect(timerCam, SIGNAL(timeout()), this, SLOT(update_window()));

    timerStick->start(2);
    //ui->verticalLayoutX->addWidget();
    //ui->verticalLayoutZ->addWidget();
    //timerBtn->start(20); //every 200 ms
    ui->labelAltitude->setText("0000");
    ui->labelTemperature->setText("00.00");

    ui->labelRoll->setPixmap(QPixmap(":/new/prefix1/front"));
    ui->labelYaw->setPixmap(QPixmap(":/new/prefix1/top"));
    ui->labelPitch->setPixmap(QPixmap(":/new/prefix1/side"));
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
Widget::~Widget()
{
    delete ui;
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::manageJoystick(int button)
{
    btnPressed = button;
    QString cmdValueStr = "";

    switch (button) {
    case BUTTON0:
        // ui->labelBtn0->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON1:
        // ui->labelBtn1->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON2:
        // ui->labelBtn2->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON3:
        // ui->labelBtn3->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON4:
        // ui->labelBtn4->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn6->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON5:
        // ui->labelBtn5->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn7->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON6:
        // ui->labelBtn6->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON7:
        // ui->labelBtn7->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn5->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON8:
        // ui->labelBtn8->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn10->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON9:
        // ui->labelBtn9->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn11->setStyleSheet("QLabel { background-color : grey; color : grey; }");

        // ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON10:
        // ui->labelBtn10->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case BUTTON11:
        // ui->labelBtn11->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;

    case HAT_0:
        // ui->labelBtnH0->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case HAT_UP:
        // ui->labelBtnH1->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case HAT_DOWN:
        // ui->labelBtnH4->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case HAT_RIGHT:
        // ui->labelBtnH2->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case HAT_LEFT:
        // ui->labelBtnH8->setStyleSheet("QLabel { background-color : red; color : green; }");
        // ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        // ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case X:
        cmdValueStr = QString::number(mJoyManager->queueCmd.front().cmdValue);
        ui->labelX->setText(cmdValueStr);

        ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn10->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        //ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        //ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        //ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        //ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case Y:
        cmdValueStr = QString::number(mJoyManager->queueCmd.front().cmdValue);
        ui->labelY->setText(cmdValueStr);

        ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn10->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case Z:
        cmdValueStr = QString::number(mJoyManager->queueCmd.front().cmdValue);
        ui->labelZ->setText(cmdValueStr);

        ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn11->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    case RZ:
        cmdValueStr = QString::number(mJoyManager->queueCmd.front().cmdValue);
        ui->labelRZ->setText(cmdValueStr);

        ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn11->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
        break;
    }
    //mJoyManager->queueCmd.pop(); //remove the first data queue
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_btnStartCam_clicked()
{
    if(ui->btnStartCam->text() == "START CAM"){
        qDebug() << "Start Cam";
        ui->btnStartCam->setText("STOP CAM");
        ui->btnCamRec->setEnabled(true);
        startCam();
    }else{
        if(ui->btnStartCam->text() == "STOP CAM"){
           qDebug() << "Stop Cam";
           ui->btnStartCam->setText("START CAM");
           ui->btnCamRec->setEnabled(false);
           if(videoData.camRecState == true){
                stopRecAction();
                videoData.camRecState = false;
           }
           stopCam();
        }
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_btnStopCam_clicked()
{
//    disconnect(timerCam, SIGNAL(timeout()), this, SLOT(update_window()));

//    cap.release();       //Camera stopped
//    video.release();     //Video release

//    Mat image = Mat::zeros(frame.size(),CV_8UC3);
//    qt_image = QImage((const unsigned char*) (image.data), image.cols, image.rows, QImage::Format_RGB888);
//    ui->labelCam->setPixmap(QPixmap::fromImage(qt_image));
//    ui->labelCam->resize(ui->labelCam->pixmap()->size());
//    cout << "camera is closed" << endl;
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
//Mat to QImage
//https://stackoverflow.com/questions/17127762/cvmat-to-qimage-and-back/17137998
//Add text to QImage
//https://stackoverflow.com/questions/8042303/is-it-possible-to-display-text-over-a-video-sequence-with-highgui-opencv
//https://docs.opencv.org/2.4/modules/highgui/doc/qt_new_functions.html?highlight=addtext
void Widget::update_window()
{
    cap >> frame;

    cvtColor(frame, frame, CV_BGR2RGB);

    //Get time
    QTime local(QTime::currentTime());
    //qDebug() << "Local time is:" << local;

    //Convert to String
    QString textImg = local.toString();

    std::string textImgStr = textImg.toLocal8Bit().constData();

    int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 1;
    int thickness = 2;
    //int baseline=0;

    //Size textSize = getTextSize(textImgStr, fontFace,fontScale, thickness, &baseline);
    //baseline += thickness;

    //Tune text position
    Point textOrg(20,(frame.rows - 20));

    //Put text
    putText(frame, textImgStr, textOrg, fontFace, fontScale,Scalar(244, 75, 46), thickness, 8);

    qt_image = QImage((const unsigned char*) (frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    ui->labelCam->setPixmap(QPixmap::fromImage(qt_image));
    ui->labelCam->resize(ui->labelCam->pixmap()->size());

    if(videoData.camRecState == true){ //if recording
        //Compress, first we need to converto Mat to QImage, frame is Mat
        //1. Qimage converto to ByteArray
        //qt_image_temp = Mat2QImage(frame);
        //2. Compress ByteArray
        //3. ByteArray convert to QImage
        //4. Qimage convert to Mat again
        //

        //step
        //1. mat to qimage

        QImage myimg = Mat2QImage(frame);
        QByteArray array;
        QBuffer buffer(&array);

        //2. qimage to qbytearray
        //3. compress qbytearray & write to Qimage
        // write image to memory buffer
        QImageWriter writer;
        writer.setDevice(&buffer);
        writer.setFormat("JP2");
        writer.setCompression(99);
        writer.write(myimg);

        //4. Qimage convert to Mat again
        cv::Mat myMat = QImage2Mat(myimg);
        if(!myMat.empty()){
           cvtColor(myMat, myMat, CV_RGB2BGR);
           video.write(myMat);
        }

        /*    if(!frame.empty()){
           cvtColor(frame, frame, CV_RGB2BGR);
           video.write(frame);
        }
        */
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::update_rec_timer(){
    //Get Date time and use it as filename
    QTime local(QTime::currentTime());
    //qDebug() << "Local time is:" << local;

    //Convert to String
    QString timeRec = local.toString();

    ui->labelRecord->setText(timeRec);

    //Remove forbiden character ":" from datetime
    //fileName.remove(QChar(':'), Qt::CaseInsensitive);
    //fileName = "C:/Video/" + fileName + ".avi";

    //std::string textFileName = fileName.toLocal8Bit().constData();
    //std::cout<< textFileName;
    //String fileSaved = "C:/Cideo/Fri Feb 22_10.40.16 2019.avi";

}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::scanCmd()
{
    //uint8_t cmdType;
    //int16_t cmdData;
    //int32_t cmdVal = 0;

    //cmdType = mJoyManager->queueCmd.front().cmdType;  //Get cmdType from front of queue
    //cmdData = mJoyManager->queueCmd.front().cmdValue; //Get cmdData from front of queue
    //Print Serial Tx here
    //Use PUSH to QUEUE to store joyStick value
    //FRONT to get first value, in widget.cpp
    //POP to delete, in widget.cpp
    if(!mJoyManager->queueCmd.empty()){
        switch (mJoyManager->queueCmd.front().cmdType) {
            case (BUTTON0):
                ui->labelBtn0->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn0\n");
                //writeData("A");
                break;
            case (BUTTON1):
                ui->labelBtn1->setStyleSheet("QLabel { background-color : red; color : green; }");
                 ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                 ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                 ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                 ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                 qDebug("Command Btn1\n");
                //writeData("B");
                break;
            case (BUTTON2):
                ui->labelBtn2->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn2\n");
                //writeData("C");
                break;
            case (BUTTON3):
                ui->labelBtn3->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn3\n");
                //writeData("D");
                break;
            case (BUTTON4):
                ui->labelBtn4->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn6->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn4\n");
                //writeData("E");
                break;
            case (BUTTON5):
                ui->labelBtn5->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn7->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn5\n");
                writeData("F");
                break;
            case (BUTTON6):
                ui->labelBtn6->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn6\n");
                //writeData("G");
                break;
            case (BUTTON7):
                ui->labelBtn7->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn5->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn7\n");
                writeData("H");
                break;
            case (BUTTON8):
                ui->labelBtn8->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn10->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn8\n");
                //writeData("I");
                break;
            case (BUTTON9):
                ui->labelBtn9->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn11->setStyleSheet("QLabel { background-color : grey; color : grey; }");

                ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn9\n");
                //writeData("J");
                break;
            case (BUTTON10):
                ui->labelBtn10->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn10\n");
                //writeData("K");
                break;
            case (BUTTON11):
                ui->labelBtn11->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Btn11\n");
                //writeData("L");
                break;
            case (HAT_0):
                ui->labelBtnH0->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Hat_0\n");
                //writeData("M");
                break;
            case (HAT_UP):
                ui->labelBtnH1->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Hat_Up\n");
                //writeData("N");
                break;
            case (HAT_DOWN):
                ui->labelBtnH4->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Hat_down\n");
                //writeData("O");
                break;
            case (HAT_LEFT):
                ui->labelBtnH8->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Hat_left\n");
                //writeData("P");
                break;
            case (HAT_RIGHT):
                ui->labelBtnH2->setStyleSheet("QLabel { background-color : red; color : green; }");
                ui->labelBtnH1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtnH8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
                qDebug("Command Hat_right\n");
                //writeData("Q");
                break;
            case (X):
                //qDebug("Command X\n");
                rovData.cmdValX = mJoyManager->queueCmd.front().cmdValue;
                /*Catatan : skala stick -32767 s/d 32767
                dikonversi menjadi 0 s/d 1000
                Formula
                1. Nilai stick + 32767
                2. Dikali 1000
                3. Dibagi 65536
                contoh :
                nilai stick -1000
                1. +32767 jadi 22767
                2. dikali 1000 jd 22767000
                3. Dibagi 65536 jd 347
                */
                rovData.cmdValX = (rovData.cmdValX + 32768)*1000;
                rovData.cmdValX = rovData.cmdValX/SCALE_TICK;
                rovData.cmdValX = 1000 - rovData.cmdValX;

                //Check right or Left
                //if(rovData.cmdValX <= 500){ //right
                //   rovData.cmdValX = rovData.cmdValX*rovData.scaleBackRight;
                //   rovData.cmdValX = rovData.cmdValX/100;
                //}else{ //> 500, left
                //    rovData.cmdValX = rovData.cmdValX*rovData.scaleBackLeft;
                //    rovData.cmdValX = rovData.cmdValX/100;
                //}

                qDebug() << "X" << rovData.cmdValX;
                #ifdef COMPORT_USED
                writeData("X");
                sprintf(charValue,"%d\n",rovData.cmdValX);
                writeData(charValue);
                memset(charValue,0,10);
                //--writeData();
                //--writeData("\n");
                #endif
                //rovData.cmdValX = rovData.cmdValX + 1000;
                //ui->labelX->setText(QString(rovData.cmdValX));
                break;
            case (Y):
                //qDebug("Command Y\n");
                rovData.cmdValY = mJoyManager->queueCmd.front().cmdValue;
                rovData.cmdValY = (rovData.cmdValY + 32768)*1000;
                rovData.cmdValY = rovData.cmdValY/SCALE_TICK;
                rovData.cmdValY = 1000 - rovData.cmdValY;
                qDebug() << "Y" << rovData.cmdValY;
                #ifdef COMPORT_USED
                writeData("Y");
                sprintf(charValue,"%d\n",rovData.cmdValY);
                writeData(charValue);
                memset(charValue,0,10);
                //--writeData();
                //--writeData("\n");
                #endif
                //rovData.cmdValY = rovData.cmdValY + 1000;
                //ui->labelY->setText(QString(rovData.cmdValY));
                break;
            case (Z):
                 //qDebug("Command Z\n");
                 rovData.cmdValZ = mJoyManager->queueCmd.front().cmdValue;
                 rovData.cmdValZ = (rovData.cmdValZ + 32768)*1000;
                 rovData.cmdValZ = rovData.cmdValZ/SCALE_TICK;
                 qDebug() << "Z" << rovData.cmdValZ;
                 #ifdef COMPORT_USED
                 //writeData("Z");
                 //sprintf(charValue,"%d\n",rovData.cmdValZ);
                 //writeData(charValue);
                 //memset(charValue,0,10);
                 //--writeData("\n");
                 #endif
                 //rovData.cmdValZ = rovData.cmdValY + 1000;
                 //ui->labelZ->setText(QString(rovData.cmdValZ));
                 break;
            case (RZ):
                //qDebug("Command RZ\n");
                rovData.cmdValRZ = mJoyManager->queueCmd.front().cmdValue;
                rovData.cmdValRZ = (rovData.cmdValRZ + 32768)*1000;
                rovData.cmdValRZ = rovData.cmdValRZ/SCALE_TICK;
                qDebug() << "RZ" << rovData.cmdValRZ;
                #ifdef COMPORT_USED
                writeData("T");
                sprintf(charValue,"%d\n",rovData.cmdValRZ);
                writeData(charValue);
                memset(charValue,0,10);
                //--writeData();
                //--writeData("\n");
                #endif
                //rovData.cmdValRZ = rovData.cmdValRZ + 1000;
                //ui->labelRZ->setText(QString(rovData.cmdValRZ));
                break;
            default:
                break;
        }
        mJoyManager->queueCmd.pop(); //remove the first data queue
    }

    //Get queue from cmd Scale
    int value = 0;
    if(!queueCmdScale.empty()){
        value = queueCmdScale.front().cmdValue;
        switch (queueCmdScale.front().cmdType){
        case SCALE_DC_FRONT_LEFT:
            qDebug() << "SCALE_DC_FRONT_LEFT:" << value;
            #ifdef COMPORT_USED
            writeData("S");
            sprintf(charValue,"%d\n",value);
            writeData(charValue);
            memset(charValue,0,10);
            #endif
            break;
        case SCALE_DC_FRONT_RIGHT:
            qDebug() << "SCALE_DC_FRONT_RIGHT:" << value;
            #ifdef COMPORT_USED
            writeData("U");
            sprintf(charValue,"%d\n",value);
            writeData(charValue);
            memset(charValue,0,10);
            #endif
            break;
        case SCALE_DC_BACK_LEFT:
            qDebug() << "SCALE_DC_BACK_LEFT:" << value;
            #ifdef COMPORT_USED
            writeData("V");
            sprintf(charValue,"%d\n",value);
            writeData(charValue);
            memset(charValue,0,10);
            #endif
            break;
        case SCALE_DC_BACK_RIGHT:
            qDebug() << "SCALE_DC_BACK_RIGHT:" << value;
            #ifdef COMPORT_USED
            writeData("W");
            sprintf(charValue,"%d\n",value);
            writeData(charValue);
            memset(charValue,0,10);
            #endif
            break;
        case SCALE_DC_RST:
            qDebug() << "SCALE_DC_RESET";
            #ifdef COMPORT_USED
            writeData("R\n");
            //sprintf(charValue,"%d\n",value);
            //writeData(charValue);
            //memset(charValue,0,10);
            #endif
            break;//
        case SCALE_GET_CONFIG:
            qDebug() << "SCALE_GET_CONFIG";
            #ifdef COMPORT_USED
            writeData("@\n");
            //sprintf(charValue,"%d\n",value);
            //writeData(charValue);
            //memset(charValue,0,10);
            #endif
            break;
        default:
            break;

        }
        queueCmdScale.pop();
    }

    //Check variable Cmd Reset Servo
    //if(rovData.rstServo == true){
    //    //Reset Servo Cmd
    //    #ifdef COMPORT_USED
    //    writeData("R");
    //    #endif
    //    rovData.rstServo = false;
    //}

    //Check variable Cmd Scale Dc Brushless

    ui->labelTemperature->setText(QString::fromLocal8Bit(rovData.temperature));
    ui->labelAltitude->setText(QString::fromLocal8Bit(rovData.depth));
    ui->labelVDepth->setText(QString::fromLocal8Bit(rovData.adcDepth));

    //BATT
    float battPercentage = atoi(rovData.adcCont);
          battPercentage = (battPercentage/1023)*33;
          battPercentage = (battPercentage-6.6)/1.8;
          battPercentage = battPercentage*100;
    ui->labelVCont->setText(QString::number(battPercentage));

    battPercentage = atoi(rovData.adcMotor);
    battPercentage = (battPercentage/1023)*33;
    battPercentage = (battPercentage-9.9)/2.7;
    battPercentage = battPercentage*100;
    ui->labelVMotor->setText(QString::number(battPercentage));

    //ui->labelVCont->setText(QString::fromLocal8Bit(rovData.adcCont));
    //ui->labelVMotor->setText(QString::fromLocal8Bit(rovData.adcMotor));

    //Compas & Direction
    ui->labelRoll_2->setText(QString::fromLocal8Bit(rovData.axis_x));
    ui->labelPitch_2->setText(QString::fromLocal8Bit(rovData.axis_y));
    ui->labelYaw_2->setText(QString::fromLocal8Bit(rovData.axis_z));


    //Leak Detector
    if(rovData.leak == true){
        ui->labelLeak->setStyleSheet("QLabel { background-color : red; color : red; }");
    }else{
        ui->labelLeak->setStyleSheet("QLabel { background-color : green; color : green; }");
    }
    //JoyStick
    //ui->labelZ->setText(QString::fromLocal8Bit(rovData.axis_x));

    //scale Servo
    //ui->->setText(QString::fromLocal8Bit(rovData.axis_x));

}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::init_port()
{
    //QTextStream out(stdout);
    QString portname = rovData.comPortName;//"COM12";
    m_serial = new QSerialPort(portname);

    m_serial->setPortName(portname);//("COM12");
    m_serial->setBaudRate(QSerialPort::Baud57600);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setStopBits(QSerialPort::OneStop);

    if(ui->btnComPort->text() == "OPEN"){
        if(rovData.comPortStatus == false){
            if (!m_serial->open(QIODevice::ReadWrite)){
                qDebug() << "Open Port error";
                rovData.comPortStatus = false;
                ui->labelSerPortStatus->setText("PORT OPEN FAIL");
                ui->labelSerial->setText("SERIAL CLOSE");
                ui->labelSerial->setStyleSheet("QLabel { background-color : red; color : red; }");
            }else{
                qDebug() << "Open Port OK";
                ui->labelSerial->setText("SERIAL OPEN");
                ui->labelSerPortStatus->setText("PORT OPEN OK");
                ui->labelSerial->setStyleSheet("QLabel { background-color : green; color : green; }");
                //ui->btnComPort->setText("CLOSE");
                timerDraw->start(20); //1 detik 50 kali
                rovData.comPortStatus = true;
                ui->btnRstServo->setEnabled(true);
                ui->btnGetConfig->setEnabled(true);
                ui->checkBoxDcBackLeft->setEnabled(true);
                ui->checkBoxDcBackRight->setEnabled(true);
                ui->checkBoxDcFrontLeft->setEnabled(true);
                ui->checkBoxDcFrontRight->setEnabled(true);
            }
        }
    }else{
        if(ui->btnComPort->text() == "CLOSE"){
            //m_serial->close();
            //ui->labelSerial->setText("SERIAL CLOSE");
            //ui->labelSerPortStatus->setText("PORT CLOSE");
            //ui->btnComPort->setText("OPEN");
            //qDebug() << "Close OK";
        }
    }

    /*if(rovData.comPortStatus == false){ //if not open
        if (!m_serial->open(QIODevice::ReadWrite))
        {
            //QMessageBox::warning(this, "port error", "cannot open port");
            qDebug() << "Open Port error";
            rovData.comPortStatus = false;
            ui->labelSerPortStatus->setText("PORT OPEN FAIL");
            ui->labelSerial->setText("SERIAL CLOSE");
            ui->labelSerial->setStyleSheet("QLabel { background-color : red; color : red; }");
        }else{
            qDebug() << "Open Port OK";
            ui->labelSerial->setText("SERIAL OPEN");
            ui->labelSerPortStatus->setText("PORT OPEN OK");
            ui->labelSerial->setStyleSheet("QLabel { background-color : green; color : green; }");
            timerDraw->start(20); //1 detik 50 kali
            rovData.comPortStatus = true;
        }
    }*/
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::closeSerialPort()
{
    if (m_serial->isOpen()){
        m_serial->close();
        qDebug() << "Close OK\n";
    }
}
/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::readData()
{
    const QByteArray myData = m_serial->readAll();
    qDebug() << "Serial received:" << myData;
    int X_idx = 0;
    bool X_found,Y_found,Z_found,Depth_found,Temp_found, VDepth_found, VMotor_found, VCont_found = false;
    bool getScaleConfig, frontLeft, frontRight, backLeft, backRight = false;

    for (int i = 0; i < myData.size(); ++i) {
        //Identification
        if (myData.at(i) == 'X'){ //Roll
            X_idx = i + 1;
            X_found = true;
            Y_found = false;
            Z_found = false;
            Depth_found = false;
            Temp_found = false;
            VDepth_found = false;
            VMotor_found = false;
            VCont_found = false;

            getScaleConfig = false;
            frontLeft = false;
            frontRight = false;
            backLeft = false;
            backRight = false;
        }else{
            if (myData.at(i) == 'Y'){ //Yaw
                X_idx = i + 1;
                X_found = false;
                Y_found = true;
                Z_found = false;
                Depth_found = false;
                Temp_found = false;
                VDepth_found = false;
                VMotor_found = false;
                VCont_found = false;

                getScaleConfig = false;
                frontLeft = false;
                frontRight = false;
                backLeft = false;
                backRight = false;
            }else{
                if (myData.at(i) == 'Z'){ //Pitch
                    X_idx = i + 1;
                    X_found = false;
                    Y_found = false;
                    Z_found = true;
                    Depth_found = false;
                    Temp_found = false;
                    VDepth_found = false;
                    VMotor_found = false;
                    VCont_found = false;

                    getScaleConfig = false;
                    frontLeft = false;
                    frontRight = false;
                    backLeft = false;
                    backRight = false;
                }else{
                    if (myData.at(i) == 'D'){ //Depth (mdpl = meter di atas permukaan laut)
                        X_idx = i + 1;
                        X_found = false;
                        Y_found = false;
                        Z_found = false;
                        Depth_found = true;
                        Temp_found = false;
                        VDepth_found = false;
                        VMotor_found = false;
                        VCont_found = false;

                        getScaleConfig = false;
                        frontLeft = false;
                        frontRight = false;
                        backLeft = false;
                        backRight = false;
                    }else{
                        if (myData.at(i) == 'S'){ // Temperature
                            X_idx = i + 1;
                            X_found = false;
                            Y_found = false;
                            Z_found = false;
                            Depth_found = false;
                            Temp_found = true;
                            VDepth_found = false;
                            VMotor_found = false;
                            VCont_found = false;

                            getScaleConfig = false;
                            frontLeft = false;
                            frontRight = false;
                            backLeft = false;
                            backRight = false;
                        }else{
                              if (myData.at(i) == 'T'){ // adcMotor
                                  X_idx = i + 1;
                                  X_found = false;
                                  Y_found = false;
                                  Z_found = false;
                                  Depth_found = false;
                                  Temp_found = false;
                                  VDepth_found = false;
                                  VMotor_found = true;
                                  VCont_found = false;

                                  getScaleConfig = false;
                                  frontLeft = false;
                                  frontRight = false;
                                  backLeft = false;
                                  backRight = false;
                              }else{
                                  if (myData.at(i) == 'U'){ // adcController
                                      X_idx = i + 1;
                                      X_found = false;
                                      Y_found = false;
                                      Z_found = false;
                                      Depth_found = false;
                                      Temp_found = false;
                                      VDepth_found = false;
                                      VMotor_found = false;
                                      VCont_found = true;

                                      getScaleConfig = false;
                                      frontLeft = false;
                                      frontRight = false;
                                      backLeft = false;
                                      backRight = false;
                                  }else{
                                      if (myData.at(i) == 'V'){ // adcDepth
                                          X_idx = i + 1;
                                          X_found = false;
                                          Y_found = false;
                                          Z_found = false;
                                          Depth_found = false;
                                          Temp_found = false;
                                          VDepth_found = true;
                                          VMotor_found = false;
                                          VCont_found = false;

                                          getScaleConfig = false;
                                          frontLeft = false;
                                          frontRight = false;
                                          backLeft = false;
                                          backRight = false;
                                      }else{
                                          if (myData.at(i) == 'W'){ // Leak occur
                                              rovData.leak = true;
                                          }else{
                                                if (myData.at(i) == 'P'){ // Leak cancel
                                                    rovData.leak = false;
                                                }else{
                                                    if (myData.at(i) == 'A'){ // front left
                                                        X_idx = i + 1;
                                                        X_found = false;
                                                        Y_found = false;
                                                        Z_found = false;
                                                        Depth_found = false;
                                                        Temp_found = false;
                                                        VDepth_found = false;
                                                        VMotor_found = false;
                                                        VCont_found = false;

                                                        getScaleConfig = false;
                                                        frontLeft = true;
                                                        frontRight = false;
                                                        backLeft = false;
                                                        backRight = false;
                                                    }else{
                                                        if (myData.at(i) == 'B'){ // front Right
                                                            X_idx = i + 1;
                                                            X_found = false;
                                                            Y_found = false;
                                                            Z_found = false;
                                                            Depth_found = false;
                                                            Temp_found = false;
                                                            VDepth_found = false;
                                                            VMotor_found = false;
                                                            VCont_found = false;

                                                            getScaleConfig = false;
                                                            frontLeft = false;
                                                            frontRight = true;
                                                            backLeft = false;
                                                            backRight = false;
                                                        }else{
                                                            if (myData.at(i) == 'C'){ // back left
                                                                X_idx = i + 1;
                                                                X_found = false;
                                                                Y_found = false;
                                                                Z_found = false;
                                                                Depth_found = false;
                                                                Temp_found = false;
                                                                VDepth_found = false;
                                                                VMotor_found = false;
                                                                VCont_found = false;

                                                                getScaleConfig = false;
                                                                frontLeft = false;
                                                                frontRight = false;
                                                                backLeft = true;
                                                                backRight = false;
                                                            }else{
                                                                if (myData.at(i) == 'E'){ // back right
                                                                    X_idx = i + 1;
                                                                    X_found = false;
                                                                    Y_found = false;
                                                                    Z_found = false;
                                                                    Depth_found = false;
                                                                    Temp_found = false;
                                                                    VDepth_found = false;
                                                                    VMotor_found = false;
                                                                    VCont_found = false;

                                                                    getScaleConfig = false;
                                                                    frontLeft = false;
                                                                    frontRight = false;
                                                                    backLeft = false;
                                                                    backRight = true;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                          }
                                      }
                                  }
                              }
                        }
                    }
                }
            }
        }

        //Parsing
        if(X_found){
            if(myData.at(i) == '\n'){
                rovData.axis_x[i-X_idx] = '\0';
                qDebug() << "X " << rovData.axis_x;
                //mCompassNeedleX->setCurrentValue(atoi(rovData.axis_x)+180);
                countRoll = atoi(rovData.axis_x)+180;
                //memset(axis_x,'\0',10);
                //break;
            }else{
               if(((myData.at(i) >= 48) && (myData.at(i) <= 57)) ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                    rovData.axis_x[i-X_idx] = myData.at(i);
               }
            }
        }else{
            if(Y_found){
                if(myData.at(i) == '\n'){
                    rovData.axis_y[i-X_idx] = '\0';
                    qDebug() << "Y " << rovData.axis_y;
                    //mCompassNeedleY->setCurrentValue(atoi(rovData.axis_y)+180);
                    countYaw = atoi(rovData.axis_y)+180;
                    //memset(axis_x,'\0',10);
                    //break;
                }else{
                   if(((myData.at(i) >= 48) && (myData.at(i) <= 57)) ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                        rovData.axis_y[i-X_idx] = myData.at(i);
                   }
                }
            }else{
                if(Z_found){
                    if(myData.at(i) == '\n'){
                        rovData.axis_z[i-X_idx] = '\0';
                        qDebug() << "Z " << rovData.axis_z;
                        //mCompassNeedleZ->setCurrentValue(atoi(rovData.axis_z)+180);
                        countPitch = atoi(rovData.axis_z)+180;
                        //memset(axis_x,'\0',10);
                        //break;
                    }else{
                       if(((myData.at(i) >= 48) && (myData.at(i) <= 57)) ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                            rovData.axis_z[i-X_idx] = myData.at(i);
                       }
                    }
                }else{
                    if(Depth_found){
                        if(myData.at(i) == '\n'){
                            rovData.depth[i-X_idx] = '\0';
                            qDebug() << "D " << rovData.depth;
                            //memset(axis_x,'\0',10);
                            //break;
                        }else{
                           if(((myData.at(i) >= 48) && (myData.at(i) <= 57)) ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                rovData.depth[i-X_idx] = myData.at(i);
                           }
                        }
                    }else{
                        if(Temp_found){
                            if(myData.at(i) == '\n'){
                                rovData.temperature[i-X_idx] = '\0';
                                //ui->labelTemperature->setText("37.25");
                                //memset(rovData.temperature,'\0',10);
                                qDebug() << "D " << rovData.temperature;
                               // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                //break;
                            }else{
                               if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                    rovData.temperature[i-X_idx] = myData.at(i);
                               }
                            }
                        }else{
                            if(VDepth_found){
                                if(myData.at(i) == '\n'){
                                    rovData.adcDepth[i-X_idx] = '\0';
                                    //ui->labelTemperature->setText("37.25");
                                    //memset(rovData.temperature,'\0',10);
                                    qDebug() << "T " << rovData.adcDepth;
                                   // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                    //break;
                                }else{
                                   if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                        rovData.adcDepth[i-X_idx] = myData.at(i);
                                   }
                                }
                            }else{
                                if(VMotor_found){
                                    if(myData.at(i) == '\n'){
                                        rovData.adcMotor[i-X_idx] = '\0';
                                        //ui->labelTemperature->setText("37.25");
                                        //memset(rovData.temperature,'\0',10);
                                        qDebug() << "U " << rovData.adcMotor;
                                        // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                        //break;
                                    }else{
                                        if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                            rovData.adcMotor[i-X_idx] = myData.at(i);
                                        }
                                    }
                                }else{
                                    if(VCont_found){
                                        if(myData.at(i) == '\n'){
                                            rovData.adcCont[i-X_idx] = '\0';
                                            //ui->labelTemperature->setText("37.25");
                                            //memset(rovData.temperature,'\0',10);
                                            qDebug() << "U " << rovData.adcCont;
                                            // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                            //break;
                                        }else{
                                            if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                                rovData.adcCont[i-X_idx] = myData.at(i);
                                            }
                                        }
                                    }else{
                                        if(frontLeft){
                                            if(myData.at(i) == '\n'){
                                                rovData.charFrontLeft[i-X_idx] = '\0';
                                                //ui->labelTemperature->setText("37.25");
                                                //memset(rovData.temperature,'\0',10);
                                                qDebug() << "Front Left " << rovData.charFrontLeft;
                                                // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                                //break;
                                            }else{
                                                if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                                    rovData.charFrontLeft[i-X_idx] = myData.at(i);
                                                }
                                            }
                                        }else{
                                            if(frontRight){
                                                if(myData.at(i) == '\n'){
                                                    rovData.charFrontRight[i-X_idx] = '\0';
                                                    //ui->labelTemperature->setText("37.25");
                                                    //memset(rovData.temperature,'\0',10);
                                                    qDebug() << "FrontRight " << rovData.charFrontRight;
                                                    // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                                    //break;
                                                }else{
                                                    if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                                        rovData.charFrontRight[i-X_idx] = myData.at(i);
                                                    }
                                                }
                                            }else{
                                                if(backLeft){
                                                    if(myData.at(i) == '\n'){
                                                        rovData.charBackLeft[i-X_idx] = '\0';
                                                        //ui->labelTemperature->setText("37.25");
                                                        //memset(rovData.temperature,'\0',10);
                                                        qDebug() << "BackLeft " << rovData.charBackLeft;
                                                        // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                                        //break;
                                                    }else{
                                                        if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                                            rovData.charBackLeft[i-X_idx] = myData.at(i);
                                                        }
                                                    }
                                                }else{
                                                    if(backRight){
                                                        if(myData.at(i) == '\n'){
                                                            rovData.charBackRight[i-X_idx] = '\0';
                                                            //ui->labelTemperature->setText("37.25");
                                                            //memset(rovData.temperature,'\0',10);
                                                            qDebug() << "BackRight " << rovData.charBackRight;
                                                            // ui->labelTemperature->setText(QString::fromUtf8(rovData.temperature));
                                                            //break;
                                                        }else{
                                                            if(((myData.at(i) >= 45) && (myData.at(i) <= 57))){// ||((myData.at(i) == 45) ||(myData.at(i) == 46))){
                                                                rovData.charBackRight[i-X_idx] = myData.at(i);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //qDebug() << rovData.axis_x;
    //QTextStream out(stdout);
    //out << rovData.axis_x << endl;
    //addDataPoint(atof(axis_x));
    //mCompassNeedle->setCurrentValue(atoi(axis_x)+180);
    //memset(rovData.axis_x,'\0',10);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::drawGraph(){
    //Draw Roll
    QPixmap ship(":/new/prefix1/front");
    QPixmap rotate(ship.size());

    QPainter p(&rotate);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.translate(rotate.size().width() / 2, rotate.size().height() / 2);
    p.rotate(countRoll);
    p.translate(-rotate.size().width() / 2, -rotate.size().height() / 2);

    p.drawPixmap(0, 0, ship);
    p.end();

    ui->labelRoll->setPixmap (rotate);

    //Draw Yaw
    QPixmap shipy(":/new/prefix1/top");
    QPixmap rotatey(shipy.size());

    QPainter py(&rotate);
    py.setRenderHint(QPainter::Antialiasing);
    py.setRenderHint(QPainter::SmoothPixmapTransform);
    py.setRenderHint(QPainter::HighQualityAntialiasing);
    py.translate(rotate.size().width() / 2, rotate.size().height() / 2);
    py.rotate(countYaw);
    py.translate(-rotate.size().width() / 2, -rotate.size().height() / 2);

    py.drawPixmap(0, 0, shipy);
    py.end();

    ui->labelYaw->setPixmap (rotate);

    //Draw Pitch
    QPixmap shipp(":/new/prefix1/side");
    QPixmap rotatep(shipp.size());

    QPainter pp(&rotate);
    pp.setRenderHint(QPainter::Antialiasing);
    pp.setRenderHint(QPainter::SmoothPixmapTransform);
    pp.setRenderHint(QPainter::HighQualityAntialiasing);
    pp.translate(rotate.size().width() / 2, rotate.size().height() / 2);
    pp.rotate(countYaw);
    pp.translate(-rotate.size().width() / 2, -rotate.size().height() / 2);

    pp.drawPixmap(0, 0, shipp);
    pp.end();

    ui->labelPitch->setPixmap (rotate);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::clearBtn(){
    qDebug() << "Reset Button\n";
    ui->labelBtn0->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn1->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn2->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn3->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn4->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn5->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn6->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn7->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn8->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn9->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn10->setStyleSheet("QLabel { background-color : grey; color : grey; }");
    ui->labelBtn11->setStyleSheet("QLabel { background-color : grey; color : grey; }");
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool Widget::startCam(){
    //cap.open(0);
    cap.open(rovData.camIdx);
    if(!cap.isOpened())  // Check if we succeeded
    {
        cout << "camera is not open" << endl;
        videoData.camOpenState = false;
        ui->btnCamRec->setEnabled(false);
        return false;
    }
    else
    {
        cout << "camera is open" << endl;
        connect(timerCam, SIGNAL(timeout()), this, SLOT(update_window()));
        timerCam->start(20);//(20);
        cout << "timer is started" << endl;
        videoData.camOpenState = true;
        ui->btnCamRec->setEnabled(true);
        //initVideo();
        return true;
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool Widget::initVideo(){
    try {
       //Get Date time and use it as filename
       QDateTime local(QDateTime::currentDateTime());
       qDebug() << "Local time is:" << local;

       //Convert to String
       QString fileName = local.toString();

       //Remove forbiden character ":" from datetime
       fileName.remove(QChar(':'), Qt::CaseInsensitive);
       fileName = "C:/Video/" + fileName + ".avi";

       std::string textFileName = fileName.toLocal8Bit().constData();
       std::cout<< textFileName;
       //String fileSaved = "C:/Cideo/Fri Feb 22_10.40.16 2019.avi";

       //Video writer here
       // Default resolution of the frame is obtained.The default resolution is system dependent.
       int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
       int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

       qDebug() << "frame width:" << frame_width;
       qDebug() << "frame height:" << frame_height;
       //Create video file first
       //CV_WRAP virtual bool open(const String& filename, int fourcc, double fps,
       //                  Size frameSize, bool isColor = true);
       video.open(textFileName,CV_FOURCC('M','J','P','G'),25,cv::Size(frame_width,frame_height));
       videoData.camVidState = true;
       return true;

   } catch (exception &error) {
        cout << "video open error" << endl;
        videoData.camVidState = false;
        return false;
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
//Compressi File
//http://amin-ahmadi.com/2016/03/30/how-to-compress-filesdata-using-qt/
//https://stackoverflow.com/questions/5441401/compressing-iplimage-to-jpeg-on-memory
bool Widget::stopCam(){
    disconnect(timerCam, SIGNAL(timeout()), this, SLOT(update_window()));
    cap.release();       //Camera stopped
    //video.release();     //Video release

    //Mat image = Mat::zeros(frame.size(),CV_8UC3);
    //qt_image = QImage((const unsigned char*) (image.data), image.cols, image.rows, QImage::Format_RGB888);
    //ui->labelCam->setPixmap(QPixmap::fromImage(qt_image));
    //ui->labelCam->resize(ui->labelCam->pixmap()->size());
    //cout << "camera is closed" << endl;
    return true;
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool Widget::saveVideo(){
    video.release();     //Video release
    return true;
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_btnCamRec_clicked()
{
    if(ui->btnCamRec->text() == "START REC"){
        qDebug() << "Start Cam";
        ui->btnCamRec->setText("STOP-SAVE");
        videoData.camRecState = true;
        initVideo();
        ui->labelRecStatus->setText("RECORDING");
        timerRec->start(1000); //1 second update
    }else{
        if(ui->btnCamRec->text() == "STOP-SAVE"){
            stopRecAction();
        }
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::stopRecAction(){
    qDebug() << "Stop Cam";
    ui->btnCamRec->setText("START RECORD");
    ui->labelRecStatus->setText("READY");
    videoData.camRecState = false;
    timerRec->stop();
    saveVideo();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
QImage Widget::Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
cv::Mat Widget::QImage2Mat(QImage const& src)
{
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}


/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
//void Widget::on_btnRstMcu_clicked()
//{
    //Incoming Reset Servo Cmd
//    rovData.rstServo = true;
//}

void Widget::on_btnRstServo_clicked()
{
    //Incoming Reset Servo Cmd
    rovData.rstServo = true;
    qDebug() << "Cmd Servo Reset";

    cmdScale.cmdType = SCALE_DC_RST;
    cmdScale.cmdValue = 0;

    std::lock_guard<std::mutex> guard(cmdScaleMutex);//protect queue
    queueCmdScale.push(cmdScaleDcStruct{cmdScale.cmdType,cmdScale.cmdValue});//store data to queue

    //#ifdef COMPORT_USED
    //writeData("R\n");
    //#endif

    //test open external program
    //QString program = "C://Windows/notepad.exe";//"/home/sabri/KHG/pcd_viewer";

    //QStringList arguments;

    //QProcess *myProcess = new QProcess(this);
    //myProcess->start(program,(QStringList) arguments<< "C://Windows/notepad.exe");

}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    const QStringList list = ui->comboBoxSerialPort->itemData(idx).toStringList();
}
//Writing text file in Qt
//https://stackoverflow.com/questions/4916193/creating-writing-into-a-new-file-in-qt
//https://forum.qt.io/topic/79519/writing-data-into-a-text-file
//https://gist.github.com/diniremix/1429352

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_horizontalSliderDCFrontLeft_valueChanged(int value)
{
    ui->lineEditDcFrontLeft->setText(QString::number(value));
}


/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_checkBoxDcFrontLeft_stateChanged(int arg1)
{
   // ui->textEditDcFrontLeft->setDisabled(true);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_checkBoxDcFrontLeft_toggled(bool checked)
{
    if(checked){
        ui->lineEditDcFrontLeft->setDisabled(true);
        ui->horizontalSliderDCFrontLeft->setDisabled(true);
        rovData.scaleFrontLeft = ui->lineEditDcFrontLeft->text().toInt();
        if(rovData.scaleFrontLeft > 99){
           rovData.scaleFrontLeft = 100;
        }else{
            if(rovData.scaleFrontLeft < 1){
                rovData.scaleFrontLeft = 1;
            }
        }
        cmdScale.cmdType = SCALE_DC_FRONT_LEFT;
        cmdScale.cmdValue = rovData.scaleFrontLeft;

        std::lock_guard<std::mutex> guard(cmdScaleMutex);//protect queue
        queueCmdScale.push(cmdScaleDcStruct{cmdScale.cmdType,cmdScale.cmdValue});//store data to queue
        qDebug() << "Scale Front Left" << rovData.scaleFrontLeft;
    }else{
        ui->lineEditDcFrontLeft->setEnabled(true);
        ui->horizontalSliderDCFrontLeft->setEnabled(true);
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_textEditDcFrontLeft_textChanged()
{
    //QString scale = ui->lineEditDcFrontLeft->text();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_horizontalSliderDCFrontRight_valueChanged(int value)
{
     ui->lineEditDcFrontRight->setText(QString::number(value));
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_checkBoxDcFrontRight_toggled(bool checked)
{
    if(checked){
        ui->lineEditDcFrontRight->setDisabled(true);
        ui->horizontalSliderDCFrontRight->setDisabled(true);
        rovData.scaleFrontRight = ui->lineEditDcFrontRight->text().toInt();
        if(rovData.scaleFrontRight > 99){
           rovData.scaleFrontRight = 100;
        }else{
            if(rovData.scaleFrontRight < 1){
                rovData.scaleFrontRight = 1;
            }
        }
        cmdScale.cmdType = SCALE_DC_FRONT_RIGHT;
        cmdScale.cmdValue = rovData.scaleFrontRight;

        std::lock_guard<std::mutex> guard(cmdScaleMutex);//protect queue
        queueCmdScale.push(cmdScaleDcStruct{cmdScale.cmdType,cmdScale.cmdValue});//store data to queue
        qDebug() << "Scale FrontRight" << rovData.scaleFrontRight;
    }else{
        ui->lineEditDcFrontRight->setEnabled(true);
        ui->horizontalSliderDCFrontRight->setEnabled(true);
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_horizontalSliderDCBackLeft_valueChanged(int value)
{
    ui->lineEditDcBackLeft->setText(QString::number(value));
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_checkBoxDcBackLeft_toggled(bool checked)
{
    if(checked){
        ui->lineEditDcBackLeft->setDisabled(true);
        ui->horizontalSliderDCBackLeft->setDisabled(true);
        rovData.scaleBackLeft = ui->lineEditDcBackLeft->text().toInt();
        if(rovData.scaleBackLeft > 99){
           rovData.scaleBackLeft = 100;
        }else{
            if(rovData.scaleBackLeft < 1){
                rovData.scaleBackLeft = 1;
            }
        }
        cmdScale.cmdType = SCALE_DC_BACK_LEFT;
        cmdScale.cmdValue = rovData.scaleBackLeft;

        std::lock_guard<std::mutex> guard(cmdScaleMutex);//protect queue
        queueCmdScale.push(cmdScaleDcStruct{cmdScale.cmdType,cmdScale.cmdValue});//store data to queue

        qDebug() << "Scale BackLeft" << rovData.scaleBackLeft;
    }else{
        ui->lineEditDcBackLeft->setEnabled(true);
        ui->horizontalSliderDCBackLeft->setEnabled(true);
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/

//void Widget::on_horizontalSliderDCFrontRight_2_valueChanged(int value)
//{
//    ui->lineEditDcBackLeft->setText(QString::number(value));
//}

void Widget::on_horizontalSliderDCBackRight_valueChanged(int value)
{
      ui->lineEditDcBackRight->setText(QString::number(value));
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_checkBoxDcBackRight_toggled(bool checked)
{
    if(checked){
        ui->lineEditDcBackRight->setDisabled(true);
        ui->horizontalSliderDCBackRight->setDisabled(true);
        rovData.scaleBackRight = ui->lineEditDcBackRight->text().toInt();
        if(rovData.scaleBackRight > 99){
           rovData.scaleBackRight = 100;
        }else{
            if(rovData.scaleBackRight < 1){
                rovData.scaleBackRight = 1;
            }
        }

        cmdScale.cmdType = SCALE_DC_BACK_RIGHT;
        cmdScale.cmdValue = rovData.scaleBackRight;

        std::lock_guard<std::mutex> guard(cmdScaleMutex);//protect queue
        queueCmdScale.push(cmdScaleDcStruct{cmdScale.cmdType,cmdScale.cmdValue});//store data to queue

        qDebug() << "Scale BackRight" << rovData.scaleBackRight;
    }else{
        ui->lineEditDcBackRight->setEnabled(true);
        ui->horizontalSliderDCBackRight->setEnabled(true);
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_btnGetConfig_clicked()
{
    cmdScale.cmdType = SCALE_GET_CONFIG;
    cmdScale.cmdValue = 0;

    std::lock_guard<std::mutex> guard(cmdScaleMutex);//protect queue
    queueCmdScale.push(cmdScaleDcStruct{cmdScale.cmdType,cmdScale.cmdValue});//store data to queue
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_btnRefresh_clicked()
{
    //Serial Ports
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        //QString s = QObject::tr("Port: ") + info.portName() + "\n";
        QString s = info.portName();
                    //+ QObject::tr("Location: ") + info.systemLocation() + "\n"
                    //+ QObject::tr("Description: ") + info.description() + "\n"
                    //+ QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
                    //+ QObject::tr("Serial number: ") + info.serialNumber() + "\n"
                    //+ QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                    //+ QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
                    //+ QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

        //auto label = new QLabel(s);
        //layout->addWidget(label);
        ui->comboBoxSerialPort->addItem(s);
    }

    //Cameras
    int p = 0;
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        QString cams = cameraInfo.description();
        //int cams = cameraInfo.deviceName();
        //QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        //videoDeviceAction->setCheckable(true);
        //videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        //if (cameraInfo == QCameraInfo::defaultCamera())
        //    videoDeviceAction->setChecked(true);

        //ui->menuDevices->addAction(videoDeviceAction);
        camData[p].camName = cams;
        camData[p].camIdx = p;
        p++;

        ui->comboBoxCam->addItem(cams);
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_comboBoxSerialPort_currentIndexChanged(const QString &arg1)
{
    rovData.comPortName = arg1;
    qDebug() << "selected port =" << rovData.comPortName;
    ui->btnComPort->setEnabled(true);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_btnComPort_clicked()
{
    init_port();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void Widget::on_comboBoxCam_currentTextChanged(const QString &arg1)
{
    int p = 0;
    rovData.camName = arg1;
    for(p=0;p<3;p++){
        if(camData[p].camName == arg1){
            //rovData.camName = arg1;
            rovData.camIdx = p;
            break;
        }
    }

    //Enabled Button Camera Open
    ui->btnCamRec->setEnabled(true);
    ui->btnStartCam->setEnabled(true);
    //rovData.camIdx =;
}
