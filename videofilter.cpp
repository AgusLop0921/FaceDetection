#include <videofilter.h>
#include "rgbframehelper.h"
#include <QDebug>
#include <QResource>
#include <QFile>

QVideoFilterRunnable *VideoFilter::createFilterRunnable()
{    
    return new FilterRunnable(this);
}

int VideoFilter::gaussianBlurSize() const
{
    return m_gaussianBlurSize;
}

void VideoFilter::setGaussianBlurSize(int gaussianBlurSize)
{
    m_gaussianBlurSize = gaussianBlurSize %2 == 1 ? gaussianBlurSize : m_gaussianBlurSize;
}

double VideoFilter::gaussianBlurCoef() const
{    
    return m_gaussianBlurCoef;
}

void VideoFilter::setGaussianBlurCoef(double gaussianBlurCoef)
{    
    m_gaussianBlurCoef = gaussianBlurCoef;
}

double VideoFilter::cannyThreshold() const
{      
    return m_cannyThreshold;
}

void VideoFilter::setCannyThreshold(double cannyThreshold)
{    
    m_cannyThreshold = cannyThreshold;
}

int VideoFilter::cannyKernelSize() const
{   
    return m_cannyKernelSize;
}

void VideoFilter::setCannyKernelSize(int cannyKernelSize)
{    
    m_cannyKernelSize = cannyKernelSize%2 == 1 ? cannyKernelSize : m_cannyKernelSize;
}

FilterRunnable::FilterRunnable(VideoFilter *filter) :
    m_filter(filter), markerDetector( new MarkerDetector ), cameraParameters( new CameraParameters )
{

    #define FRONTAL_FACE_FILE_RESOURCE ":/FrontalFace.xml"
    #define FRONTAL_FACE_FILE_LOCAL "./FrontalFace.xml"

    QResource xml( FRONTAL_FACE_FILE_RESOURCE );

    QFile xmlFileResource(xml.absoluteFilePath());

    if (!xmlFileResource.open(QIODevice::ReadOnly | QIODevice::Text))  {
        qDebug() << "No se pudo iniciar camara 2 / Problema con parametros de la camara";
    }

    countDetectedFaces = 0;
    QTextStream in(&xmlFileResource);
    QString contentXML = in.readAll();

        // Creo un archivo nuevo para almacenarlo
    QFile xmlFileLocal(FRONTAL_FACE_FILE_LOCAL);
    if (!xmlFileLocal.open(QIODevice::WriteOnly | QIODevice::Text))  {
        qDebug() << "No se pudo iniciar camara / Problema con parametros de la camara";
    }

    QTextStream outXML(&xmlFileLocal);
    outXML << contentXML;

    xmlFileLocal.close();

    if(frontalFaceClassifier.load( "./FrontalFace.xml" )){
            qDebug()<<"-----FrontalFace Cargado correctamente-------";
    }
    else qDebug()<<"-----FrontalFace no se pudo cargar-------";

    if(frontalFaceClassifier.empty()){
        qDebug()<<".............cascadeClassifier empty..................";
    }
    else
    {
               qDebug()<<".............No esta vacio cascadeClassifier..................";
    }

    texture = new QOpenGLTexture( QOpenGLTexture::Target2D );
    texture->setMinificationFilter( QOpenGLTexture::Nearest );
    texture->setMagnificationFilter( QOpenGLTexture::Linear );
    texture->setFormat( QOpenGLTexture::RGBA8_UNorm );

    #define CAMERA_PARAMETERS_FILE_RESOURCE ":/CameraParameters.yml"
    #define CAMERA_PARAMETERS_FILE_LOCAL "./CameraParameters.yml"

    QResource yml( CAMERA_PARAMETERS_FILE_RESOURCE );

    QFile ymlFileResource(yml.absoluteFilePath());

    if (!ymlFileResource.open(QIODevice::ReadOnly | QIODevice::Text))  {
        qDebug() << "No se pudo iniciar camara 2 / Problema con parametros de la camara";
    }

    QTextStream inXML(&ymlFileResource);
    QString content = inXML.readAll();

        // Creo un archivo nuevo para almacenarlo
    QFile ymlFileLocal(CAMERA_PARAMETERS_FILE_LOCAL);
    if (!ymlFileLocal.open(QIODevice::WriteOnly | QIODevice::Text))  {
        qDebug() << "No se pudo iniciar camara / Problema con parametros de la camara";
    }

    QTextStream out(&ymlFileLocal);
    out << content;

    ymlFileLocal.close();

    cameraParameters->readFromXMLFile( CAMERA_PARAMETERS_FILE_LOCAL );


    if ( ! cameraParameters->isValid() )  {
        qDebug() << "Error con YML / No es valido. La App se cerrara";
    }
}

FilterRunnable::~FilterRunnable()
{
    qDebug()<<"{destructor FilterRunnable}";

}

QVideoFrame FilterRunnable::run( QVideoFrame *input,
                                 const QVideoSurfaceFormat &surfaceFormat,
                                 QVideoFilterRunnable::RunFlags flags )
{


    // La valores por defecto son estos.
    int gaussianBlurSize = 7;
    double gaussianBlurCoef = 1.5f;
    int cannyKernelSize = 3;
    double cannyThreshold = 0;



    if ( ! input->isValid() )
        return *input;

    if (input->handleType() == QAbstractVideoBuffer::NoHandle) {
//        input->map(QAbstractVideoBuffer::ReadOnly);
//        cv::Mat mat( input->height(), input->width(), CV_8UC1, input->bits() );

//        markerDetector->detect( mat, detectedMarkers, *cameraParameters, 0.57f );

//        for( unsigned int i = 0; i < detectedMarkers.size(); i++ )
//            detectedMarkers.at( i ).draw( mat, Scalar( 255, 0, 255 ), 1 );

//        QVector<QRgb> colorTable;
//        for (int i = 0; i < 256; i++)
//        {
//            colorTable.push_back(qRgb(i, i, i));
//        }
//        const uchar *qImageBuffer = (const uchar*) mat.data;
//        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
//        img.setColorTable(colorTable);
//        input->unmap();
//        QVideoFrame outputFrame = QVideoFrame(img);
//        return outputFrame;
    } else {

        input->map(QAbstractVideoBuffer::ReadOnly);
        QImage image = imageWrapper(*input);

//        QImage imageScaled = image.scaled(640,480,Qt::IgnoreAspectRatio);

//        qDebug()<<"-.-.-.-.-.-Tamaño QImage :  "<<image.size();

        cv::Mat mat(image.width(),image.height(),CV_8UC3,image.bits(), image.bytesPerLine());


        cv::Mat dst;
        cv::resize(mat, dst, cv::Size(320,240));
        int rows = dst.rows;
        int cols = dst.cols;

        cv::Size s = dst.size();
        rows = s.height;
        cols = s.width;

//        resize(mat, dst, Size(1024, 768), 0, 0, INTER_CUBIC);

        qDebug()<<"-.-.-.-.-.-Tamaño mat :  "<<rows;

        qDebug()<<"-.-.-.-.-.-Tamaño mat dst :  "<<cols;


        vector< Rect > detectedFaces;
        detectedFaces.clear();
        frontalFaceClassifier.detectMultiScale( dst, detectedFaces,
                                         1.05, 2, 0 | CASCADE_SCALE_IMAGE, Size(60,60) );


        if( detectedFaces.size() > 0 ){
            actualFace = detectedFaces.at( 0 );
            countDetectedFaces++;
            qDebug()<<"**************************qwerty*************************"<<detectedFaces.size()<<"------------------------";
            Point pt1;
            pt1.x = 0;
            pt1.y = 0;
            Point pt2;
            pt2.x = 700;
            pt2.y = 700;
            cv::line(mat, pt1, pt2, cv::Scalar(0,255,0), 20);
        }
        input->unmap();
        texture->setData(image);
        texture->bind();
        qDebug()<<"Cuenta de caras detectadasssssss : "<<countDetectedFaces<<"-------------------------------";

        return frameFromTexture(texture->textureId(),input->size(),input->pixelFormat());
    }
}
void FilterRunnable::deleteColorComponentFromYUV( QVideoFrame *input )
{
    // Assign 0 to Us and Vs
    int firstU = input->width() * input->height();
    int lastV = input->width() * input->height() + 2 * input->width() * input->height() / 4;
    uchar* inputBits = input->bits();

    for ( int i = firstU ; i < lastV ; i++ )
        inputBits[i] = 127;    
}

// Metodo extraido de https://github.com/alpqr/qt-opencv-demo/blob/master/opencvhelper.cpp
cv::Mat FilterRunnable::yuvFrameToMat8( QVideoFrame * frame)
{    

    Q_ASSERT(frame->handleType() == QAbstractVideoBuffer::NoHandle && frame->isReadable());
    Q_ASSERT(frame->pixelFormat() == QVideoFrame::Format_YUV420P || frame->pixelFormat() == QVideoFrame::Format_NV12);

    cv::Mat tmp(frame->height() + frame->height() / 2, frame->width(), CV_8UC1, (uchar *) frame->bits());
    cv::Mat result(frame->height(), frame->width(), CV_8UC3);
    cvtColor(tmp, result, frame->pixelFormat() == QVideoFrame::Format_YUV420P ? CV_YUV2BGR_YV12 : CV_YUV2BGR_NV12);
    return result;
}

