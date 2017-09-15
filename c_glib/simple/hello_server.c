#include "gen-c_glib/hello_svc.h"

#include <thrift/c_glib/server/thrift_simple_server.h>
#include <thrift/c_glib/transport/thrift_server_socket.h>
#include <thrift/c_glib/transport/thrift_buffered_transport_factory.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol_factory.h>
#include <thrift/c_glib/protocol/thrift_protocol_factory.h>
#include <thrift/c_glib/transport/thrift_server_transport.h>
#include <thrift/c_glib/thrift.h>

#include <glib-object.h>
#include <glib.h>

#include <stdio.h>

G_BEGIN_DECLS

#define TYPE_EXAMPLE_HELLO_SVC_HANDLER                          \
  (example_hello_svc_handler_get_type ())
#define EXAMPLE_HELLO_SVC_HANDLER(obj)                          \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                           \
                               TYPE_EXAMPLE_HELLO_SVC_HANDLER,  \
                               ExampleHelloSvcHandler))
#define EXAMPLE_HELLO_SVC_HANDLER_CLASS(c)                      \
  (G_TYPE_CHECK_CLASS_CAST ((c),                                \
                            TYPE_EXAMPLE_HELLO_SVC_HANDLER,     \
                            ExampleHelloSvcHandlerClass))
#define IS_EXAMPLE_HELLO_SVC_HANDLER(obj)                       \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                           \
                               TYPE_EXAMPLE_HELLO_SVC_HANDLER))
#define IS_EXAMPLE_HELLO_SVC_HANDLER_CLASS(c)                   \
  (G_TYPE_CHECK_CLASS_TYPE ((c),                                \
                            TYPE_EXAMPLE_HELLO_SVC_HANDLER))
#define EXAMPLE_HELLO_SVC_HANDLER_GET_CLASS(obj)                \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                            \
                              TYPE_EXAMPLE_HELLO_SVC_HANDLER,   \
                              ExampleHelloSvcHandlerClass))

struct _ExampleHelloSvcHandler {
  helloSvcHandler parent_instance;
};
typedef struct _ExampleHelloSvcHandler ExampleHelloSvcHandler;

struct _ExampleHelloSvcHandlerClass {
  helloSvcHandlerClass parent_class;
};
typedef struct _ExampleHelloSvcHandlerClass ExampleHelloSvcHandlerClass;

GType example_hello_svc_handler_get_type (void);

G_END_DECLS

G_DEFINE_TYPE (ExampleHelloSvcHandler,
               example_hello_svc_handler,
               TYPE_HELLO_SVC_HANDLER)

static gboolean
example_hello_svc_handler_get_message (helloSvcIf   *iface,
                                       gchar       **_return,
                                       const gchar  *name,
                                       GError      **error)
{
  THRIFT_UNUSED_VAR (iface);
  THRIFT_UNUSED_VAR (error);

  printf ("Server received: %s, from client\n", name);

  *_return = g_strconcat ("Hello ", name, NULL);

  return TRUE;
}

static gboolean
example_hello_svc_handler_get_output (helloSvcIf   *iface,
                                       Work     **_return,
                                       GError      **error)
{
  THRIFT_UNUSED_VAR (iface);
  THRIFT_UNUSED_VAR (error);
  
  Work *work;
  /* Thrift structs are implemented as GObjects, with each of the
     struct's members exposed as an object property. */
  work = g_object_new (TYPE_WORK, NULL);
  g_object_set (work,
                  "output", 8361,
                  "message", "Job successfully executed",
                  NULL);
  printf ("Server received a message to work and give output\n");
  *_return = work;

  return TRUE;
}

static void
example_hello_svc_handler_init (ExampleHelloSvcHandler *self)
{
}

static void
example_hello_svc_handler_class_init (ExampleHelloSvcHandlerClass *klass)
{
  helloSvcHandlerClass *hello_svc_handler_class =
    HELLO_SVC_HANDLER_CLASS (klass);

  hello_svc_handler_class->get_message = example_hello_svc_handler_get_message;
  hello_svc_handler_class->get_output = example_hello_svc_handler_get_output;
}

int
main (void)
{
  helloSvcHandler *handler;
  helloSvcProcessor *processor;

  ThriftServerTransport *server_transport;
  ThriftTransportFactory *transport_factory;
  ThriftProtocolFactory *protocol_factory;
  ThriftServer *server;

  GError *error = NULL;

#if (!GLIB_CHECK_VERSION (2, 36, 0))
  g_type_init ();
#endif

  handler   = g_object_new (TYPE_EXAMPLE_HELLO_SVC_HANDLER,
                            NULL);
  processor = g_object_new (TYPE_HELLO_SVC_PROCESSOR,
                            "handler", handler,
                            NULL);

  server_transport  = g_object_new (THRIFT_TYPE_SERVER_SOCKET,
                                    "port", 9090,
                                    NULL);
  transport_factory = g_object_new (THRIFT_TYPE_BUFFERED_TRANSPORT_FACTORY,
                                    NULL);
  protocol_factory  = g_object_new (THRIFT_TYPE_BINARY_PROTOCOL_FACTORY,
                                    NULL);

  server = g_object_new (THRIFT_TYPE_SIMPLE_SERVER,
                         "processor",                processor,
                         "server_transport",         server_transport,
                         "input_transport_factory",  transport_factory,
                         "output_transport_factory", transport_factory,
                         "input_protocol_factory",   protocol_factory,
                         "output_protocol_factory",  protocol_factory,
                         NULL);

  thrift_server_serve (server, &error);

  g_object_unref (server);
  g_object_unref (protocol_factory);
  g_object_unref (transport_factory);
  g_object_unref (server_transport);

  g_object_unref (processor);
  g_object_unref (handler);

  return 0;
}
