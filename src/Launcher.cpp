#define _CRT_SECURE_NO_WARNINGS
#include <AppCore/JSHelpers.h>
#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

#include "Launcher.h"
#include <iostream>
#include <string>
#include <sstream>
#include <malloc.h>
#include "json/json.h"
#include <json/value.h>
#include <fstream>

#include <cstring>
#include "curl/curl.h"
#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 800

using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;

using namespace std;
#include <stdio.h>
#include <stdlib.h>

Launcher::Launcher() {
  app_ = App::Create();

  window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
    false, kWindowFlags_Titled | kWindowFlags_Resizable);

  window_->SetTitle("Launcher v0.1.0");

  app_->set_window(*window_.get());

  // Create the overlay
  overlay_ = Overlay::Create(*window_.get(), 1, 1, 0, 0);

  // If resized, update window's width and height
  OnResize(window_->width(), window_->height());

  // Set the file as the first page
  overlay_->view()->LoadURL("file:///app.html");

  app_->set_listener(this);

  window_->set_listener(this);

  overlay_->view()->set_load_listener(this);
}

Launcher::~Launcher() {
}

void Launcher::Run() {
  // Run the application
  app_->Run();
}

// Convert an Ultralight string to a UTF8 string.
inline std::string convert_to_utf8(const ultralight::String& str) {
  // Get utf8 string
  ultralight::String8 utf8 = str.utf8();
  return std::string(utf8.data(), utf8.length());
}

// Check if a file exists on the file system.
bool file_exists (const std::string& filename) {
  // check if file exists on the filesystem or not by opening for reading.
  if (FILE *fp = fopen(filename.c_str(), "r")) {
    // file exists
    fclose(fp);
    return true;
  } 
  // file does not exist, so return false.
  return false; 
}

// Check if a user is logged-into their Deltadex account.
bool is_logged_in()
{   
  char filename[FILENAME_MAX] = "response.json";

  // create a stream
  std::ifstream file(filename);

  // if file contains a response JSON object, user is logged in. 
  // if file is empty, user not logged in. 
  if (file_exists(filename)) {
    if (file.peek() == std::ifstream::traits_type::eof() == true) {
      return false;
    }
    // file contains a response object
    return true;
  }

  // file does not exist
  return false;
}

// libcurl callback to write data to the filesystem (write response from API)
static size_t write_data_to_filesystem(void *pointer, size_t size, size_t nmemb, void *stream)
{
  size_t is_written_to_filesystem = fwrite(pointer, size, nmemb, (FILE *)stream);
  return is_written_to_filesystem;
}

// Verify if a user is authenticated, for JS interop.
JSValue Launcher::VerifyAuth(const JSObject& obj, const JSArgs& args) {
  bool is_user_logged_in = is_logged_in();
  return JSValue(is_user_logged_in);
}

// Login to Deltadex account.
void Launcher::Login(const JSObject& obj, const JSArgs& args) {
  CURL *curl;
  CURLcode res;
  FILE *file;

  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://185.49.60.100/auth/login");
    String username_string = args[0];
    String password_string = args[1];

    std::string username = convert_to_utf8(username_string);
    std::string password = convert_to_utf8(password_string);

    char * username_cstr = new char [username.length()+1];
    std::strcpy (username_cstr, username.c_str());

    char * password_cstr = new char [password.length()+1];
    std::strcpy (password_cstr, password.c_str());
    
    long response_code;
    // Create JSON data
    char json_str[80];
    strcpy (json_str,"{\"username\" : \"");
    strcat (json_str, username_cstr);
    strcat (json_str, "\", \"password\" : \"");
    strcat (json_str, password_cstr);
    strcat (json_str, "\"}");

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);

    char output_filename[FILENAME_MAX] = "response.json";
  
    file = fopen(output_filename,"w");

    if (file) {
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_filesystem);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    }

    // Set headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    // Set content type 
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Set utf8
    headers = curl_slist_append(headers, "charsets: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

    // Perform request
    res = curl_easy_perform(curl);

    if(res == CURLE_OK) {

      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      if (response_code == 200) {
        overlay_->view()->LoadURL("file:///login-successful.html");
      } 
    }
    fclose(file);
    curl_easy_cleanup(curl);

    if (response_code != 200) {
      // clear the file; opening file for writing will clear it of its contents.
      // if a file is cleared, user is not logged in.
      fopen(output_filename, "w");
      overlay_->view()->LoadURL("file:///login-error.html");
    } 
  }
  curl_global_cleanup();
}

// Sign up for a Deltadex account.
void Launcher::SignUp(const JSObject& obj, const JSArgs& args) {

  CURL *curl;
  CURLcode res;
 
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {

    curl_easy_setopt(curl, CURLOPT_URL, "http://185.49.60.100/auth/register");
    String username_string = args[0];
    String password_string = args[1];

    std::string username = convert_to_utf8(username_string);
    std::string password = convert_to_utf8(password_string);

    // Convert to c string
    char * username_cstr = new char [username.length()+1];
    std::strcpy (username_cstr, username.c_str());

    // Convert to c string
    char * password_cstr = new char [password.length()+1];
    std::strcpy (password_cstr, password.c_str());

    // Create JSON data
    char json_str[80];
    strcpy (json_str,"{\"username\" : \"");
    strcat (json_str, username_cstr);
    strcat (json_str, "\", \"password\" : \"");
    strcat (json_str, password_cstr);
    strcat (json_str, "\"}");

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);

    // Set headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    // Set content type 
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Set utf8
    headers = curl_slist_append(headers, "charsets: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
      overlay_->view()->LoadURL("file:///signup-error.html");
    }

    else {
      long response_code;

      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      if (response_code == 200) {
        overlay_->view()->LoadURL("file:///signup-successful.html");
      }
    }
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
}

// Logout from Deltadex account and update auth file on filesystem.
void Launcher::Logout(const JSObject& obj, const JSArgs& args) {
  FILE *fp;
  char filename[FILENAME_MAX] = "response.json";

  std::ifstream file(filename);

  if (file_exists(filename)) {
    // clear the file's authentication details.
    fp = fopen(filename,"w");
  }
}

int update_progress_bar(void* ptr, double totalToDownload, double nowDownloaded, double totalToUpload, double nowUploaded)
{
    return 0;
}

static void downloadCards() {
    // Set up 
    CURL *curl;
    FILE *fp;
    CURLcode res;

    // Initialize URL which contains card
    const char *url = "https://raw.githubusercontent.com/DeltadexGame/Cards/master/cards/1.json";

    // Initialize output filename 
    char output_filename[FILENAME_MAX] = "assets/card1.json";

    // Code adapated from libcurl website: https://curl.haxx.se/libcurl/c/url2file.html
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        fp = fopen(output_filename,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Write data to the filesystem
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_filesystem);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);

        /* cleanup resources */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

static void downloadGame() {

  // Download the game. 
  // For now it just downloads a standard JAR open-source file available from GitHub and executes it.
  // When jar files will be built of the game, the following link below will be replaced.
  CURL *curl;
  FILE *fp;
  CURLcode res;

  // Initialize URL which contains card
  const char *url = "https://github.com/DeltadexGame/Client/releases/download/1.0/desktop-1.0.jar";

  // Initialize output filename 
  char output_filename[FILENAME_MAX] = "assets/deltadex.jar";

  // Code adapated from libcurl website: https://curl.haxx.se/libcurl/c/url2file.html
  curl = curl_easy_init();

  // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    fp = fopen(output_filename, "wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, update_progress_bar); 

    // Write data to the filesystem
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_filesystem);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);

    // if there was an error in the download, report to the user
    if(res != CURLE_OK) {
    }

    /* cleanup resources */
    curl_easy_cleanup(curl);
    fclose(fp);

  }
}

// Update cursor when user hovers over links
void Launcher::DisplayPage(const JSObject& obj, const JSArgs& args) {
  String path_to_file = "file:///";

  // Get the name of the desired page and append to the path
  String desired_page = args[0];
  path_to_file += desired_page;

  // Finally, append the file extension
  path_to_file += ".html";

  overlay_->view()->LoadURL(path_to_file);
}

// Clean up resources here
void Launcher::OnClose() {
}

void Launcher::OnUpdate() {
}

void Launcher::OnResize(uint32_t width, uint32_t height) {
  overlay_->Resize(width, height);
}

void Launcher::OnFinishLoading(View* caller) {
}

void Launcher::OnDownloadGame(const JSObject& obj, const JSArgs& args) {
  bool isLoggedIn = is_logged_in();

  if (isLoggedIn) {

    // downloadGame();

    std::ifstream response("response.json", std::ifstream::binary);
    Json::Value json_obj;

    response >> json_obj;

    Json::Value content = json_obj["content"];

    Json::Value username = content["username"];
    std::string token = content["token"].asString();

    std::string command = "start javaw -jar assets/deltadex.jar ";

    // Append token 
    command += token;

    char * command_cstr = new char [command.length()+1];
    std::strcpy (command_cstr, command.c_str());
    
    system(command_cstr);
  }

  else {
    std::string filename = "file:///login.html";
    char * filename_cstr = new char [filename.length()+1];
    std::strcpy (filename_cstr, filename.c_str());

    String filename_ultralight_str = ultralight::String(filename_cstr);
    overlay_->view()->LoadURL(filename_ultralight_str); 
  }
}

// When the DOM is ready and has finished loading, bind JS callbacks here. 
void Launcher::OnDOMReady(View* view) {
  SetJSContext(view->js_context());
  JSContextRef context = view->js_context();
  JSObject global = JSGlobalObject();

  global["SignUp"] = BindJSCallback(&Launcher::SignUp);
  global["Login"] = BindJSCallback(&Launcher::Login);
  global["OnDownloadGame"] = BindJSCallback(&Launcher::OnDownloadGame);
  global["VerifyAuth"] = BindJSCallbackWithRetval(&Launcher::VerifyAuth);
  global["Logout"] = BindJSCallback(&Launcher::Logout);
  global["DisplayPage"] = BindJSCallback(&Launcher::DisplayPage);

  view->EvaluateScript("display()");
}