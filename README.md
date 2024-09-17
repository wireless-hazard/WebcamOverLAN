# WebcamOverLAN
Simple application to stream the webcam over the Local Network using GStreamer on Linux

## Building
To build the application, run: 
```
gcc main.cpp -o a.out `pkg-config --cflags --libs gstreamer-1.0`
```
