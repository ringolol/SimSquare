import QtQuick 2.12
import QtAndroidTools 1.0

QtAndroidAdMobBanner {
    width: 320
    height: 50
    objectName: "banner1"
    id: banner1
    unitId: "ca-app-pub-3940256099942544/6300978111"
    type: QtAndroidAdMobBanner.TYPE_BANNER
    onLoading: banner1state.text = "Loading"
    onLoaded: banner1state.text = "Loaded"
    onLoadError: banner1state.text = "Error " + errorId
}
