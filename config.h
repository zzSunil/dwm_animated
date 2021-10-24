#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx = 2; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const unsigned int systraypinning =
    0; /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor
          X */
static const unsigned int systrayspacing = 2; /* systray spacing */
#define ICONSIZE 18   /* icon size */
#define ICONSPACING 6 /* space between icon and title */
static const int systraypinningfailfirst =
    1; /* 1: if pinning fails, display systray on the first monitor, False:
          display systray on the last monitor*/
static const int showsystray = 1;      /* 0 means no systray */
static const unsigned int gappih = 10; /* horiz inner gap between windows */
static const unsigned int gappiv = 10; /* vert inner gap between windows */
static const unsigned int gappoh =
    10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov =
    10; /* vert outer gap between windows and screen edge */
static const int smartgaps =
    0; /* 1 means no outer gap when there is only one window */
static const int showbar = 1;       /* 0 means no bar */
static const int topbar = 0;        /* 0 means bottom bar */
static const Bool viewontag = True; /* Switch view on tag switch */
static const char *fonts[] = {
    "Nerd Font:size=14:antialias=true:autohint=true",
    "Hack Nerd Font:size=14:antialias=true:autohint=true",
    "JoyPixels:size=14:antialias=true:autohint=true",
    "monospace:size=14:antialias=true:autohint=true",
    "Source Code Pro:size=14:antialias=true:autohint=true"};
static const char dmenufont[] = "SauceCodePro Nerd Font Mono:size=15";
// static const char *fonts[]          = { "monospace:size=16" };
// static const char dmenufont[]       = "monospace:size=13";
static const char col_gray1[] = "#3c3836";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#fbf1c7";
static const char col_gray4[] = "#d65d0e";
static const char col_gray5[] = "#504945";
static const char col_gruvred[] = "#cc241d";
static const char col_cyan[] = "#282828";
static const char col_border[] = "#cc241d";
static const unsigned int baralpha = 0xff;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {col_gray5, col_cyan, col_gray2},
    [SchemeShow] = {col_gray3, col_cyan, col_gray2},
    [SchemeSel] = {col_gray4, col_cyan, col_border},
    [SchemeHid] = {col_cyan, col_gray1, col_gray2},
    [SchemeSel2] = {col_gruvred, col_cyan, col_border},
};
static const unsigned int alphas[][3] = {
    /*               fg      bg        border     */
    [SchemeNorm] = {OPAQUE, baralpha, borderalpha},
    [SchemeSel] = {OPAQUE, baralpha, borderalpha},
    [SchemeShow] = {OPAQUE, baralpha, borderalpha},
    [SchemeHid] = {OPAQUE, baralpha, borderalpha},
    [SchemeSel2] = {OPAQUE, baralpha, borderalpha}};

/* tagging */
static const char *tags[] = {"", "", " ", " ", " ", " ", " ", "", " "};
static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Android Emulator", NULL, NULL, 0, 1, 0, -1},
    {"Emulator", NULL, NULL, 0, 1, 0, -1},
    {"quemu-system-i386", NULL, NULL, 0, 1, 0, -1},
    {"st", NULL, NULL, 0, 0, 1, -1},
    {"st", NULL, NULL, 0, 0, 1, -1},
    {"Gimp", NULL, NULL, 0, 1, 0, -1},
    {"Firefox", NULL, NULL, 1 << 8, 0, 0, -1},
};

/* layout(s) */
static const float mfact = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;   /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {" ", tile}, /* first entry is default */
    {" ", NULL}, /* no layout function means floating behavior */
    {"[M]", monocle},
    {"TTT", bstack},
    {"===", bstackhoriz},
    {"|M|", centeredmaster},
    {">M>", centeredfloatingmaster},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
    "dmenu_run", "-m",      dmenumon, "-fn",    dmenufont, "-nb",     col_gray1,
    "-nf",       col_gray3, "-sb",    col_cyan, "-sf",     col_gray4, NULL};
static const char *termcmd[] = {"alacritty", NULL};
static const char *browsercmd[] = {"firefox", NULL};

static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = {"st", "-t",     scratchpadname,
                                      "-g", "100x20", NULL};

static const char *upvol[] = {"/home/linus/scripts/vol-up.sh", NULL};
static const char *downvol[] = {"/home/linus/scripts/vol-down.sh", NULL};
static const char *mutevol[] = {"/home/linus/scripts/vol-toggle.sh", NULL};

static const char *wpcmd[] = {"/home/linus/scripts/wp-change.sh", NULL};
static const char *sktogglecmd[] = {"/home/linus/scripts/sck-tog.sh", NULL};

static const char *setcolemakcmd[] = {
    "/home/linus/scripts/setxmodmap-colemak.sh", NULL};
static const char *setqwertycmd[] = {"/home/linus/scripts/setxmodmap-qwerty.sh",
                                     NULL};
static const char *trayercmd[] = {"/home/linus/scripts/toggle-trayer.sh",
                                     NULL};
static const char *neteasecmd[] = {"/home/linus/scripts/neteasecmd.sh", NULL};

static const char *suspendcmd[] = {"/home/linus/scripts/suspend.sh", NULL};

static const char *screenshot[] = {"/home/linus/scripts/screenshot.sh", NULL};

static Key keys[] = {
    /* modifier            key                      function        argument */
    {MODKEY, XK_s, spawn, {.v = dmenucmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_c, spawn, {.v = browsercmd}},
    {MODKEY | ShiftMask, XK_t, spawn, {.v = trayercmd}},
    {MODKEY, XK_g, spawn, {.v = neteasecmd}},
    {MODKEY | ShiftMask, XK_p, spawn, {.v = suspendcmd}},
    {MODKEY | ControlMask, XK_s, spawn, {.v = sktogglecmd}},
    {MODKEY, XK_y, togglescratch, {.v = scratchpadcmd}},
    {0, XF86XK_AudioLowerVolume, spawn, {.v = downvol}},
    {0, XF86XK_AudioMute, spawn, {.v = mutevol}},
    {0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol}},
    {MODKEY, XK_bracketleft, spawn, {.v = downvol}},
    {MODKEY, XK_backslash, spawn, {.v = mutevol}},
    {MODKEY | ControlMask, XK_u, setlayout, {.v = &layouts[5]}},
    {MODKEY | ControlMask, XK_o, setlayout, {.v = &layouts[6]}},
    {MODKEY, XK_bracketright, spawn, {.v = upvol}},
    {MODKEY, XK_b, spawn, {.v = wpcmd}},
    {MODKEY, XK_z, spawn, {.v = screenshot}},
    {MODKEY | ShiftMask, XK_e, rotatestack, {.i = +1}},
    {MODKEY | ShiftMask, XK_u, rotatestack, {.i = -1}},
    {MODKEY, XK_e, focusstack, {.i = +1}},
    {MODKEY, XK_u, focusstack, {.i = -1}},
    {MODKEY, XK_n, viewtoleft, {0}},
    {MODKEY, XK_w, toggleMonitor, {0}},
    {MODKEY, XK_i, viewtoright, {0}},
    {MODKEY | ShiftMask, XK_n, tagtoleft, {0}},
    {MODKEY | ShiftMask, XK_i, tagtoright, {0}},
    {MODKEY | ShiftMask, XK_g, setlayout, {.v = &layouts[3]}},
    {MODKEY | ShiftMask, XK_o, setlayout, {.v = &layouts[4]}},
    {MODKEY | ShiftMask, XK_h, incnmaster, {.i = +1}},
    {MODKEY | ShiftMask, XK_l, incnmaster, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY, XK_k, hidewin, {0}},
    {MODKEY | ShiftMask, XK_k, restorewin, {0}},
    {MODKEY, XK_o, hideotherwins, {0}},
    {MODKEY | ShiftMask, XK_o, restoreotherwins, {0}},
    {MODKEY | ShiftMask, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY, XK_q, killclient, {0}},
    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY | ShiftMask, XK_f, fullscreen, {0}},
    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_apostrophe, togglescratch, {.v = scratchpadcmd}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8){MODKEY | ControlMask, XK_q, quit, {0}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument
     */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button1, togglewin, {0}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
