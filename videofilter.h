#ifndef VIDEOFILTER
#define VIDEOFILTER

#include <QAbstractVideoFilter>
#include <QOpenGLTexture>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/videoio.hpp"
#include <aruco/aruco.h>
#include <QDebug>


#include <QTimer>

using namespace cv;
using namespace std;
using namespace aruco;


class VideoFilter : public QAbstractVideoFilter
{
    Q_OBJECT

    Q_PROPERTY(int gaussianBlurSize READ gaussianBlurSize WRITE setGaussianBlurSize NOTIFY gaussianBlurSizeChanged)
    Q_PROPERTY(double gaussianBlurCoef READ gaussianBlurCoef WRITE setGaussianBlurCoef  NOTIFY gaussianBlurCoefChanged)
    Q_PROPERTY(int cannyKernelSize READ cannyKernelSize WRITE setCannyKernelSize NOTIFY cannyKernelSizeChanged)
    Q_PROPERTY(double cannyThreshold READ cannyThreshold WRITE setCannyThreshold  NOTIFY cannyThresholdChanged)


public:
    QVideoFilterRunnable *createFilterRunnable() Q_DECL_OVERRIDE;

    int rows = 1;
    int cols = 1;

    int gaussianBlurSize() const;
    void setGaussianBlurSize(int gaussianBlurSize);

    double gaussianBlurCoef() const;
    void setGaussianBlurCoef(double gaussianBlurCoef);

    double cannyThreshold() const;
    void setCannyThreshold(double cannyThreshold);

    int cannyKernelSize() const;
    void setCannyKernelSize(int cannyKernelSize);

private:
    int m_gaussianBlurSize;
    double m_gaussianBlurCoef;

    int m_cannyKernelSize;
    double m_cannyThreshold;


signals:
    void gaussianBlurSizeChanged();
    void gaussianBlurCoefChanged();
    void cannyKernelSizeChanged();
    void cannyThresholdChanged();
};



class FilterRunnable : public QVideoFilterRunnable
{

public:
    FilterRunnable(VideoFilter *filter);
    ~FilterRunnable();

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) Q_DECL_OVERRIDE;

private:
    VideoFilter * m_filter;
    QOpenGLTexture *texture;
    MarkerDetector * markerDetector;
    std::vector< Marker > detectedMarkers;
    CameraParameters *cameraParameters;

    QTimer *sceneTimer;

    int countDetectedFaces;
    CascadeClassifier frontalFaceClassifier;
    CascadeClassifier smileClassifier;

    Rect actualFace;
    Rect calibration;


    Mat cameraTexture;
    VideoCapture *videoCapture;

    void deleteColorComponentFromYUV(QVideoFrame *input);

    cv::Mat yuvFrameToMat8(QVideoFrame *frame);
};

#endif // VIDEOFILTER

