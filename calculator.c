/* derived from nuklear - v1.00 - public domain */

#define WIN_CALC_NAME "Integer Calculator"

enum calc_mode {MODE_HEX, MODE_DEC};
static int sel_mode = MODE_HEX;

static int
calculator(struct nk_context *ctx, int show)
{
    int has_focus;

    if (show)
    {
	nk_window_show(ctx, WIN_CALC_NAME, NK_SHOWN);
	nk_window_set_focus(ctx, WIN_CALC_NAME);
    }
    else
	return 0;

    if (nk_begin(ctx, WIN_CALC_NAME, nk_rect(10, 10, 200, 350),
        NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|
	NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE))
    {
	has_focus = nk_window_has_focus(ctx);

        static int set = 0, prev = 0, op = 0;
	static long a = 0, b = 0;
	static long *current = &a;
	int base;

        int solve = 0;

	{
	    int len;
	    char in_buffer[256];
	    nk_plugin_filter filter;

	    nk_layout_row_dynamic(ctx, 35, 2);
	    if (nk_option_label(ctx, "Hexicecimal (0x)", sel_mode == MODE_HEX)) sel_mode = MODE_HEX;
	    if (nk_option_label(ctx, "Decimal", sel_mode == MODE_DEC)) sel_mode = MODE_DEC;
	    filter = sel_mode == MODE_HEX ? nk_filter_hex : nk_filter_decimal;
	    nk_layout_row_dynamic(ctx, 35, 1);
	    len = snprintf(in_buffer, 256, sel_mode == MODE_HEX ? "0x%lX" : "%ld", *current);
	    nk_edit_string(ctx, NK_EDIT_SIMPLE | NK_EDIT_READ_ONLY, in_buffer, &len, 255, filter);
	    in_buffer[len] = 0;
	    *current = strtol(in_buffer, NULL, 0);

	    nk_layout_row_dynamic(ctx, 25, 1);
	    nk_labelf(ctx, NK_TEXT_RIGHT, sel_mode == MODE_DEC ? "Hex: 0x%lX": "Dec: %ld", *current);
	}

	base = sel_mode == MODE_HEX ? 16 : 10;

	nk_layout_row_dynamic(ctx, 35, 6);
	if (nk_button_label(ctx, "D") && sel_mode == MODE_HEX){*current = *current * base + 0xd;set=0;}
	if (nk_button_label(ctx, "E") && sel_mode == MODE_HEX){*current = *current * base + 0xe;set=0;}
	if (nk_button_label(ctx, "F") && sel_mode == MODE_HEX){*current = *current * base + 0xf;set=0;}
	nk_label(ctx, "", NK_TEXT_CENTERED);
	if (nk_button_label(ctx, "^")){if (!set){if (current !=&b) current = &b; else {prev = op; solve=1;}}op='^';set=1;}
	if (nk_button_label(ctx, "&")){if (!set){if (current !=&b) current = &b; else {prev = op; solve=1;}}op='&';set=1;}

	nk_layout_row_dynamic(ctx, 35, 6);
	if (nk_button_label(ctx, "A") && sel_mode == MODE_HEX){*current = *current * base + 0xa;set=0;}
	if (nk_button_label(ctx, "B") && sel_mode == MODE_HEX){*current = *current * base + 0xb;set=0;}
	if (nk_button_label(ctx, "C") && sel_mode == MODE_HEX){*current = *current * base + 0xc;set=0;}
	nk_label(ctx, "", NK_TEXT_CENTERED);
	if (nk_button_label(ctx, "|")){if (!set){if (current !=&b) current = &b; else{ prev = op; solve=1;}}op='|';set=1;}
	nk_label(ctx, "", NK_TEXT_CENTERED);

	nk_layout_row_dynamic(ctx, 35, 6);
	if (nk_button_label(ctx, "7")){*current = *current * base + 7;set=0;}
	if (nk_button_label(ctx, "8")){*current = *current * base + 8;set=0;}
	if (nk_button_label(ctx, "9")){*current = *current * base + 9;set=0;}
	nk_label(ctx, "", NK_TEXT_CENTERED);
	if (nk_button_label(ctx, "+")){if (!set){if (current !=&b) current = &b; else {prev = op; solve=1;}}op='+';set=1;}
	if (nk_button_label(ctx, "-")){if (!set){if (current !=&b) current = &b; else {prev = op; solve=1;}}op='-';set=1;}

	nk_layout_row_dynamic(ctx, 35, 6);
	if (nk_button_label(ctx, "4")){*current = *current * base + 4;set=0;}
	if (nk_button_label(ctx, "5")){*current = *current * base + 5;set=0;}
	if (nk_button_label(ctx, "6")){*current = *current * base + 6;set=0;}
	nk_label(ctx, "", NK_TEXT_CENTERED);
	if (nk_button_label(ctx, "*")){if (!set){if (current !=&b) current = &b; else {prev = op; solve=1;}}op='*';set=1;}
	if (nk_button_label(ctx, "/")){if (!set){if (current !=&b) current = &b; else {prev = op; solve=1;}}op='/';set=1;}

	nk_layout_row_dynamic(ctx, 35, 6);
	if (nk_button_label(ctx, "1")){*current = *current * base + 1;set=0;}
	if (nk_button_label(ctx, "2")){*current = *current * base + 2;set=0;}
	if (nk_button_label(ctx, "3")){*current = *current * base + 3;set=0;}
	if (nk_button_label(ctx, "0")){*current = *current * base + 0;set=0;}
	if (nk_button_label(ctx, "Clr")){a = b = op = 0; current = &a; set = 0;}
	if (nk_button_label(ctx, "=")){solve=1; prev=op; op=0;}

        if (solve) {
            if (prev == '^') a = a ^ b;
            if (prev == '&') a = a & b;
            if (prev == '|') a = a | b;
            if (prev == '+') a = a + b;
            if (prev == '-') a = a - b;
            if (prev == '*') a = a * b;
            if (prev == '/') a = a / b;
            current = &a;
            if (set) current = &b;
            b = 0; set = 0;
        }
    }
    nk_end(ctx);

    if (nk_window_is_hidden(ctx, WIN_CALC_NAME) || nk_window_is_closed(ctx, WIN_CALC_NAME))
	return 0;

    return 1;
}

