#include "util.h"

#include <Windows.h>

#include <DXUT.h>
#include <iostream>


std::wstring GetExePath()
{
	// get full path to .exe
	const size_t bufferSize = 1024;
	wchar_t buffer[bufferSize];
	if(0 == GetModuleFileNameW(nullptr, buffer, bufferSize))
	{
		return std::wstring(L"");
	}
	std::wstring path(buffer);
	// extract path (remove filename)
	size_t posSlash = path.find_last_of(L"/\\");
	if(posSlash != std::wstring::npos)
	{
		path = path.substr(0, posSlash + 1);
	}
	return path;
}


void UpdateWindowTitle(const std::wstring& appName)
{
	// check if we should update the window title
	bool update = false;

	// update if window size changed
	static int s_windowWidth = 0;
	static int s_windowHeight = 0;
	if (s_windowWidth != DXUTGetWindowWidth() || s_windowHeight != DXUTGetWindowHeight()) {
		s_windowWidth = DXUTGetWindowWidth();
		s_windowHeight = DXUTGetWindowHeight();
		update = true;
	}

	// update if fps changed (updated once per second by DXUT)
	static float s_fps = 0.0f;
	static float s_mspf = 0.0f;
	if (s_fps != DXUTGetFPS()) {
		s_fps = DXUTGetFPS();
		s_mspf = 1000.0f / s_fps;
		update = true;
	}

	// update window title if something relevant changed
	if (update) {
		const size_t len = 512;
		wchar_t str[len];
		swprintf_s(str, len, L"%s %ux%u @ %.2f fps / %.2f ms", appName.c_str(), s_windowWidth, s_windowHeight, s_fps, s_mspf);
		SetWindowText(DXUTGetHWND(), str);
	}
}

void ToggleWindowedFullscreen()
{
    static RECT windowRectBackup;

    // Get current window style
    LONG_PTR style = GetWindowLongPtr(DXUTGetHWND(), GWL_STYLE);

    // Deterdetermine if we are currently in fullscreen
    bool isWindowedFS = (0 == (style & (WS_THICKFRAME | WS_CAPTION)));

    if (!isWindowedFS)
    {
        // Get info of the monitor on which the window is placed´on currently 
        HMONITOR hmon = MonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO moninfo = {sizeof(MONITORINFO)};
        if (GetMonitorInfo(hmon, &moninfo) == TRUE)
        {
            // Backup current window position
            GetWindowRect(DXUTGetHWND(), &windowRectBackup);

            // Remove window borders
            LONG_PTR newStyle = style & (~WS_THICKFRAME) & (~WS_CAPTION); 
            SetWindowLongPtr(DXUTGetHWND(), GWL_STYLE, newStyle);
                    
            // Move window to cover whole screen (triggers swap chain resize)
            const RECT& r = moninfo.rcMonitor;
            MoveWindow(DXUTGetHWND(), r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE); 
        }
        else
        {
            std::cerr << "Couldn't retrieve monitor info, not going into full-screen mode." << std::endl;
        }
    }
    else
    {
        // Re-add window borders
        LONG_PTR newStyle = style | WS_THICKFRAME | WS_CAPTION; 
        SetWindowLongPtr(DXUTGetHWND(), GWL_STYLE, newStyle);
                    
        // Restore original window position (triggers swap chain resize)
        const RECT& r = windowRectBackup;
        MoveWindow(DXUTGetHWND(), r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE); 
    }
}