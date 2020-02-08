#include "Launcher.h"
#include <AppCore/JSHelpers.h>
#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

#define WINDOW_WIDTH  1100
#define WINDOW_HEIGHT 800

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")
using namespace std;

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
  overlay_->view()->LoadURL("file:///app2.html");
}

// LoadSignIn --
void Launcher::LoadSignIn(const JSObject& obj, const JSArgs& args) {
  overlay_->view()->LoadURL("file:///signin.html");
}

void Launcher::LoadLogIn(const JSObject& obj, const JSArgs& args) {
  overlay_->view()->LoadURL("file:///login.html");
}

void Launcher::LoadLatestCards(const JSObject& obj, const JSArgs& args) {
  overlay_->view()->LoadURL("file:///latest-cards.html");
}

void Launcher::LoadHome(const JSObject& obj, const JSArgs& args) {
  overlay_->view()->LoadURL("file:///app.html");
}

void Launcher::LoadLatestNews(const JSObject& obj, const JSArgs& args) {
  overlay_->view()->LoadURL("file:///news.html");
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
  
  // Download the game. 
  // For now it just downloads a standard JAR open-source file available from GitHub and executes it.
  // When jar files will be built of the game, the following link below will be replaced.
  // HRESULT hresult;
  // LPCTSTR Url = _T("https://drive.google.com/uc?export=download&id=18ObJuHnwmoLe0r09KtxU3oZyaMckWm6A"), File = _T("assets/deltadex.jar");
  // hresult = URLDownloadToFile (0, Url, File, 0, 0);

  // Open the JAR file by executing the following command in WINDOWS.
  system("start javaw -jar assets/deltadex.jar");

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
  global["OnLoadLatestCards"] = BindJSCallback(&Launcher::LoadLatestCards);
  global["OnLoadLatestNews"] = BindJSCallback(&Launcher::LoadLatestNews);
  global["OnLoadLogIn"] = BindJSCallback(&Launcher::LoadLogIn);
  global["OnLoadHome"] = BindJSCallback(&Launcher::LoadHome);

  JSStringRef name = JSStringCreateWithUTF8CString("OnButtonClick");

  JSObjectRef function = JSObjectMakeFunctionWithCallback(context, name, 
                                                      OnButtonClick);
  
  JSObjectRef globalObj = JSContextGetGlobalObject(context);

  JSObjectSetProperty(context, globalObj, name, function, 0, 0);

  JSStringRelease(name);
}