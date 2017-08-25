import QtQuick 2.5
import QtMultimedia 5.5
import com.calibration.opencv 1.0

Item {

    property double gaussianBlurCoefValue
    property int gaussianBlurSizeValue
    property double cannyThresholdValue
    property int cannyKernelSizeValue

    Camera {
        id: camera

        viewfinder.resolution: "640x480"

        imageCapture  {
            resolution: "640x480"
        }
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        source: camera
        focus : visible
        orientation: 270
        filters: [videoFilter]


    }

    VideoFilter  {
        id: videoFilter
        gaussianBlurCoef: gaussianBlurCoefValue
        gaussianBlurSize: gaussianBlurSizeValue
        cannyThreshold: cannyThresholdValue
        cannyKernelSize: cannyKernelSizeValue
    }
}
