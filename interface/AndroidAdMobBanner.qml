
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    Column {
        objectName: "col"
        width: parent.wdith
        height: parent.height * 0.8
        anchors.centerIn: parent
        spacing: 20

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 15
            text: "Banner"
        }
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
        Label {
            id: banner1state
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "Banner not loaded"
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 15
            text: "Smart Banner"
        }
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            border.width: 1
            border.color: "black"
            width: banner2.width
            height: banner2.height

            QtAndroidAdMobBanner {
                id: banner2
                unitId: "ca-app-pub-3940256099942544/6300978111"
                type: QtAndroidAdMobBanner.TYPE_SMART_BANNER
                onLoading: banner2state.text = "Loading"
                onLoaded: banner2state.text = "Loaded"
                onLoadError: banner2state.text = "Error " + errorId
            }
        }
        Label {
            id: banner2state
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "Banner not loaded"
        }

        Button {
            objectName: "Button1"
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show banners"
            onClicked: {
                banner1.show();
                banner2.show();
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Hide banners"
            onClicked: {
                banner1.hide();
                banner2.hide();
            }
        }
    }
}
