#include "gen-c_glib/hello_svc.h"

#include <thrift/c_glib/transport/thrift_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>

#include <glib-object.h>
#include <glib.h>

#include <stdio.h>

int
main (void)
{
  ThriftSocket *socket;
  ThriftTransport *transport;
  ThriftProtocol *protocol;

  helloSvcIf *client;

  gboolean success;
  gchar *message = NULL;
  GError *error = NULL;

  Work *work;
  work = g_object_new (TYPE_WORK, NULL);

#if (!GLIB_CHECK_VERSION (2, 36, 0))
  g_type_init ();
#endif

  socket    = g_object_new (THRIFT_TYPE_SOCKET,
                            "hostname",  "localhost",
                            "port",      9090,
                            NULL);
  transport = g_object_new (THRIFT_TYPE_BUFFERED_TRANSPORT,
                            "transport", socket,
                            NULL);
  protocol  = g_object_new (THRIFT_TYPE_BINARY_PROTOCOL,
                            "transport", transport,
                            NULL);

  client = g_object_new (TYPE_HELLO_SVC_CLIENT,
                         "input_protocol",  protocol,
                         "output_protocol", protocol,
                         NULL);

  success =
    thrift_transport_open (transport, &error) &&
    hello_svc_if_get_message (client, &message, "world!", &error);

  if (success) {
    puts (message);
    g_free (message);
  } else {
    fprintf (stderr, "Client caught an exception: %s\n", error->message);
    g_clear_error (&error);
  }

  hello_svc_if_get_output (client, &work, &error);
  if(!error){
    // Following the thrift way of fetching data
    // https://developer.gnome.org/gobject/stable/gobject-The-Base-Object-Type.html#g-object-get
    gchar *message_string;
    gint32 output;
    g_object_get (work, "message", &message_string, NULL);
    g_object_get (work, "output", &output, NULL);
    //printf("Output : %d\nMessage : %s\n", work->output, work->message);
    printf("Output : %d\nMessage : %s\n", output, message_string);
    g_object_unref (work);
  }

  thrift_transport_close (transport, &error);
  success = success && (error == NULL);

  g_clear_error (&error);

  g_object_unref (client);
  g_object_unref (protocol);
  g_object_unref (transport);
  g_object_unref (socket);

  return (success ? 0 : 1);
}
