#include <AppCore/JSHelpers.h>
#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

#include "Launcher.h"
#include <iostream>
#include <String.h>

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

// When the user clicks a button to switch views, 
// this function gets called and depending on the button clicked,
// the view will update accordingly.
void Launcher::UpdateView(const JSObject& obj, const JSArgs& args) {
  // Start of path
  String path_to_file = "file:///";

  // Get the name of the desired page and append to the path
  String desired_page = args[0];
  path_to_file += desired_page;

  // Finally, append the file extension
  path_to_file += ".html";

  overlay_->view()->LoadURL(path_to_file);
}

static size_t write_data_to_filesystem(void *pointer, size_t size, size_t nmemb, void *stream)
{
  size_t is_written_to_filesystem = fwrite(pointer, size, nmemb, (FILE *)stream);
  return is_written_to_filesystem;
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

// Update cursor when user hovers over links
void Launcher::UpdateCursor(const JSObject& obj, const JSArgs& args) {
  Cursor cursor = Cursor::kCursor_Pointer;
  window_->SetCursor(cursor);
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

JSValueRef OnButtonClick(JSContextRef ctx, JSObjectRef function,
  JSObjectRef thisObject, size_t argumentCount, 
  const JSValueRef arguments[], JSValueRef* exception) {
  
  const char* str = 
    "document.getElementById('result').innerText = 'Launching game ...'";

  JSStringRef script = JSStringCreateWithUTF8CString(str);

  JSEvaluateScript(ctx, script, 0, 0, 0, 0);

  JSStringRelease(script);

  downloadCards();
  // Download the game. 
  // For now it just downloads a standard JAR open-source file available from GitHub and executes it.
  // When jar files will be built of the game, the following link below will be replaced.
	// HRESULT hr = URLDownloadToFile ( NULL, _T("https://drive.google.com/uc?export=download&id=18ObJuHnwmoLe0r09KtxU3oZyaMckWm6A"), _T("assets/deltadex.jar"), 0, NULL );
  // Open the JAR file by executing the following command in WINDOWS.


  return JSValueMakeNull(ctx);
}


// When the DOM is ready and has finished loading, bind JS callbacks here. 
void Launcher::OnDOMReady(View* view) {
  SetJSContext(view->js_context());
  JSContextRef context = view->js_context();
  JSObject global = JSGlobalObject();

  global["OnUpdateView"] = BindJSCallback(&Launcher::UpdateView);
  global["OnUpdateCursor"] = BindJSCallback(&Launcher::UpdateCursor);
  global["GetMessage"] = BindJSCallback(&Launcher::UpdateView);

  JSStringRef name = JSStringCreateWithUTF8CString("OnButtonClick");

  JSObjectRef function = JSObjectMakeFunctionWithCallback(context, name, 
                                                      OnButtonClick);
  
  JSObjectRef globalObj = JSContextGetGlobalObject(context);

  JSObjectSetProperty(context, globalObj, name, function, 0, 0);

  JSStringRelease(name);
}