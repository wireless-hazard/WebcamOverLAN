#include <gst/gst.h>
#include <string>

int main (int argc, char *argv[])
{
    /* Takes the first argument to be used as the target IP to send the webcam stream*/
    std::string target_ip{argv[1]};

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Create the elements */
    GstElement *source = gst_element_factory_make("v4l2src", nullptr);
    GstElement *sink = gst_element_factory_make("udpsink", "sink");
    GstElement *rtpvrawpay_element = gst_element_factory_make("rtpvrawpay", nullptr);
    GstElement *video_convert = gst_element_factory_make("videoconvert", "video_convert");
    /* Video scale it waaay down to achieve "Real-time" transmission*/
    GstElement *video_scale = gst_element_factory_make("videoscale", nullptr);

    /* Sets the IP:PORT to be used by the UDP socket*/
    g_object_set(sink, "host", target_ip.c_str(), "port", 5200, nullptr);

    /* Create the empty pipeline */
    GstElement *pipeline = gst_pipeline_new("simple-pipeline");

    if (!pipeline || !source || !sink || !video_convert || !video_scale || !rtpvrawpay_element) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN (pipeline), source, sink, video_convert, video_scale, rtpvrawpay_element, nullptr);

    /* Limits the webcam fps to 30 */
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
        "framerate", GST_TYPE_FRACTION, 30, 1,
        nullptr);
    gboolean link_ok = gst_element_link_filtered (source, video_convert, caps);
    gst_caps_unref (caps);

    if (!link_ok) {
        g_error ("Failed to link element1 and element2!");
    }

    /* resolution for the videoscale */
    GstCaps *caps_convert = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, 240,
        "height", G_TYPE_INT, 240,
        nullptr);

    /* links videoscale to rtpvrawpay using the caps above */
    link_ok = gst_element_link_filtered(video_scale, rtpvrawpay_element, caps_convert);
    gst_caps_unref(caps_convert);

    if (!link_ok) {
        g_error ("Failed to link element1 and element2!");
    }

    /* links videoconvert to videoscale */
    link_ok = gst_element_link(video_convert, video_scale);

    if (!link_ok) {
        g_error ("Failed to link element1 and element2!");
    }

    /* Missing link in the pipeline is connecting the rtpvrawpay to the sink*/
    /* Link all elements that can be automatically linked because they have "Always" pads */
    if (gst_element_link_many(rtpvrawpay_element, sink, nullptr) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg =
        gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        g_printerr ("An error occurred! Re-run with the GST_DEBUG=*:WARN "
        "environment variable set for more details.\n");
    }

    /* Free resources */
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}
