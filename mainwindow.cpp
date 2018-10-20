#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GetCameras(ui->menuDevices);
    WorkerInit();
    StartCap();
    StartTimer();
}

MainWindow::~MainWindow()
{
    delete ui;
    capture.release();
}

void MainWindow::FrameUpdate()
{
    if(capture.read(Src)){
        Worker();
    }
}

void MainWindow::StartCap()
{
    capture.open(0);
    if(!capture.isOpened()){
        return;
    }
    FrameUpdate();
}

void MainWindow::restartCap(int number)
{
    capture.release();
    capture.open(number);
    if(!capture.isOpened()){
        return;
    }
    FrameUpdate();
}

void MainWindow::StartTimer()
{
    tmrTimer = new QTimer(this);
    connect(tmrTimer,SIGNAL(timeout()),this,SLOT(FrameUpdate()));
    tmrTimer->start(LoopTime); //msec
}

void MainWindow::OutMat(Mat &ToOut)
{
    QImage qimgOut((uchar*) ToOut.data,ToOut.cols,ToOut.rows,ToOut.step,QImage::Format_RGB888);
    ui->OutLabel->setPixmap(QPixmap::fromImage(qimgOut));
}

void MainWindow::Worker()
{
    BackSubTractor->apply(Src,Subt);
    BinSubtToRGBCut(Src,Subt,Out);

    bitwise_or(Out,DrawArrRect(ContoursToArrRect(GetContoursFromFrame(Subt),420,3333)),Out);

    OutMat(Out);
}

void MainWindow::BinSubtToRGBCut(Mat& BGRSource,Mat& Subtracted, Mat& RGBOuted)
{
    cvtColor(Subtracted,RGBOuted,CV_GRAY2BGR);
    bitwise_and(BGRSource,RGBOuted,RGBOuted);
    cvtColor(RGBOuted,RGBOuted,CV_BGR2RGB);
}

void MainWindow::on_BackSubtSel_currentIndexChanged(int index)
{
    WorkerCode=index;
    WorkerInit();

}

void MainWindow::WorkerInit(){
    switch (WorkerCode) {
    case 0:
        BackSubTractor = createBackgroundSubtractorMOG(420,5,0.7,0);
        break;
    case 1:
        BackSubTractor = createBackgroundSubtractorMOG2(500,16,false);//много шума и "эффект памяти"
        break;
    case 2:
        BackSubTractor = createBackgroundSubtractorKNN(500,400,false);//немного хуже CNT
        break;
    case 3:
        BackSubTractor = createBackgroundSubtractorCNT(15,true,15*60,true);//самый лучший
        break;
    case 4:
        BackSubTractor = createBackgroundSubtractorGMG(120,0.8);//нестабильный
        break;
    default:
        break;
    }
}

void MainWindow::GetCameras(QMenu *Devices)
{
    QActionGroup *DevicesGroup = new QActionGroup(this);
    DevicesGroup->setExclusive(true);

    const QList<QCameraInfo> Cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &camInfo : Cameras)
    {
        QAction *videoDeviceAction = new QAction(camInfo.description(), DevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(camInfo));

        connect(videoDeviceAction, SIGNAL(triggered(bool)), this, SLOT(changeCamera()));

        QStringList cams=camInfo.deviceName().split("video");
        //CamerasMap[QVariant::fromValue(camInfo)] = QString(cams.last()).toInt();
        CamDevArr[camInfo.description()] = QString(cams.last()).toInt();

        if (camInfo == QCameraInfo::defaultCamera())
            videoDeviceAction->setChecked(true);

        Devices->addAction(videoDeviceAction);
    }

}

void MainWindow::changeCamera()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QString name = action->iconText();
    int number = CamDevArr[name];
    restartCap(number);
}

vector<vector<Point>> MainWindow::GetContoursFromFrame(Mat& InputBinMat)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    /// Find contours
    findContours(InputBinMat, contours, hierarchy, CV_RETR_TREE, 	CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    return contours;
}

vector<Rect> MainWindow::ContoursToArrRect(vector<vector<Point> > contours,int minLength,int maxLength)
{
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );

     for( uint i = 0; i < contours.size(); i++ ){
       approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       if((arcLength(contours[i],false)>=minLength)&&(arcLength(contours[i],false)<=maxLength))
         boundRect[i] = boundingRect( Mat(contours_poly[i]) );
     }

    return boundRect;
}

Mat MainWindow::DrawArrRect(vector<Rect> Rect)
{
        RNG rng(12345);
        Mat drawing = Mat::zeros(Src.size(), CV_8UC3 );
        for( uint i = 0; i< Rect.size(); i++ ){
           Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
           rectangle( drawing, Rect[i].tl(), Rect[i].br(), color, 2, 8, 0 );
         }

        return drawing;
}
