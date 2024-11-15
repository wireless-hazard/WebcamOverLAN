# WebcamOverLAN
Simple application to stream the webcam over the Local Network using GStreamer on Linux

## Building
To build the application, run: 
```
g++ main.cpp -o a.out `pkg-config --cflags --libs gstreamer-1.0`
```
## Run
Call the executable, passing as the command line argument the IP which will receive the stream
```
./a.out 127.0.0.1
```

## Sending and receiving over the network using UDP
The temporary pipeline using gst-launch to send and receive data over the network are:

**Send**:
```
gst-launch-1.0 -v v4l2src ! "video/x-raw,framerate=30/1" ! videoconvert ! rtpvrawpay ! udpsink host=127.0.0.1 port=5200
```

**Receive**:

*This will vary according with your webcam
```
gst-launch-1.0 udpsrc address=127.0.0.1 port=5200 caps = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)640, height=(string)480, colorimetry=(string)SMPTE240M, payload=(int)96" ! rtpvrawdepay ! videoconvert ! queue ! autovideosink
```

### Sending and receiving over the network using UDP with H264 enconding

**Send**
```
gst-launch-1.0 -v v4l2src ! videoconvert  ! "video/x-raw, framerate=30/1" ! openh264enc ! h264parse ! video/x-h264,stream-format=byte-stream ! udpsink host=127.0.0.1 port=5200
```

**Receive**
```
gst-launch-1.0 udpsrc address=127.0.0.1 port=5200 ! h264parse ! openh264dec ! videoconvert ! queue ! autovideosink
```