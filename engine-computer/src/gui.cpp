#include "mach/gui.hpp"
#include <thread>
#include <filesystem>
#include <thread>
#include <wx/stdpaths.h>
#include <spdlog/spdlog.h>
#include "mach/device/device_manager.hpp"

namespace mach::gui {

static void _startGUI(int argc, char* argv[]) {
    wxApp::SetInstance(new MyApp());
    wxEntryStart(argc, argv);
    wxTheApp->OnInit();
    wxTheApp->OnRun();
    wxTheApp->OnExit();
    wxEntryCleanup();
}

void startGUI(int argc, char* argv[]) {
    std::thread(_startGUI, argc, argv).detach();
}
 
bool MyApp::OnInit() {
    wxImage::AddHandler(new wxPNGHandler);
    MyFrame *frame = new MyFrame("MACH Engine Computer");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300), 
              wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Load logo image
    wxString resourcesPath = wxStandardPaths::Get().GetResourcesDir();
    spdlog::info("MACH: GUI Resources path: {}", resourcesPath.ToStdString());

    wxImage logoImage;
    if (!logoImage.LoadFile(resourcesPath + std::filesystem::path::preferred_separator + "resources" + 
                            std::filesystem::path::preferred_separator + "mach_logo.png", wxBITMAP_TYPE_PNG)) {
        wxMessageBox("Failed to load GUI logo image!", "Error", wxOK | wxICON_ERROR);
        wxTheApp->OnExit();
        return;
    }

    // Header
    wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

    // Scale the logo image to be 50px high
    int newHeight = 75;
    int newWidth = (logoImage.GetWidth() * newHeight) / logoImage.GetHeight();
    logoImage = logoImage.Scale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH);

    // Add logo
    wxStaticBitmap* logoBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(logoImage));
    headerSizer->Add(logoBitmap, 0, wxALIGN_LEFT | wxALL, 5);

    // Add title
    wxStaticText* titleText = new wxStaticText(this, wxID_ANY, "MACH Engine Computer");
    titleText->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    headerSizer->Add(titleText, 0, wxALIGN_CENTER | wxALL, 5);

    sizer->Add(headerSizer, 0, wxALIGN_CENTER);

    // Set the application icon
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxBitmap(logoImage)); // Use the original size image for the icon
    SetIcon(appIcon);

    // Buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxFont buttonFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    wxButton* abortButton = new wxButton(this, ID_Abort, "Abort");
    abortButton->SetBackgroundColour(*wxRED);
    abortButton->SetForegroundColour(*wxWHITE);
    abortButton->SetMinSize(wxSize(100, 30));
    abortButton->SetFont(buttonFont);
    buttonSizer->Add(abortButton, 2, wxALL, 5);

    wxButton* reconnectButton = new wxButton(this, ID_Reconnect, "Reconnect");
    reconnectButton->SetBackgroundColour(*wxWHITE);
    reconnectButton->SetForegroundColour(*wxBLACK);
    reconnectButton->SetMinSize(wxSize(100, 30));
    reconnectButton->SetFont(buttonFont);
    buttonSizer->Add(reconnectButton, 2, wxALL, 5);

    wxButton* disconnectButton = new wxButton(this, ID_Disconnect, "Disconnect");
    disconnectButton->SetBackgroundColour(*wxWHITE);
    disconnectButton->SetForegroundColour(*wxBLACK);
    disconnectButton->SetMinSize(wxSize(100, 30));
    disconnectButton->SetFont(buttonFont);
    buttonSizer->Add(disconnectButton, 2, wxALL, 5);

    sizer->Add(buttonSizer, 0, wxALIGN_CENTER);

    SetSizerAndFit(sizer);

    Bind(wxEVT_BUTTON, &MyFrame::OnAbort, this, ID_Abort);
    Bind(wxEVT_BUTTON, &MyFrame::OnDisconnect, this, ID_Disconnect);
    Bind(wxEVT_BUTTON, &MyFrame::OnReconnect, this, ID_Reconnect);
}

void MyFrame::OnAbort(wxCommandEvent& event)
{
    spdlog::info("MACH: Abort button clicked.");
    std::thread([]() {
        DeviceManager::getInstance().abortAllLabjacks();
    }).detach();
}

void MyFrame::OnDisconnect(wxCommandEvent& event)
{
    spdlog::info("MACH: Disconnect button clicked.");
    std::thread([]() {
        DeviceManager::getInstance().disconnectAllLabjacks();
    }).detach();
}

void MyFrame::OnReconnect(wxCommandEvent& event)
{
    spdlog::info("MACH: Reconnect button clicked.");
    std::thread([]() {
        DeviceManager::getInstance().reconnectAllLabjacks();
    }).detach();
}
 
void MyFrame::OnExit(wxCommandEvent& event)
{
    spdlog::info("MACH: GUI exited.");
    Close(true);
}

} // namespace mach::gui