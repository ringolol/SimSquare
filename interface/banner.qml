import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0
    width: 320
    height: 50
    Column {
        objectName: "col"
        width: parent.wdith
        height: parent.height
        anchors.centerIn: parent
        spacing: 0

        Rectangle {
            objectName: "rect"
            anchors.horizontalCenter: parent.horizontalCenter
            border.width: 1
            border.color: "black"
            width: banner1.width
            height: banner1.height

            QtAndroidAdMobBanner {
                objectName: "banner1"
                id: banner1
                unitId: "ca-app-pub-3940256099942544/6300978111"
                type: QtAndroidAdMobBanner.TYPE_BANNER
                onLoading: banner1state.text = "Loading"
                onLoaded: banner1state.text = "Loaded"
                onLoadError: banner1state.text = "Error " + errorId
            }
        }
    }
}
