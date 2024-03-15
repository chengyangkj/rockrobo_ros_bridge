
#include "mongoose.h"

static const char *s_listen_on = "http://0.0.0.0:8000";
static const char *s_web_directory = "./html";

static const char *s_url =
    "http://127.0.0.1/api/v2/robot/capabilities/BasicControlCapability";
static const char *s_start_clear_data = "{\"action\" : \"start\"}";
static const char *s_stop_clear_data = "{\"action\" : \"stop\"}";
static const char *s_go_home_data = "{\"action\" : \"home\"}";

bool StartRobotControlProcess() {
  int result = system("nohup /opt/rockrobo/cleaner/bin/RoboController&");
  return result != -1;
}
bool StopRobotControlProcess() {
  int result =
      system("kill -9 $(ps -aux | grep 'RoboController' | awk '{print $2}')");
  return result != -1;
}

// Print HTTP response and signal that we're done
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    printf("%.*s", (int)hm->message.len, hm->message.ptr);
    c->is_closing = 1;
    *(bool *)fn_data = true;
  }
}
void sleep_ms(int milliseconds) {
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;

  nanosleep(&ts, NULL);
}

int StartClear() {
  struct mg_mgr mgr; // Event manager
  mg_mgr_init(&mgr); // Initialise event manager

  bool done = false; // Event handler flips it to true
  struct mg_str host = mg_url_host(s_url);
  int content_length = strlen(s_start_clear_data);
  struct mg_connection *c = mg_http_connect(&mgr, s_url, fn, &done);
  if (c != NULL) {
    printf("send msg:\n");
    mg_printf(c,
              "PUT %s HTTP/1.1\r\n"
              "Host: %.*s\r\n"
              "Content-Type: application/json\r\n"
              "Content-Length: %d\r\n"
              "\r\n",
              mg_url_uri(s_url), (int)host.len, host.ptr, content_length);
    mg_send(c, s_start_clear_data, content_length);
  }
  while (done == false)
    mg_mgr_poll(&mgr, 1000); // Event loop
  mg_mgr_free(&mgr);

  return 0;
}

int StopClear() {
  struct mg_mgr mgr; // Event manager
  mg_mgr_init(&mgr); // Initialise event manager

  bool done = false; // Event handler flips it to true
  struct mg_str host = mg_url_host(s_url);
  int content_length = strlen(s_stop_clear_data);
  struct mg_connection *c = mg_http_connect(&mgr, s_url, fn, &done);
  if (c != NULL) {
    printf("send msg:\n");
    mg_printf(c,
              "PUT %s HTTP/1.1\r\n"
              "Host: %.*s\r\n"
              "Content-Type: application/json\r\n"
              "Content-Length: %d\r\n"
              "\r\n",
              mg_url_uri(s_url), (int)host.len, host.ptr, content_length);
    mg_send(c, s_stop_clear_data, content_length);
  }
  while (done == false)
    mg_mgr_poll(&mgr, 1000); // Event loop
  mg_mgr_free(&mgr);

  return 0;
}
int GoHome() {
  struct mg_mgr mgr; // Event manager
  mg_mgr_init(&mgr); // Initialise event manager

  bool done = false; // Event handler flips it to true
  struct mg_str host = mg_url_host(s_url);
  int content_length = strlen(s_go_home_data);
  struct mg_connection *c = mg_http_connect(&mgr, s_url, fn, &done);
  if (c != NULL) {
    printf("send msg:\n");
    mg_printf(c,
              "PUT %s HTTP/1.1\r\n"
              "Host: %.*s\r\n"
              "Content-Type: application/json\r\n"
              "Content-Length: %d\r\n"
              "\r\n",
              mg_url_uri(s_url), (int)host.len, host.ptr, content_length);
    mg_send(c, s_go_home_data, content_length);
  }
  while (done == false)
    mg_mgr_poll(&mgr, 1000); // Event loop
  mg_mgr_free(&mgr);

  return 0;
}

static void fn_server(struct mg_connection *c, int ev, void *ev_data,
                      void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    if (mg_http_match_uri(hm, "/robot/start")) {
      //在回到home点之前 把进程杀掉
      StopRobotControlProcess();
      StartRobotControlProcess();
      sleep(1);
      StartClear();
      sleep(20);
      StopClear();
      GoHome();
      sleep(3);
      StopRobotControlProcess();
      mg_http_reply(c, 200, "{\"result\": %d,\"code\":%s}\n", 0,
                    "start success test!");
    } else if (mg_http_match_uri(hm, "/robot/stop")) {
      StopRobotControlProcess();
      StartRobotControlProcess();
      sleep_ms(100);
      StopClear();
      GoHome();
      mg_http_reply(c, 200, "{\"result\": %d,\"code\":%s}\n", 0,
                    "stop success!");
    } else {
      mg_http_serve_dir(c, ev_data, s_web_directory); // Serve static files
    }
  }
  (void)fn_data;
}

int main(void) {
  struct mg_mgr mgr; // Event manager
  mg_mgr_init(&mgr); // Initialise event manager
  mg_http_listen(&mgr, s_listen_on, fn_server, NULL); // Create HTTP listener
  for (;;)
    mg_mgr_poll(&mgr, 1000); // Infinite event loop
  mg_mgr_free(&mgr);
  return 0;
}
