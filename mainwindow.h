#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCamera>
#include <QCameraInfo>
#include <QVariant>
#include <QActionGroup>
#include <QWidgetAction>
#include <QLineEdit>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Q_DECLARE_METATYPE(QCameraInfo)

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTimer *tmrTimer;

    Ui::MainWindow *ui;
    VideoCapture capture;
    Mat Src;
    Mat Out;
    Mat Subt;
    Ptr<BackgroundSubtractor> BackSubTractor;

    QMap<QString, int> CamDevArr;

    int LoopTime=50;
    int WorkerCode=1;
    int contoursCount=0;

    void StartCap();
    void RestartCap(int);
    void RestartCap(QString);
    void StartTimer();
    void GetCameras(QMenu*);
    void Worker();
    void WorkerInit();
    void OutMat(Mat&);
    void BinSubtToRGBCut(Mat&,Mat&,Mat&);
    void GenMenuStream(QMenu*);

    vector<vector<Point>> GetContoursFromFrame(Mat&);
    vector<Rect> ContoursToArrRect(vector<vector<Point> >,int,int);
    Mat DrawArrRect(vector<Rect>);

signals:
    void timeout();

private slots:
    void FrameUpdate();
    void on_BackSubtSel_currentIndexChanged(int index);
    void ChangeCamera();
    void ChangeURL(QLineEdit*);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
