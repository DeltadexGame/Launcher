#pragma once
#include <AppCore/AppCore.h>

using namespace ultralight;

class Launcher : public AppListener,
              public WindowListener,
              public LoadListener {
public:
  Launcher();

  virtual ~Launcher();

  virtual void Run();

  virtual void OnUpdate() override;

  virtual void OnClose() override;

  virtual void OnResize(uint32_t width, uint32_t height) override;

  virtual void OnFinishLoading(View* caller) override;

  virtual void UpdateView(const JSObject& obj, const JSArgs& args);

  virtual void SignUp(const JSObject& obj, const JSArgs& args);

  virtual void UpdateCursor(const JSObject& obj, const JSArgs& args);

  virtual void OnDOMReady(View* caller) override;

protected:
  RefPtr<App> app_;
  RefPtr<Window> window_;
  RefPtr<Overlay> overlay_;
};