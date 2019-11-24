// based on https://developer.chrome.com/apps/serial
// chrome app to send input from RS232 to web page

const DEVICE_PATH = 'COM8';
const serial = chrome.serial;
var dataRecieved = "";

/* Interprets an ArrayBuffer as UTF-8 encoded string data. */
var ab2str = function (buf) {
    var bufView = new Uint8Array(buf);
    var encodedString = String.fromCharCode.apply(null, bufView);
    return decodeURIComponent(escape(encodedString));
};

/* Converts a string to UTF-8 encoding in a Uint8Array; returns the array buffer. */
var str2ab = function (str) {
    var encodedString = unescape(encodeURIComponent(str));
    var bytes = new Uint8Array(encodedString.length);
    for (var i = 0; i < encodedString.length; ++i) {
        bytes[i] = encodedString.charCodeAt(i);
    }
    return bytes.buffer;
};

var lastLength = 0;
var timer = null;

var SerialConnection = function () {
    this.connectionId = -1;
    this.lineBuffer = "";
    this.boundOnReceive = this.onReceive.bind(this);
    this.boundOnReceiveError = this.onReceiveError.bind(this);
};

var cPort = null;

SerialConnection.prototype.onConnectComplete = function (connectionInfo) {
    if (!connectionInfo) {
        console.log("Connection failed.");
        return;
    }
    console.log("Connection successful.");

    this.connectionId = connectionInfo.connectionId;
    chrome.serial.onReceive.addListener(this.boundOnReceive);
    chrome.serial.onReceiveError.addListener(this.boundOnReceiveError);

    chrome.runtime.onConnectExternal.addListener(function (port) {
        cPort = port;
        port.onMessage.addListener(function (msg) {
            // message from webpage
            console.log(msg);
        });
    });
};

SerialConnection.prototype.onReceive = function (receiveInfo) {
    if (receiveInfo.connectionId !== this.connectionId) {
        return;
    }

    this.lineBuffer += ab2str(receiveInfo.data);

    var index;
    while ((index = this.lineBuffer.indexOf('\n')) >= 0) {
        var line = this.lineBuffer.substr(0, index + 1);
        this.onReadLine.dispatch(line);
        this.lineBuffer = this.lineBuffer.substr(index + 1);
    }

    var cnt = this;
    var output = function () {
        if (lastLength === 0 && cnt.lineBuffer.length === 0) return;

        if (lastLength !== cnt.lineBuffer.length) {
            lastLength = cnt.lineBuffer.length;
            timer = setTimeout(output, 500);
            return;
        }

        console.log(cnt.lineBuffer);

        if (cPort)
            cPort.postMessage({ data: cnt.lineBuffer });

        lastLength = 0;
        cnt.lineBuffer = '';
    };

    clearTimeout(timer);
    timer = setTimeout(output, 500);
};

SerialConnection.prototype.onReceiveError = function (errorInfo) {
    if (errorInfo.connectionId === this.connectionId) {
        this.onError.dispatch(errorInfo.error);
    }
};

SerialConnection.prototype.connect = function (path) {
    serial.connect(path, this.onConnectComplete.bind(this));
    console.log("Connect to " + path);
};

SerialConnection.prototype.send = function (msg) {
    if (this.connectionId < 0) {
        throw 'Invalid connection';
    }
    serial.send(this.connectionId, str2ab(msg), function () { });
};

SerialConnection.prototype.disconnect = function () {
    if (this.connectionId < 0) {
        throw 'Invalid connection';
    }
    serial.disconnect(this.connectionId, function () { });
};

var connection = new SerialConnection();
connection.connect(DEVICE_PATH);