#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "QcGaugeWidget/source/qcgaugewidget.h"
#include <QSerialPort>
#include <iostream>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <queue>
#include <QMediaPlayer>
#include <QSerialPortInfo>

using namespace cv;
using namespace std;

#define SCALE_TICK 65536
#define COMPORT_USED 1

namespace Ui
{
class Widget;
}

enum scaleCmdType{
    SCALE_DC_FRONT_LEFT,
    SCALE_DC_FRONT_RIGHT,
    SCALE_DC_BACK_LEFT,
    SCALE_DC_BACK_RIGHT,
    SCALE_DC_RST,
    SCALE_GET_CONFIG
};

struct rovStruct{
    //ROV variables
    char axis_x[10];
    char axis_y[10];
    char axis_z[10];

    char depth[10];
    char temperature[10];
    char voltBatt[10];
    char adcMotor[10];
    char adcCont[10];
    char adcDepth[10];

    char charFrontLeft[10];
    char charFrontRight[10];
    char charBackLeft[10];
    char charBackRight[10];

    bool lamp;
    bool leak;
    bool auto_heading;
    bool auto_depth;
    bool rstServo;

    int32_t cmdValX;
    int32_t cmdValY;
    int32_t cmdValZ;
    int32_t cmdValRZ;

    uint8_t scaleFrontLeft;
    uint8_t scaleFrontRight;
    uint8_t scaleBackLeft;
    uint8_t scaleBackRight;

    QString comPortName;
    bool comPortStatus;

    QString camName;
    int camIdx;
};

struct camStruct{
    QString camName;
    int camIdx;
};

struct videoStruct{
    bool camOpenState;
    bool camRecState;
    bool camVidState;
};

struct cmdScaleDcStruct{
     //uint8_t cmdCount;
     uint8_t cmdType;
     int16_t cmdValue;
};

class DuJoystickManager;

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    bool mutexObject = false;
    cmdScaleDcStruct cmdScale;
    queue <cmdScaleDcStruct> queueCmdScale;
    mutex cmdScaleMutex;

private slots:
    void on_btnStartCam_clicked();
    void on_btnStopCam_clicked();
    void update_window();
    void update_rec_timer();
    void writeData(const QByteArray &data);
    void scanCmd();

    void init_port();
    void closeSerialPort();
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void drawGraph();
    void on_btnCamRec_clicked();

    void on_btnRstMcu_clicked();

    void on_btnRstServo_clicked();

    void on_horizontalSliderDCFrontLeft_valueChanged(int value);

    void on_checkBoxDcFrontLeft_stateChanged(int arg1);

    void on_checkBoxDcFrontLeft_toggled(bool checked);

    void on_textEditDcFrontLeft_textChanged();

    void on_horizontalSliderDCFrontRight_valueChanged(int value);

    void on_checkBoxDcFrontRight_toggled(bool checked);

    void on_horizontalSliderDCBackLeft_valueChanged(int value);

    void on_checkBoxDcBackLeft_toggled(bool checked);

    void on_horizontalSliderDCFrontRight_2_valueChanged(int value);

    void on_horizontalSliderDCBackRight_valueChanged(int value);

    void on_checkBoxDcBackRight_toggled(bool checked);

    void on_btnGetConfig_clicked();

    void on_btnRefresh_clicked();

    void on_comboBoxSerialPort_currentIndexChanged(const QString &arg1);

    void on_btnComPort_clicked();

    void on_comboBoxCam_currentTextChanged(const QString &arg1);

private:
    Ui::Widget *ui;
    QTimer *timerCam;
    QTimer *timerStick;
    QTimer *timerDraw;
    QTimer *timerRec;

    QcGaugeWidget *mCompassGaugeX;
    QcNeedleItem *mCompassNeedleX;

    QcGaugeWidget *mCompassGaugeY;
    QcNeedleItem *mCompassNeedleY;

    QcGaugeWidget *mCompassGaugeZ;
    QcNeedleItem *mCompassNeedleZ;

    QcGaugeWidget *mCompass;
    QcNeedleItem *mCompassNeedle;

    QSerialPort *m_serial = nullptr;

    VideoCapture cap;
    VideoWriter video; //Video Writer instance

    Mat frame;
    QImage qt_image;
    QImage qt_image_temp;

    DuJoystickManager *mJoyManager;

    int btnPressed;
    //bool recState;
    void manageJoystick(int button);
    void stopRecAction();

    bool startCam();
    bool stopCam();

    bool initVideo();
    bool saveVideo();

    void clearBtn();

    QImage Mat2QImage(cv::Mat const& src);
    cv::Mat QImage2Mat(QImage const& src);

    void showPortInfo(int idx);

    char charValue[10];

    int countRoll;
    int countYaw;
    int countPitch;

    rovStruct rovData;
    videoStruct videoData;
    camStruct camData[3];
};

#endif // WIDGET_H
