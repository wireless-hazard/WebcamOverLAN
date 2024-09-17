#include <gst/gst.h>

int main (int argc, char *argv[])
{
    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Create the elements */
    GstElement *source = gst_element_factory_make ("v4l2src", "source");
    GstElement *sink = gst_element_factory_make ("autovideosink", "sink");
    GstElement *video_convert = gst_element_factory_make ("videoconvert", "video_convert");

    /* Create the empty pipeline */
    GstElement *pipeline = gst_pipeline_new ("simple-pipeline");

    if (!pipeline || !source || !sink || !video_convert) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many (GST_BIN (pipeline), source, sink, video_convert, nullptr);

    GstCaps *caps = gst_caps_new_simple ("video/x-raw",
        "framerate", GST_TYPE_FRACTION, 30, 1,
        nullptr);

    gboolean link_ok = gst_element_link_filtered (source, video_convert, caps);
    gst_caps_unref (caps);

    if (!link_ok) {
        g_error ("Failed to link element1 and element2!");
    }

    /* Link all elements that can be automatically linked because they have "Always" pads */
    if (gst_element_link_many (video_convert, sink, nullptr) != TRUE) {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;
    }

    /* Start playing */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    GstBus *bus = gst_element_get_bus (pipeline);
    GstMessage *msg =
        gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
        g_printerr ("An error occurred! Re-run with the GST_DEBUG=*:WARN "
        "environment variable set for more details.\n");
    }

    /* Free resources */
    gst_message_unref (msg);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
}