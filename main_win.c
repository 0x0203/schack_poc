/* SC Hack PoC - Based on nuklear x11 demo - public domain
  This is a proof-of-concept demo only. It is throw-away code. It is bad.
  Don't use it for anything you care about. Or better yet, at all.
  Otherwise I reserve the right to mock you publicly for doing so. */
#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   1024

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D9_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_d3d9.h"

#define NKWINDOW	"MainWindow"

#include "packet.h"
#include "style.c"
#include "calculator.c"
#include "ascii.c"

typedef enum
{
    AEGS,
    ANVL,
    AOPO,
    BANU,
    CNOU,
    CRUS,
    DRAK,
    ESPR,
    KRIG,
    MISC,
    RSI_,
    MOBI,
    MANUFACTURE_COUNT
} manufacturer;

static const char *manufacturer_names[] =
{
    "Aegis Dynamics",
    "Anvil Aerospace",
    "AopoA",
    "Banu Souli",
    "Consolidated Outland",
    "Crusader Industries",
    "Drake Interplanetary",
    "Esperia Inc",
    "Kruger Intergalactic",
    "Musashi Industrial and Starflight Concern",
    "Roberts Space Industries",
    "MobiGlas by microTech"
};

static const char *exploit_names[] =
{
    "SAN Bus Buffer overflow",
    "Read Memory",
    "data intercept",
    "remote code execution",
    "data poison"
};

static const char *payload_names[] =
{
    "data intercept",
    "download firmware",
    "scan SANBus and chain exploit",
    "remote station console",
    "turret controller",
    "engine controller",
    "Door/lock managment",
    "data downloader"
};

static char *cap_strings[] = {"Capture", "End Capture"};
enum {NOCAPTURE, CAPTURE};

static IDirect3DDevice9 *device;
static IDirect3DDevice9Ex *deviceEx;
static D3DPRESENT_PARAMETERS present;

static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        if (device)
        {
            UINT width = LOWORD(lparam);
            UINT height = HIWORD(lparam);
            if (width != 0 && height != 0 &&
                (width != present.BackBufferWidth || height != present.BackBufferHeight))
            {
                nk_d3d9_release();
                present.BackBufferWidth = width;
                present.BackBufferHeight = height;
                HRESULT hr = IDirect3DDevice9_Reset(device, &present);
                NK_ASSERT(SUCCEEDED(hr));
                nk_d3d9_resize(width, height);
            }
        }
        break;
    }

    if (nk_d3d9_handle_event(wnd, msg, wparam, lparam))
        return 0;

    return DefWindowProcW(wnd, msg, wparam, lparam);
}

static void create_d3d9_device(HWND wnd)
{
    HRESULT hr;

    present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    present.BackBufferWidth = WINDOW_WIDTH;
    present.BackBufferHeight = WINDOW_HEIGHT;
    present.BackBufferFormat = D3DFMT_X8R8G8B8;
    present.BackBufferCount = 1;
    present.MultiSampleType = D3DMULTISAMPLE_NONE;
    present.SwapEffect = D3DSWAPEFFECT_DISCARD;
    present.hDeviceWindow = wnd;
    present.EnableAutoDepthStencil = TRUE;
    present.AutoDepthStencilFormat = D3DFMT_D24S8;
    present.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    present.Windowed = TRUE;

    {/* first try to create Direct3D9Ex device if possible (on Windows 7+) */
        typedef HRESULT WINAPI Direct3DCreate9ExPtr(UINT, IDirect3D9Ex**);
        Direct3DCreate9ExPtr *Direct3DCreate9Ex = (void *)GetProcAddress(GetModuleHandleA("d3d9.dll"), "Direct3DCreate9Ex");
        if (Direct3DCreate9Ex) {
            IDirect3D9Ex *d3d9ex;
            if (SUCCEEDED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9ex))) {
                hr = IDirect3D9Ex_CreateDeviceEx(d3d9ex, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
                    &present, NULL, &deviceEx);
                if (SUCCEEDED(hr)) {
                    device = (IDirect3DDevice9 *)deviceEx;
                } else {
                    /* hardware vertex processing not supported, no big deal
                    retry with software vertex processing */
                    hr = IDirect3D9Ex_CreateDeviceEx(d3d9ex, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
                        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
                        &present, NULL, &deviceEx);
                    if (SUCCEEDED(hr)) {
                        device = (IDirect3DDevice9 *)deviceEx;
                    }
                }
                IDirect3D9Ex_Release(d3d9ex);
            }
        }
    }

    if (!device) {
        /* otherwise do regular D3D9 setup */
        IDirect3D9 *d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

        hr = IDirect3D9_CreateDevice(d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
            &present, &device);
        if (FAILED(hr)) {
            /* hardware vertex processing not supported, no big deal
            retry with software vertex processing */
            hr = IDirect3D9_CreateDevice(d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
                &present, &device);
            NK_ASSERT(SUCCEEDED(hr));
        }
        IDirect3D9_Release(d3d9);
    }
}

/* creates a string padded out to the appropriate size.
   caller's responsibility to free ret */
static int shuffle_packet(char *packet, int packet_len, uint8_t **ret, int bph,
			  uint8_t *hop_list, int hops, int start_offset)
{
    int		    len;
    int		    pad;
    int		    i;
    int		    hop_index = 0;

    if (!ret || !packet || !hop_list || !bph || !hops)
	return 0;

    //len = strlen(packet);
    len = (bph * hops);
    len = len * ((packet_len + len - 1 - start_offset) / len);


    (*ret) = malloc(len);
    if (!*ret)
	return 0;

    memset(*ret, 0, len);

    for (i=0; i<len; i++)
    {
	hop_index = (i / bph) % hops;
	(*ret)[i] = (uint8_t)packet[i+start_offset] ^ (uint8_t)hop_list[hop_index];
    }
    return len;
}

static int pretty_print_clear(uint8_t *data, char *save, int data_len, int save_offset)
{
    int i, count = 0;
    char *special_out = NULL;

    for (i=0; i < data_len; i++)
    {
	if (data[i] == 0x01 && data[i+1] == 0x02)
	{
	    count += sprintf(&save[count+save_offset], "%s", "---Start Transmission---\nSTX\n");
	    i++;
	}
	else if (data[i] == 0x03 && data[i+1] == 0x04)
	{
	    count += sprintf(&save[count+save_offset], "%s", "\nETX\n---End Transmission--\n");
	    i++;
	}
	else if (data[i] == '\n')
	    count += sprintf(&save[count+save_offset], "%c", '\n');
	else if (data[i] < 0x20 || data[i] > 0x7e)
	    count += sprintf(&save[count+save_offset], "%s", "_.");
	else
	    count += sprintf(&save[count+save_offset], "%c", (char)data[i]);
    }

    return count;
}

static int pretty_print_raw(uint8_t *data, char *save, int data_len, int save_offset)
{
    int i, count = 0;

    count += sprintf(&save[count+save_offset], "0x%4.4X   ", 0);
    for (i=0; i < data_len; i++)
    {
	count += sprintf(&save[count+save_offset], "%2.2X ", data[i]);
	if (((i+1) % 16) == 0)
	{
	    count += sprintf(&save[count+save_offset], "\n0x%4.4X   ", i+1);
	    continue;
	}
	if (((i+1) % 8) == 0)
	    count += sprintf(&save[count+save_offset], "  ");
	if (((i+1) % 4) == 0)
	    count += sprintf(&save[count+save_offset], "  ");
    }
    return count;
}

int main(void)
{
    long dt;
    long started;
    int running = 1;
    e_theme sel_theme = THEME_DARK;
    int raw_mode = 1;
    int	packets_captured = 1;
    int show_calc = 0;
    int show_ascii = 0;
    struct nk_context *ctx;
    struct nk_colorf bg;
    int num_hops = 1;
    int bytesphop = 6;
    int i, j;
    int	offset = 0;
    int freqs[6];
    uint8_t dfreqs[6];
    int selected_manufacturer = 0;
    int selected_exploit = 0;
    int selected_payload = 0;
    int capture_state = NOCAPTURE;
    int first_packet_offset = 0;
    char *capbut_text = cap_strings[capture_state];
    int raw_text_len;
    int clear_text_len;
    char raw_text[1<<16];
    char clear_text[1<<16];
    char total_crypt_string[1<<16];
    int	total_crypt_len;
    int popup_error = 0;
    int first_loop = 1; //HACK!!!
    int	frequency_locked = 0;
    struct nk_style_button inactive_button_style;
    struct nk_style_button active_button_style;
    struct
    {
	manufacturer id;
	const char *sig;
	const char *name;
	int	    num_hops;
	int	    bph;
	uint8_t	key[6];
    } manufacturers[] = {
	{AEGS,  "AEGS", manufacturer_names[AEGS], 0, 0, {0,0,0,0,0,0}},
	{ANVL,  "ANVL", manufacturer_names[ANVL], 0, 0, {0,0,0,0,0,0}},
	{AOPO,  "AOPO", manufacturer_names[AOPO], 0, 0, {0,0,0,0,0,0}},
	{BANU,  "BANU", manufacturer_names[BANU], 0, 0, {0,0,0,0,0,0}},
	{CNOU,  "CNOU", manufacturer_names[CNOU], 0, 0, {0,0,0,0,0,0}},
	{CRUS,  "CRUS", manufacturer_names[CRUS], 0, 0, {0,0,0,0,0,0}},
	{DRAK,  "DRAK", manufacturer_names[DRAK], 0, 0, {0,0,0,0,0,0}},
	{ESPR,  "ESPR", manufacturer_names[ESPR], 0, 0, {0,0,0,0,0,0}},
	{KRIG,  "KRIG", manufacturer_names[KRIG], 0, 0, {0,0,0,0,0,0}},
	{MISC,  "MISC", manufacturer_names[MISC], 0, 0, {0,0,0,0,0,0}},
	{RSI_,  "RSI_", manufacturer_names[RSI_], 0, 0, {0,0,0,0,0,0}},
	{MOBI,  "MOBI", manufacturer_names[MOBI], 0, 0, {0,0,0,0,0,0}},
    };

    freqs[0] = freqs[1] = freqs[2] = freqs[3] = freqs[4] = freqs[5] =  0;
    dfreqs[0] = dfreqs[1] = dfreqs[2] = dfreqs[3] = dfreqs[4] = dfreqs[5] =  0;

    memset(clear_text, 0, sizeof(clear_text));

    WNDCLASSW wc;
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;
    HWND wnd;

    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"NuklearWindowClass";
    RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, style, FALSE, exstyle);

    wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Nuklear Demo",
        style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);

    create_d3d9_device(wnd);

    /* GUI */
    ctx = nk_d3d9_init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
    struct nk_font_atlas *atlas;
    nk_d3d9_font_stash_begin(&atlas);
    nk_d3d9_font_stash_end();
    }

    inactive_button_style = ctx->style.button;
    inactive_button_style.normal = nk_style_item_color(nk_rgb(90,90,90));
    inactive_button_style.hover = nk_style_item_color(nk_rgb(90,90,90));
    inactive_button_style.active = nk_style_item_color(nk_rgb(90,90,90));
    inactive_button_style.border_color = nk_rgb(110,110,110);
    inactive_button_style.text_background = nk_rgb(60,60,60);
    inactive_button_style.text_normal = nk_rgb(60,60,60);
    inactive_button_style.text_hover = nk_rgb(60,60,60);
    inactive_button_style.text_active = nk_rgb(60,60,60);

    bg.r = 0.0f, bg.g = 0.0, bg.b = 0.0f, bg.a = 1.0f;

    /* Init ship packet formatting data */
    srand(time(NULL));
    for (i=0; i< NK_LEN(manufacturers); i++)
    {
	int j;
	int bph = 4 + (2*(rand() % 3));
	int n = 1 + (rand() % 6);
	manufacturers[i].bph = bph;
	manufacturers[i].num_hops = n;
	printf("%d  %d  -  ", bph, n);
	for (j=0; j < n; j++)
	{
	    manufacturers[i].key[j] = rand() % 256;
	    printf("%d ", manufacturers[i].key[j]);
	}
	printf("\n");
    }

    while (running)
    {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

	if (ctx->input.keyboard.keys[NK_KEY_TEXT_RESET_MODE].down)
	    break;

        nk_input_end(ctx);

        /* GUI */

/* my code here */
        if (nk_begin(ctx, NKWINDOW, nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0))
        {

	    /* Theme Selection Menu */
	    nk_menubar_begin(ctx);
	    nk_layout_row_begin(ctx, NK_STATIC, 25, 2);
	    nk_layout_row_push(ctx, 45);
	    if (nk_menu_begin_label(ctx, "Theme", NK_TEXT_LEFT, nk_vec2(80,200)))
	    {
		nk_layout_row_dynamic(ctx, 15, 1);
		if (nk_option_label(ctx, "Black", sel_theme == THEME_BLACK)) sel_theme = THEME_BLACK;
		nk_layout_row_dynamic(ctx, 15, 1);
		if (nk_option_label(ctx, "White", sel_theme == THEME_WHITE)) sel_theme = THEME_WHITE;
		nk_layout_row_dynamic(ctx, 15, 1);
		if (nk_option_label(ctx, "Red",   sel_theme == THEME_RED)) sel_theme = THEME_RED;
		nk_layout_row_dynamic(ctx, 15, 1);
		if (nk_option_label(ctx, "Blue",  sel_theme == THEME_BLUE)) sel_theme = THEME_BLUE;
		nk_layout_row_dynamic(ctx, 25, 1);
		if (nk_option_label(ctx, "Dark",  sel_theme == THEME_DARK)) sel_theme = THEME_DARK;
		nk_menu_end(ctx);
	    }
	    set_style(ctx, sel_theme);
	    active_button_style = ctx->style.button;
	    nk_layout_row_push(ctx, 45);
	    if (nk_menu_begin_label(ctx, "Tools", NK_TEXT_LEFT, nk_vec2(80,200)))
	    {
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_menu_item_label(ctx, "Calculator", NK_TEXT_LEFT))
                    show_calc = nk_true;
		nk_layout_row_dynamic(ctx, 25, 1);
		if (nk_menu_item_label(ctx, "ASCII Chart", NK_TEXT_LEFT))
		    show_ascii = nk_true;

                nk_menu_end(ctx);
	    }

	    nk_layout_row_dynamic(ctx, 35, 2);
	    nk_button_label(ctx, "Load Profile");
	    nk_button_label(ctx, "Save Profile");

	    /* Load Manufacturer dycrypter key */
	    nk_layout_row_begin(ctx, NK_STATIC, 25, 3);
	    {
		int n = 0;
		nk_layout_row_push(ctx, 200);
		nk_label(ctx, "Load Manufacturer Dycryption Key", NK_TEXT_RIGHT);
		nk_layout_row_push(ctx, 300);
		if (first_loop ||
		    selected_manufacturer != (n = nk_combo(ctx, manufacturer_names,
							   NK_LEN(manufacturer_names),
							   selected_manufacturer,
							   25, nk_vec2(300,300))))
		{
		    uint8_t *crypt_string;
		    int cp_len;

		    if (capture_state == CAPTURE)
		    {
			popup_error = 1;
			goto popup;
		    }

		    total_crypt_len = 0;
		    selected_manufacturer = n;

		    first_packet_offset = (rand() % (strlen((data_packets[n].packets)[0])/2));
		    printf("first packet offset: %d\n", first_packet_offset);

		    memset(total_crypt_string, 0, sizeof(total_crypt_string));

		    for (i=0; i < data_packets[n].count; i++)
		    {
			cp_len = shuffle_packet((char *)((data_packets[n].packets)[i]),
						strlen((data_packets[n].packets)[i]),
						&crypt_string,
						manufacturers[n].bph,
						manufacturers[n].key,
						manufacturers[n].num_hops,
						0);
			memcpy(&(total_crypt_string[total_crypt_len]), crypt_string, cp_len);
			total_crypt_len += cp_len;
			free(crypt_string);
		    }
		}
popup:
		if (popup_error)
		{
		    static struct nk_rect s = {200, 100, 400, 120};
		    if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Error",
			NK_WINDOW_CLOSABLE|NK_WINDOW_TITLE, s))
		    {
			nk_window_set_focus(ctx, "Error");
			nk_layout_row_dynamic(ctx, 25, 1);
			nk_label(ctx, "Cannot switch decryption modules while "
					"capture is active", NK_TEXT_CENTERED);
			nk_layout_row_dynamic(ctx, 25, 1);
			if (nk_button_label(ctx, "OK"))
			{
			    popup_error = 0;
			    nk_popup_close(ctx);
			}
			nk_popup_end(ctx);
		    }
		}
		nk_button_label(ctx, "Load from Firmware");
	    }
	    nk_layout_row_end(ctx);

	    nk_layout_row_dynamic(ctx, 50, 1);
	    nk_label(ctx, "Frequency Hop Settings", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 25, 3);
	    nk_label(ctx, "", NK_TEXT_CENTERED);
            nk_property_int(ctx, "Number of Frequency Hops:", 1, &num_hops, 6, 1, 0.5f);
	    nk_label(ctx, "", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 25, 3);
	    nk_label(ctx, "", NK_TEXT_CENTERED);
            nk_property_int(ctx, "Bytes per Hop:", 4, &bytesphop, 8, 2, 2);
	    nk_label(ctx, "", NK_TEXT_CENTERED);

	    for (i=0; i < num_hops; i++)
	    {
		nk_layout_row_begin(ctx, NK_DYNAMIC, 25, 2);
		nk_layout_row_push(ctx, 0.10f);
		nk_labelf(ctx, NK_TEXT_LEFT, "Frequency %d :", i);
		nk_layout_row_push(ctx, 0.85f);
		nk_slider_int(ctx, 0, &freqs[i], 255, 1);
		dfreqs[i] = (uint8_t)freqs[i];
		nk_layout_row_push(ctx, 0.05f);
		nk_labelf(ctx, NK_TEXT_LEFT, "%d.%d MHz", 110 + selected_manufacturer, freqs[i]);
	    }

	    nk_layout_row_dynamic(ctx, 40, 1);
	    if (nk_button_label(ctx, capbut_text))
	    {
		capture_state = (capture_state == NOCAPTURE) ? CAPTURE : NOCAPTURE;
		capbut_text = cap_strings[capture_state];
		if (capture_state == CAPTURE)
		    packets_captured = 1;
	    }

	    nk_layout_row_begin(ctx, NK_DYNAMIC, 25, 3);
	    nk_layout_row_push(ctx, 0.25f);
	    if (nk_option_label(ctx, "Raw Data", raw_mode == 1)) raw_mode = 1;
	    nk_layout_row_push(ctx, 0.25f);
	    if (nk_option_label(ctx, "Frequency Chart", raw_mode == 0)) raw_mode = 0;
	    nk_layout_row_push(ctx, 0.5f);
	    nk_label(ctx, "Demangled Data", NK_TEXT_CENTERED);
	    nk_layout_row_end(ctx);

	    nk_layout_row_dynamic(ctx, 400, 2);

	    memset(raw_text, 0, sizeof(raw_text));
	    memset(clear_text, 0, sizeof(clear_text));
	    raw_text_len = clear_text_len = 0;

	    if (capture_state == CAPTURE)
	    {
		uint8_t *decrypt_string = NULL;
		int	decrypt_len;
		int	show_len = 0;
		int	len, packet_len;

		if (rand() % 800 == 0)
		    packets_captured++;

		for (i=0; i < packets_captured && i<data_packets[selected_manufacturer].count; i++)
		{
		    packet_len = strlen(data_packets[selected_manufacturer].packets[i]);
		    len = (manufacturers[selected_manufacturer].bph * manufacturers[selected_manufacturer].num_hops);
		    show_len += len * ((packet_len + len - 1) / len);

		    if (i==0) show_len -= first_packet_offset;
		}


		raw_text_len = pretty_print_raw((uint8_t *)(total_crypt_string + first_packet_offset), raw_text,
						show_len, 0);

		decrypt_len = shuffle_packet((char *)(&total_crypt_string[offset + first_packet_offset]),
					    total_crypt_len, &decrypt_string,
					     bytesphop, dfreqs, num_hops, 0);
		if (decrypt_len == 0)
		    goto cleanup;
		clear_text_len = pretty_print_clear(decrypt_string, clear_text, show_len - offset, 0);

		free(decrypt_string);
	    }

	    if (raw_mode)
	    {
		nk_edit_string(ctx,
			      NK_EDIT_EDITOR,
			      raw_text,
			      &raw_text_len,
			      sizeof(raw_text),
			      nk_filter_ascii);
	    }
	    else
	    {
		nk_flags result;
		struct nk_rect bounds;
		bounds = nk_widget_bounds(ctx);
		if (nk_chart_begin(ctx, NK_CHART_LINES, 256, 0.0f, 1.0f))
		{
		    const float real_factor = 0.1f;
		    const float select_factor = 0.6f;
		    float range = 0.0, floor = 0.0;
		    float res = 0;
		    float boost_factor;
		    int dist, max;
		    int n = manufacturers[selected_manufacturer].num_hops;

		    for (i=0; i<256; i++)
		    {
			res = dist = 0;

			for (j=0; j<n; j++)
			{
			    dist += j>num_hops ? 255 : NK_ABS(freqs[j] - manufacturers[selected_manufacturer].key[j]);
			}
			max = 255 * n;
			boost_factor = (1.0f - ((float)dist / (float)max)) * select_factor;

			for (j=0; j<n; j++)
			{
			    dist = NK_ABS(manufacturers[selected_manufacturer].key[j] - i);
			    if (dist)
			    {
				res += (real_factor + boost_factor) / (float)dist;
				res = NK_MIN(res, real_factor + boost_factor);
			    }
			    else
				res = real_factor + boost_factor;
			}
			floor = (1.0f - (real_factor + select_factor))/2.0f;
			range = ((float)rand()/((float)RAND_MAX/(floor*2.0))) - floor;
			if (capture_state == CAPTURE)
			    result = nk_chart_push(ctx, res + range + floor);

		    }
		    nk_chart_end(ctx);

		    /* mouse over char */
		    if (capture_state == CAPTURE &&
			nk_input_is_mouse_hovering_rect(&(ctx->input), bounds))
		    {
			float x, y;
			float hover_freq;
			struct nk_color c = {8,24,158,128};

			x = ctx->input.mouse.pos.x - bounds.x;

			hover_freq = (255.0 * x) / bounds.w;
			nk_stroke_line(&ctx->current->buffer, ctx->input.mouse.pos.x, bounds.y,
				       ctx->input.mouse.pos.x, bounds.y + bounds.h, 1, c);
			nk_tooltipf(ctx, "%d.%d", 110 + selected_manufacturer, (int)hover_freq);
		    }
		}

	    }

	    nk_edit_string(ctx,
			  NK_EDIT_EDITOR,
			  clear_text,
			  &clear_text_len,
			  sizeof(clear_text),
			  nk_filter_ascii);

	    /* Offset slider */
	    nk_layout_row_begin(ctx, NK_DYNAMIC, 25, 3);
	    nk_layout_row_push(ctx, 0.10f);
	    nk_label(ctx, "Starting offset:", NK_TEXT_LEFT);
	    nk_layout_row_push(ctx, 0.85f);
	    offset = NK_MIN(offset, strlen((data_packets[selected_manufacturer]).packets[0]));
	    nk_slider_int(ctx, 0, &offset,
			  strlen((data_packets[selected_manufacturer]).packets[0]), 1);
	    nk_layout_row_push(ctx, 0.05f);
	    nk_labelf(ctx, NK_TEXT_LEFT, "%d", offset);

	    frequency_locked = 1;
	    if (num_hops == (j = manufacturers[selected_manufacturer].num_hops) &&
		bytesphop == manufacturers[selected_manufacturer].bph &&
		capture_state == CAPTURE)
	    {
		for (i = 0; i < j; i++)
		{
		    if (dfreqs[i] != manufacturers[selected_manufacturer].key[i])
		    {
			frequency_locked = 0;
			break;
		    }
		}
	    }
	    else
		frequency_locked = 0;


	    /* Exploits and Payloads */

	    /* Load Manufacturer dycrypter key */
	    nk_layout_row_dynamic(ctx, 30, 4);
	    nk_label(ctx, "Select Exploit(s)", NK_TEXT_RIGHT);
	    selected_exploit = nk_combo(ctx, exploit_names, NK_LEN(exploit_names),
					    selected_exploit, 25, nk_vec2(300,300));

	    nk_label(ctx, "Attach Payload(s)", NK_TEXT_RIGHT);
	    selected_payload = nk_combo(ctx, payload_names, NK_LEN(payload_names),
					    0, 25, nk_vec2(300,300));
	    nk_layout_row_dynamic(ctx, 35, 1);
	    ctx->style.button = frequency_locked ? active_button_style : inactive_button_style;
	    int foo = 0;
	    if (nk_button_label(ctx, "Launch Attack") && frequency_locked)
	    {
		foo = 1;
	    }
	    ctx->style.button = active_button_style;
	    if (foo)
	    {
		struct nk_rect bounds;
		bounds = nk_widget_bounds(ctx);
		bounds.y -= 300;
		bounds.h += 300;
		if (nk_popup_begin(ctx, NK_POPUP_DYNAMIC, "pwnd",
		    NK_WINDOW_TITLE, bounds))
		{
		    nk_window_set_focus(ctx, "pwnd");
		    nk_layout_row_dynamic(ctx, 40, 1);
		    nk_label(ctx, "Attack Successful! (The appropriate payload would now run for as long as the target is in range and we stay tuned to the correct frequencies).", NK_TEXT_CENTERED);
		    nk_layout_row_dynamic(ctx, 40, 1);
		    if (nk_button_label(ctx, "OK"))
		    {
			foo = 0;
			nk_popup_close(ctx);
		    }
		    nk_popup_end(ctx);
		}
	    }
        }
        nk_end(ctx);

	show_calc = calculator(ctx, show_calc);
	show_ascii = ascii(ctx, show_ascii);

        if (nk_window_is_hidden(ctx, NKWINDOW)) break;

/* ************ */

        /* Draw */
        {
            HRESULT hr;
            hr = IDirect3DDevice9_Clear(device, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
                D3DCOLOR_COLORVALUE(bg.a, bg.r, bg.g, bg.b), 0.0f, 0);
            NK_ASSERT(SUCCEEDED(hr));

            hr = IDirect3DDevice9_BeginScene(device);
            NK_ASSERT(SUCCEEDED(hr));
            nk_d3d9_render(NK_ANTI_ALIASING_ON);
            hr = IDirect3DDevice9_EndScene(device);
            NK_ASSERT(SUCCEEDED(hr));

            if (deviceEx) {
                hr = IDirect3DDevice9Ex_PresentEx(deviceEx, NULL, NULL, NULL, NULL, 0);
            } else {
                hr = IDirect3DDevice9_Present(device, NULL, NULL, NULL, NULL);
            }
            if (hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICEHUNG || hr == D3DERR_DEVICEREMOVED) {
                /* to recover from this, you'll need to recreate device and all the resources */
                MessageBoxW(NULL, L"D3D9 device is lost or removed!", L"Error", 0);
                break;
            } else if (hr == S_PRESENT_OCCLUDED) {
                /* window is not visible, so vsync won't work. Let's sleep a bit to reduce CPU usage */
                Sleep(10);
            }
            NK_ASSERT(SUCCEEDED(hr));
        }
	first_loop = 0;
    }
cleanup:
    nk_d3d9_shutdown();
    if (deviceEx)IDirect3DDevice9Ex_Release(deviceEx);
    else IDirect3DDevice9_Release(device);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}
