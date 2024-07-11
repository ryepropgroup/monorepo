#pragma once
#include <wx/wx.h>

namespace mach::gui {

void startGUI(int argc, char* argv[]);

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

    void OnExit(wxCommandEvent& event);

    void OnAbort(wxCommandEvent& event);
    void OnDisconnect(wxCommandEvent& event);
    void OnReconnect(wxCommandEvent& event);
};

enum
{
    ID_Abort = 1,
    ID_Disconnect,
    ID_Reconnect
};

} // namespace mach::gui