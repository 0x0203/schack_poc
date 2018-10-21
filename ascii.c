/* public domain */

const char ascii_table[] =
"  Dec   Hex   Char                          Dec   Hex   Char          Dec   Hex   Char         Dec   Hex   Char\n"
"----------------------------------------------------------------------------------------------------------------\n"
"  0     00    NUL '\\0' (null character)  |  32    20    SPACE       |  64    40    @        |  96    60    `\n"
"  1     01    SOH (start of heading)     |  33    21    !           |  65    41    A        |  97    61    a\n"
"  2     02    STX (start of text)        |  34    22    \"           |  66    42    B        |  98    62    b\n"
"  3     03    ETX (end of text)          |  35    23    #           |  67    43    C        |  99    63    c\n"
"  4     04    EOT (end of transmission)  |  36    24    $           |  68    44    D        |  100   64    d\n"
"  5     05    ENQ (enquiry)              |  37    25    %           |  69    45    E        |  101   65    e\n"
"  6     06    ACK (acknowledge)          |  38    26    &           |  70    46    F        |  102   66    f\n"
"  7     07    BEL '\\a' (bell)            |  39    27    '           |  71    47    G        |  103   67    g\n"
"  8     08    BS  '\\b' (backspace)       |  40    28    (           |  72    48    H        |  104   68    h\n"
"  9     09    HT  '\\t' (horizontal tab)  |  41    29    )           |  73    49    I        |  105   69    i\n"
"  10    0A    LF  '\\n' (new line)        |  42    2A    *           |  74    4A    J        |  106   6A    j\n"
"  11    0B    VT  '\\v' (vertical tab)    |  43    2B    +           |  75    4B    K        |  107   6B    k\n"
"  12    0C    FF  '\\f' (form feed)       |  44    2C    ,           |  76    4C    L        |  108   6C    l\n"
"  13    0D    CR  '\\r' (carriage ret)    |  45    2D    -           |  77    4D    M        |  109   6D    m\n"
"  14    0E    SO  (shift out)            |  46    2E    .           |  78    4E    N        |  110   6E    n\n"
"  15    0F    SI  (shift in)             |  47    2F    /           |  79    4F    O        |  111   6F    o\n"
"  16    10    DLE (data link escape)     |  48    30    0           |  80    50    P        |  112   70    p\n"
"  17    11    DC1 (device control 1)     |  49    31    1           |  81    51    Q        |  113   71    q\n"
"  18    12    DC2 (device control 2)     |  50    32    2           |  82    52    R        |  114   72    r\n"
"  19    13    DC3 (device control 3)     |  51    33    3           |  83    53    S        |  115   73    s\n"
"  20    14    DC4 (device control 4)     |  52    34    4           |  84    54    T        |  116   74    t\n"
"  21    15    NAK (negative ack.)        |  53    35    5           |  85    55    U        |  117   75    u\n"
"  22    16    SYN (synchronous idle)     |  54    36    6           |  86    56    V        |  118   76    v\n"
"  23    17    ETB (end of trans. blk)    |  55    37    7           |  87    57    W        |  119   77    w\n"
"  24    18    CAN (cancel)               |  56    38    8           |  88    58    X        |  120   78    x\n"
"  25    19    EM  (end of medium)        |  57    39    9           |  89    59    Y        |  121   79    y\n"
"  26    1A    SUB (substitute)           |  58    3A    :           |  90    5A    Z        |  122   7A    z\n"
"  27    1B    ESC (escape)               |  59    3B    ;           |  91    5B    [        |  123   7B    {\n"
"  28    1C    FS  (file separator)       |  60    3C    <           |  92    5C    \\        |  124   7C    |\n"
"  29    1D    GS  (group separator)      |  61    3D    =           |  93    5D    ]        |  125   7D    }\n"
"  30    1E    RS  (record separator)     |  62    3E    >           |  94    5E    ^        |  126   7E    ~\n"
"  31    1F    US  (unit separator)       |  63    3F    ?           |  95    5F    _        |  127   7F    DEL\n";

#define WIN_ASCII_NAME "ASCII"

static int
ascii(struct nk_context *ctx, int show)
{
    int has_focus;
    int len;

    if (show)
    {
	nk_window_show(ctx, WIN_ASCII_NAME, NK_SHOWN);
	nk_window_set_focus(ctx, WIN_ASCII_NAME);
    }
    else
	return 0;

    if (nk_begin(ctx, WIN_ASCII_NAME, nk_rect(10, 10, 710, 565),
        NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|
	NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE))
    {
	nk_layout_row_dynamic(ctx, 520, 1);
	nk_edit_string(ctx,
		      NK_EDIT_EDITOR|NK_EDIT_READ_ONLY,
		      (char *)ascii_table,
		      &len,
		      sizeof(ascii_table),
		      nk_filter_ascii);
    }
    nk_end(ctx);

    if (nk_window_is_hidden(ctx, WIN_ASCII_NAME) || nk_window_is_closed(ctx, WIN_ASCII_NAME))
	return 0;

    return 1;
}
