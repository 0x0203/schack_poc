/* SC Hack PoC - Based on nuklear x11 demo - public domain
  This is a proof-of-concept demo only. It is throw-away code. It is bad.
  Don't use it for anything you care about. Or better yet, at all.
  Otherwise I reserve the right to mock you publicly for doing so. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_xlib.h"

#define DTIME           20
#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   1024

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

typedef struct XWindow XWindow;
struct XWindow {
    Display *dpy;
    Window root;
    Visual *vis;
    Colormap cmap;
    XWindowAttributes attr;
    XSetWindowAttributes swa;
    Window win;
    int screen;
    XFont *font;
    unsigned int width;
    unsigned int height;
    Atom wm_delete_window;
};

static void
die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

static long
timestamp(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) return 0;
    return (long)((long)tv.tv_sec * 1000 + (long)tv.tv_usec/1000);
}

static void
sleep_for(long t)
{
    struct timespec req;
    const time_t sec = (int)(t/1000);
    const long ms = t - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = ms * 1000000L;
    while(-1 == nanosleep(&req, &req));
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

int
main(void)
{
    long dt;
    long started;
    int running = 1;
    e_theme sel_theme = THEME_DARK;
    int raw_mode = 1;
    int	packets_captured = 1;
    XWindow xw;
    XWindowAttributes attr;
    int show_calc = 0;
    int show_ascii = 0;
    struct nk_context *ctx;
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

    /* X11 */
    memset(&xw, 0, sizeof xw);
    xw.dpy = XOpenDisplay(NULL);
    if (!xw.dpy) die("Could not open a display; perhaps $DISPLAY is not set?");
    xw.root = DefaultRootWindow(xw.dpy);
    xw.screen = XDefaultScreen(xw.dpy);
    xw.vis = XDefaultVisual(xw.dpy, xw.screen);
    xw.cmap = XCreateColormap(xw.dpy,xw.root,xw.vis,AllocNone);

    xw.swa.colormap = xw.cmap;
    xw.swa.event_mask =
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask| ButtonMotionMask |
        Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
        PointerMotionMask | KeymapStateMask;
    xw.win = XCreateWindow(xw.dpy, xw.root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
        XDefaultDepth(xw.dpy, xw.screen), InputOutput,
        xw.vis, CWEventMask | CWColormap, &xw.swa);

    XStoreName(xw.dpy, xw.win, "Ethershark - (SC Hack PoC)");
    XMapWindow(xw.dpy, xw.win);
    xw.wm_delete_window = XInternAtom(xw.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(xw.dpy, xw.win, &xw.wm_delete_window, 1);
    XGetWindowAttributes(xw.dpy, xw.win, &xw.attr);
    xw.width = (unsigned int)xw.attr.width;
    xw.height = (unsigned int)xw.attr.height;

    /* GUI */
    xw.font = nk_xfont_create(xw.dpy, "fixed");
    ctx = nk_xlib_init(xw.font, xw.dpy, xw.screen, xw.win, xw.width, xw.height);

    inactive_button_style = ctx->style.button;
    inactive_button_style.normal = nk_style_item_color(nk_rgb(90,90,90));
    inactive_button_style.hover = nk_style_item_color(nk_rgb(90,90,90));
    inactive_button_style.active = nk_style_item_color(nk_rgb(90,90,90));
    inactive_button_style.border_color = nk_rgb(110,110,110);
    inactive_button_style.text_background = nk_rgb(60,60,60);
    inactive_button_style.text_normal = nk_rgb(60,60,60);
    inactive_button_style.text_hover = nk_rgb(60,60,60);
    inactive_button_style.text_active = nk_rgb(60,60,60);


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
        /* ============================ Input ============================= */
        XEvent evt;
        started = timestamp();
        nk_input_begin(ctx);
        while (XPending(xw.dpy)) {
            XNextEvent(xw.dpy, &evt);
            if (evt.type == ClientMessage) goto cleanup;
            if (XFilterEvent(&evt, xw.win)) continue;
            nk_xlib_handle_event(xw.dpy, xw.screen, xw.win, &evt);
        }

	if (ctx->input.keyboard.keys[NK_KEY_TEXT_RESET_MODE].down)
	    break;

        nk_input_end(ctx);
	/* ================================================================= */


        /* ============================== GUI ============================== */
        if (nk_begin(ctx, NKWINDOW, nk_rect(0, 0, xlib.surf->w, xlib.surf->h), 0))
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
	    static int foo = 0;
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
	/* ================================================================= */


        /* =============================== Draw ============================ */
        XClearWindow(xw.dpy, xw.win);
        nk_xlib_render(xw.win, nk_rgb(30,30,30));
        XFlush(xw.dpy);
	/* ================================================================= */


        /* ============================== Timing =========================== */
        dt = timestamp() - started;
        if (dt < DTIME)
            sleep_for(DTIME - dt);
	/* ================================================================= */
	first_loop = 0;
    }

cleanup:
    nk_xfont_del(xw.dpy, xw.font);
    nk_xlib_shutdown();
    XUnmapWindow(xw.dpy, xw.win);
    XFreeColormap(xw.dpy, xw.cmap);
    XDestroyWindow(xw.dpy, xw.win);
    XCloseDisplay(xw.dpy);
    return 0;
}

