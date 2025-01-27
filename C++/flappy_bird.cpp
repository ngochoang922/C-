#include <iostream>
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
#endif
#include <string>
#include <thread>
#include <time.h>
#include <fstream>
#include <sstream>

#ifdef _WIN32
    #define configFileName  ".\\flappy.conf"
    #define scoreFileName   ".\\score.txt"
    #define soundMainmenu   ".\\sound\\mainmenu.wav"
    #define soundBirdFlyUp   ".\\sound\\birdFlyUp.wav"
    #define soundBirdDead   ".\\sound\\birdDead.wav"
#else
    #define configFileName  "./flappy.conf"
    #define scoreFileName   "./score.txt"
    #define soundMainmenu   "./sound/mainmenu.wav"
    #define soundBirdFlyUp   "./sound/birdFlyUp.wav"
    #define soundBirdDead   "./sound/birdDead.wav"
#endif

#ifdef _WIN32
    #define BLACK			0
    #define BLUE			1
    #define GREEN			2
    #define CYAN			3
    #define RED				4
    #define MAGENTA			5
    #define BROWN			6
    #define LIGHTGRAY		7
    #define DARKGRAY		8
    #define LIGHTBLUE		9
    #define LIGHTGREEN		10
    #define LIGHTCYAN		11
    #define LIGHTRED		12
    #define LIGHTMAGENTA	13
    #define YELLOW			14
    #define WHITE			15
#else
    #define BLACK         "\033[0;30m"
    #define RED           "\033[0;31m"
    #define GREEN         "\033[0;32m"
    #define BROWN         "\033[0;33m"
    #define YELLOW        "\033[1;33m"
    #define BLUE          "\033[0;34m"
    #define MAGENTA       "\033[0;35m"
    #define CYAN          "\033[0;36m"
    #define LIGHTGRAY	  "\033[0;37m"
    #define DARKGRAY	  "\033[1;30m"
    #define LIGHTRED      "\033[1;31m"
    #define LIGHTBLUE	  "\033[1;34m"
    #define LIGHTCYAN	  "\033[1;36m"
    #define LIGHTGREEN    "\033[1;32m"
    #define LIGHTMAGENTA  "\033[1;35m"
    #define WHITE         "\033[1;37m"
#endif

using namespace std;

string version_code = "1.0.0";
char defaultKeymapData[7][2] = {
    #ifdef _WIN32
        {0, 119},    // UP       'w'
        {0, 115},    // DOWN     's'
        {0, 97},    // LEFT      'a'
        {0, 100},    // RIGHT    'd'
        {0, 27},     // ESC      
        {0, 32},   // SPACE      ' '
        {0, 13}     // ENTER     '\r'
    #else
        #ifdef _TERMUX
            {118, 65},    // UP  
            {118, 66},    // DOWN  
            {118, 68},    // LEFT 
            {118, 67},    // RIGHT 
            {0, 27},     // ESC      
            {172, 126},   // SPACE  
            {171, 126}     // ENTER 
        #else
            {0, 119},    // UP   
            {0, 115},    // DOWN 
            {0, 97},    // LEFT   
            {0, 100},    // RIGHT
            {0, 27},     // ESC 
            {0, 32},   // SPACE    
            {0, 10}     // ENTER    
        #endif
    #endif
};
char keymapData[7][2];

int listHighScore[8];
int sizelistHighScore = sizeof(listHighScore) / sizeof(listHighScore[0]);

int settingsData[20] = {
    true,  // music
    true,  // sfx
    0, // brightness  // removed
    false,  // auto mode
    0,  // skin bird
    true,  // cmd mode v1/v2 (0/1)
    true,  // enable load conf
    true,  // enable color
    true,  // enable sound
    true,   // enable resolution
    true,   // enable auto mode
    true,   // enable skin
    true,   // show firework
    true,   // show background
    true,   // show X/Y
    0,      // skin wall
    4,      // number of firework 
    2,      // game speed
    true,   // show FPS
    2       // Difficult
};

#ifdef _WIN32
    int brightnessData = WHITE;
#else
    string brightnessData = WHITE;
#endif

int FPS = 0;
int frameFPS = 0;
int terminalColumns, terminalRows;
int tmp_int[3] = {0, 0, 0};
int aniShowMenu[3] = {-20, -20, -20};
int listWall[10][3];
int sizelistWall = sizeof(listWall) / sizeof(listWall[0]);
bool isInGame = false;
bool gameStarted = false;
#ifndef _WIN32  
    bool frameStarted = true;
#endif

string smallLogo = "";

#ifdef _WIN32
    HWND consoleWindow = GetConsoleWindow();
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    DWORD dwConsoleMode;
#else
    winsize bufferSize;
#endif

string backGround[5] = {
    // 
    // "                   /`\\             "
    // "  ___             /   \\   /```\\   "
    // " /   \\  __      /     \\_/     \\  "
    // "/     \\/  \\   /                \\_"
    // "            \\_/                    "
    //

    "                   /`\\\\             ",
    "  ___             /   \\\\   /```\\\\   ",
    " /   \\\\  __      /     \\\\_/     \\\\  ",
    "/     \\\\/  \\\\   /                \\\\_",
    "            \\\\_/                    "
};
int sizeBackground = sizeof(backGround) / sizeof(backGround[0]);

string skinWall[4][6] = {
    {
         "|  |", // up
         "|__|",
        "|____|",

        "|````|",  // down
         "|``|",
         "|  |"
    },
    {
         "|__|", // up
         "|__|",
        "/____\\",

        "\\````/",  // down
          "|``|",
          "|``|"
    },
    {
         "(__)", // up
         "(__)",
        "(____)",

        "(````)",  // down
         "(``)",
         "(``)"
    },
    {
         "{  }", // up
         "{__}",
        "(O_O )",

        "( O`O)",  // down
         "{``}",
         "{  }"
    }
};
int sizeSkinWall = sizeof(skinWall) / sizeof(skinWall[0]);

string skinFlyAnimation[3][4][3] = {
    {
        {
            "  /",
            ">@@(O>",
            "  \\"
        },
        {
            "  )",
            ">@@(O>",
            "  )"
        },
        {
            "  \\",
            ">@@(O>",
            "  /"
        },
        {
            " __",
            ">@@(O>",
            " ``"
        } 
    },
    {
        {
            " ( O>",
            "/ @ @\\",
            " ^ ^"
        },
        {
            " ( O>",
            "\\ @ @/",
            " ^ ^"
        },
        {
            " ( O>",
            "\\ @ @/",
            " ^ ^"
        },
        {
            " ( O>",
            "/ @ @\\",
            " ^ ^"
        }
    },
    {
        {
            "  ...",
            " | []|",
            "/.....\\"
        },
        {
            "  ...",
            " | []|",
            "/.....\\"
        },
        {
            "  ...",
            " | []|",
            "/.....\\"
        },
        {
            "  ...",
            " | []|",
            "/.....\\"
        }
    }
};

string skinDeadAnimation[3][3] = {
    {
        "",
        ">@@(*>",
        ""
    },
    {
        " ( *>",
        " |@ @|",
        " ^ ^"
    },
    {
        "  ...",
        " | XX|",
        "/.....\\"
    }
};

int sizeBirdAnimation = sizeof(skinFlyAnimation) / sizeof(skinFlyAnimation[0]);

void inputMenu(int *chooseMenu, int max, int type_menu);
void flappyBird();

void __sleep__(int s) {
    if (s < 1) {
        return;
    };
    #ifdef _WIN32
        Sleep(s);
    #else
        usleep((s * 1000) + 11000);
    #endif
    return;
};

#ifdef _WIN32
    int __getch__() {
        return _getch();   
    };
#else
    char __getch__() {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) {
            return 0;
        };
        return c;
    };
#endif

bool __kbhit__() {
    #ifdef _WIN32
        return _kbhit();
    #else
        struct timeval tv;
        fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds); // STDIN_FILENO is 0
        select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        return FD_ISSET(STDIN_FILENO, &fds);
    #endif
};

#ifndef _WIN32
    void set_terminal_mode() {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        return;
    };
    void reset_terminal_mode() {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag |= ICANON | ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    };
#endif

void cursorPos_move(int column, int row) {
    #ifdef _WIN32
        COORD tmpPos;
        tmpPos.X = column;
        tmpPos.Y = row;
        SetConsoleCursorPosition(hOutput, tmpPos);
    #else
        cout << "\033[" + to_string(row) + ";" + to_string(column) + "H";
    #endif
    return;
};

void cursorPos_up() {
    cursorPos_move(0, 0);
    return;
};

void clearTerminal() {
    int i;
    string line = "";
    string text = "";
    for(i = 0; i < terminalColumns; ++i) {
        line = line + ' ';
    };
    for(i = 0; i < terminalRows - 1; ++i) {
        text = text + line + "\n";
    };
    text = text + line;
    
    cursorPos_up();
    cout << text;
    cursorPos_up();
    return;
};

void hideCursor() {  // Windows API
    #ifdef _WIN32
        CONSOLE_CURSOR_INFO cursorInfo;

        cursorInfo.dwSize = 1;
        cursorInfo.bVisible = false;

        SetConsoleCursorInfo(hOutput, &cursorInfo);
    #else
        cout << "\033[?25l";
    #endif
    return;
};

void getTerminalSize(int *columns, int *rows) {  // Windows API
    #ifdef _WIN32
        GetConsoleScreenBufferInfo(hOutput, &bufferInfo);
        *columns = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
        *rows = bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1;
        if(!settingsData[5]) {
            *columns = *columns - 1;
        };
    #else
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &bufferSize);

        *columns = bufferSize.ws_col;
        *rows = bufferSize.ws_row;
    #endif
    return;
};

#ifdef _WIN32
    void color(int index) {  // Windows API
        if (settingsData[7]) {
            SetConsoleTextAttribute(hOutput, index);
        };
        return;
    };
#else
    void color(string textColor) {  // Windows API
        if (settingsData[7]) {
            cout << textColor;
        };
        return;
    };
#endif

void disableTouch() {  // Windows API
    #ifdef _WIN32
        GetConsoleMode(hInput, &dwConsoleMode);
        dwConsoleMode &= ~ENABLE_PROCESSED_INPUT;
        dwConsoleMode &= ~ENABLE_QUICK_EDIT_MODE;
        SetConsoleMode(hInput, dwConsoleMode);
    #else
    #endif
    return;
};

void disableCloseButton(bool isDisable) {  // WIndows API
    #ifdef _WIN32
        if (isDisable) {
            EnableMenuItem(GetSystemMenu(consoleWindow, FALSE), SC_CLOSE, MF_GRAYED);
        } else {
            EnableMenuItem(GetSystemMenu(consoleWindow, FALSE), SC_CLOSE, MF_ENABLED);
        };
    #else
    #endif
    return;
};

void disableMaximizeButton() {  // Windows API
    #ifdef _WIN32
        SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    #else
    #endif
    return;
};

void configureTerminal() {
    #ifdef _WIN32
        system("color 07 >NUL 2>&1"); // default color CMD
    #else
    #endif
    hideCursor();
    disableTouch();
    disableMaximizeButton();
    disableCloseButton(false);
    return;
};

void __getch(int p[2]) {
    p[0] = 0;
    p[1] = __getch__();
    #ifdef _WIN32
        if((p[1] == 0) || (p[1] == 224)) {
            p[1] = __getch__();
            p[0] = 1;
        };
    #else
        if((__kbhit__()) && (p[1] == 27)) {
            p[0] = p[0] + p[1];
            p[1] = __getch__();
            if ((__kbhit__()) && ((p[1] == 91) || (p[1] == 79))) {
                p[0] = p[0] + p[1];
                p[1] = __getch__();
                if ((__kbhit__()) && ((p[1] == 49) || (p[1] == 50) || (p[1] == 51) || (p[1] == 53) || (p[1] == 54))) {
                    p[0] = p[0] + p[1];
                    p[1] = __getch__();
                    if ((__kbhit__()) && ((p[1] == 48) || (p[1] == 53) || (p[1] == 55) || (p[1] == 56) || (p[1] == 57))) {
                        p[0] = p[0] + p[1];
                        p[1] = __getch__();
                    };
                };
            };
        };
    #endif
    return;
};

void flushStdin() {
    while (__kbhit__()) {
        __getch__();
    };
    return;
};

void pauseFrame() {
    #ifndef _WIN32
        if (!frameStarted) {
            while(true) {
                if (frameStarted) {
                    flushStdin();
                    return;
                };
                __sleep__(250);
            };
        };
    #endif
    return;
};

void anyKey() {
    flushStdin();
    __getch__();
    flushStdin();
    return;
};

void playSound(string file, bool isSFX) {  // Windows API
    if (settingsData[8]) {
        if (isSFX) {
            if(settingsData[1]) {
                #ifdef _WIN32
                    PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME|SND_ASYNC);
                #else
                #endif
            };
        } else {
            if(settingsData[0]) {
                #ifdef _WIN32
                    PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
                #else
                #endif
            };
        };
    };
    return;
};

void stopSound() {
    if (settingsData[8]) {
        #ifdef _WIN32
            PlaySound(NULL, 0, 0);
        #else
        #endif
    };
    return;
};

string readFile(string fileName) {
    ifstream f(fileName);
    string text = "";
    string line;
    if (f.is_open()) {
        while (getline(f, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            };
            text = text + line + "\n";
        };
        f.close();
    };
    return text;
};

void writeFile(string fileName, string data) {
    ofstream f(fileName);
    if (f.is_open()) {
        f << data;
        f.close();
    };
};

void writeConfig(string key, string value) {
    if (settingsData[6]) {
        string fileData = readFile(configFileName);
        istringstream file(fileData);
        string line;
        string key_;
        string value_;
        ofstream f(configFileName);
        if (f.is_open()) {
            f << key << "=" << value << "\n";
            while (getline(file, line)) {
                if (line.empty() || line[0] == '#') {
                    continue;
                };
                istringstream iss(line);
                if (getline(iss, key_, '=') && (key_ == key)) {
                    continue;
                } else {
                    getline(iss, value_);
                    f << key_ << "=" << value_ << "\n";
                };
            };
            f.close();
        };
    };
    return;
};

void resetKeymapData(bool isWriteConfig) {
    int i, j;
    for(i = 0; i < sizeof(defaultKeymapData) / sizeof(defaultKeymapData[0]); ++i) {
        for(j = 0; j < sizeof(defaultKeymapData[0]) / sizeof(defaultKeymapData[0][0]); ++j) {
            keymapData[i][j] = defaultKeymapData[i][j];
            if (isWriteConfig) {
                writeConfig("key" + to_string(i), "-1");
            };
        };
    };
    return;
};

void titleTerminal(string name) {
    #ifdef _WIN32
        SetConsoleTitle((LPCTSTR)name.c_str());
    #else
        cout << "\033]0;" + name + "\007";
    #endif
    return;
};

void showBlur() {
    int i, j;
    #ifdef _WIN32    
        do {
            i = rand() % 15;
        } while ((i == BLACK) || (i == RED));
        color(i);
    #else
        color(RED);
    #endif
    
    for(i = 0; i < terminalRows; ++i) {
        for(j = 0; j < terminalColumns; j = j + 5) {
            cursorPos_move(j, i);
            cout << "-";
        };
    };
    color(brightnessData);
    return;
};

void bottomKeymap(string text) {
    string lineLastTer = "";
    int i, j;
    for(j=0; j < terminalColumns; ++j) {
        lineLastTer = lineLastTer + "_";
    };
    lineLastTer = lineLastTer + "\n";
    i = text.length();
    for(j = 0; j < terminalColumns - i; ++j) {
        text = text + ' ';
    };
    while (text.length() > terminalColumns) {
        text.pop_back();
    };
    #ifdef _WIN32
        cursorPos_move(0, terminalRows - 2);
    #else
        cursorPos_move(0, terminalRows - 1);
    #endif
    color(WHITE);
    cout << lineLastTer;
    color(GREEN);
    cout << text;
    #ifndef _WIN32
        cout << flush;
    #endif
    color(brightnessData);
    return;
};

void showFPS(string output[]) {
    if (!settingsData[18]) {
        return;
    };
    string text = "FPS: " + to_string(FPS);
    int i;
    int locate = terminalColumns - (text.length());
    if (output == NULL) {
        color(brightnessData);
        cursorPos_move(locate - 1, 0);
        cout << " " << text;
        #ifndef _WIN32
            cout << flush;
        #endif
    } else {
        for(i = 0; i < text.length(); ++i) {
            output[0][locate + i] = text[i];
        };
    };
    frameFPS = frameFPS + 1;
    return;
};

void errorBox(string output, string bottom, bool isBlur) {
    //
    //     ________________________   .
    //    /                        \ /
    //   /      Not available       /
    //  / \                        /
    // `   ````````````````````````
    //

    int i, j;
    int boxSize = 26;
    int sizeColumn = 0;
    int sizeRow;

    if(bottom == "") {
        bottom = "Press any key to continue!";
    };
    
    for(i = 0; i < (terminalColumns - boxSize) / 2; ++i) {
        sizeColumn = sizeColumn + 1;
    };
    string text;

    j = output.length();
    if (j > 20) {
        text = "";
        for(i = 0; i < 20; ++i) {
            text = text + output[i];
        };
        output = text; 
    };

    if (isBlur) {
        showBlur();
    };

    while(true) {
        pauseFrame();
        showFPS(NULL);
        color(RED);
        sizeRow = (terminalRows / 2) - 3;
        text = "";
        cursorPos_move(sizeColumn, sizeRow);

        text = text + ' ';
        for(j = 0; j < boxSize - 2; ++j) {
            text = text + "_";
        };
                
        text = text + "   /";
        cout << text;

        text = "";
        cursorPos_move(sizeColumn, sizeRow + 1);

        text = text + "/ ";
        for(j = 0; j < boxSize - 4; ++j) {
            text = text + ' ';
        }; 
        text = text + " \\ /";
        cout << text;

        text = "";
        cursorPos_move(sizeColumn - 1, sizeRow + 2);
        text = text + "/ ";
        for(j = 0; j < boxSize - 2 - (output.length() ); ++j) {
            if (j == ((boxSize - 2) - output.length() ) / 2) {
                text = text + ' ' + output;
            } else {
                text = text + ' ';
            };
        };
        text = text + " /";
        cout << text;

        text = "";
        cursorPos_move(sizeColumn - 2, sizeRow + 3);
        text = text + "/ \\ ";
        for(j = 0; j < boxSize - 4; ++j) {
            text = text + ' ';
        }; 
        text = text + " /";
        cout << text;

        text = "";
        cursorPos_move(sizeColumn - 3, sizeRow + 4);

        text = text + "/   ";
        for(j = 0; j < boxSize - 2; ++j) {
            text = text + "`";
        };
                
        text = text + ' ';
        cout << text;

        bottomKeymap(bottom);
        if(__kbhit__()) {
            flushStdin();
            return;
        };
        __sleep__(200);
    };
    return;
};

void showOverlayResolution() {
    color(brightnessData);
    string resolutionString = to_string(terminalColumns) + " x " + to_string(terminalRows);
    string text = "";
    string text2 = "";
    int i, j;
    for(i = 0; i < terminalColumns; ++i) {
        text = text + "=";
    };

    text2 = text2 + "| +-";
    for(i = 0; i < resolutionString.length(); ++i) {
        text2 = text2 + "-";
    };
    text2 = text2 + "-+\n| | " + resolutionString + " |\n| +-";
    for(i = 0; i < resolutionString.length(); ++i) {
        text2 = text2 + "-";
    };
    text2 = text2 + "-+";

    color(brightnessData);
    for(i = 0; i < terminalRows; ++i) {
        if (i == 0) {
            cursorPos_move(0, i);
            cout << text;
        } else if (i == terminalRows - 1) {
            cursorPos_move(0, i);
            cout << text;
        } else {
            if (i == 1) {
                cursorPos_move(0, i);
                cout << text2;
            };
            cursorPos_move(0, i);
            cout << "|";
            cursorPos_move(terminalColumns - 1, i);
            cout << "|";
        };
    };
    return;
};

void showBoxText(string text, bool isBlur) {
    int columns = terminalColumns;
    int rows = terminalRows;
    #ifndef _WIN32
        if (!frameStarted) {
            columns = tmp_int[0];
            rows = tmp_int[1];
        };
    #endif

    int sizeColumn = 0;
    int sizeRow = ((rows / 2) - 3);
    if (sizeRow < 0) {
        sizeRow = 0;
    };
    int i, j;
    string tmp;
    if (text.length() > (columns - 16)) {
        tmp = "";
        for(i = 0; i < (columns - 16); ++i) {
            tmp = tmp + text[i];  
        };
        text = tmp;
    };
    for(i = 0; i < (columns - text.length()) / 2; ++i) {
        sizeColumn = sizeColumn + 1;
    };
    sizeColumn = sizeColumn - 1;

    if (isBlur) {
        showBlur();
    };

    color(YELLOW);
    cursorPos_move(sizeColumn, sizeRow);
    tmp = "  ";
    for(j = 0; j < text.length() + 2; ++j) {
        tmp = tmp + "_";
    };
    cout << tmp;

    cursorPos_move(sizeColumn, sizeRow + 1);
    tmp = " | ";
    for(j = 0; j < text.length(); ++j) {
        tmp = tmp + ' ';
    };
    cout << tmp << " |";

    cursorPos_move(sizeColumn, sizeRow + 2);
    cout << " | " << text << " |";

    cursorPos_move(sizeColumn, sizeRow + 3);
    tmp = " | ";
    for(j = 0; j < text.length(); ++j) {
        tmp = tmp + ' ';
    };
    cout << tmp << " |";

    cursorPos_move(sizeColumn, sizeRow + 4);
    tmp = "  ";
    for(j = 0; j < text.length() + 2; ++j) {
        tmp = tmp + "`";
    };
    cout << tmp;
    return;
};

void resizeTerminal(int column, int row) {
    if (!settingsData[3]) {
        gameStarted = false;
    };
    string cmd;
    if (settingsData[9]) {
        #ifdef _WIN32
            if(!settingsData[5]) {
                column = column + 1;
            };
            cmd = "MODE " + to_string(column) + "," + to_string(row) + " >NUL 2>&1";
            if (system(cmd.c_str()) != 0) {
                clearTerminal();
                errorBox("API ERROR", "Please run on real Windows!", false);
                exit(1);
            };
            showOverlayResolution();
        #else
            frameStarted = false;
            while(true) {
                if ((column >= terminalColumns) && (row >= terminalRows)) {
                    system("clear");
                    frameStarted = true;
                    break;
                };
                getTerminalSize(&column, &row);
                tmp_int[0] = column;
                tmp_int[1] = row;
                color(YELLOW);
                showBoxText("Resize [" + to_string(column) + " x " + to_string(row) + "] to [" + to_string(terminalColumns) + " x " + to_string(terminalRows) + "]", false);
                __sleep__(500);
            };
        #endif
    };
    // if (settingsData[5]) {
    //     cmd = "Enabled";
    // } else {
    //     cmd = "Disabled";
    // };
    titleTerminal("Flappy Bird - KhanhNguyen9872 - C++  |  Res: " + to_string(terminalColumns) + " x " + to_string(terminalRows));
    return;
};

void setResolution(int value) {
    switch(value) {
        case 1:
            terminalColumns = 100;
            terminalRows = 26;
            break;
        case 2:
            terminalColumns = 120;
            terminalRows = 30;
            break;
        case 3:
            terminalColumns = 140;
            terminalRows = 36;
            break;
        case 4:
            terminalColumns = 160;
            terminalRows = 40;
            break;
        #ifdef _TERMUX
            case 5:
                terminalColumns = 113;
                terminalRows = 20;
                break;
        #endif
        default:
            terminalColumns = 80;
            terminalRows = 20;
            if (value != -1) {
                value = 0;
            };
            break;
    };
    smallLogo = "";
    if (value != -1) {
        writeConfig("resolution", to_string(value));
    };
    resizeTerminal(terminalColumns, terminalRows);
    return;
};

int getResolutionValue() {
    if ((terminalColumns == 80) && (terminalRows == 20)) {
        return 0;
    } else if ((terminalColumns == 100) && (terminalRows == 26)) {
        return 1;
    } else if ((terminalColumns == 120) && (terminalRows == 30)) {
        return 2;
    } else if ((terminalColumns == 140) && (terminalRows == 36)) {
        return 3;
    } else if ((terminalColumns == 160) && (terminalRows == 40)) {
        return 4;
    } else {
        #ifdef _TERMUX
            if ((terminalColumns == 113) && (terminalRows == 20)) {
                return 0;
            };
        #endif
        setResolution(0);
        return 0;
    };
};

int readConfig(string key) {
    if (settingsData[6]) {
        string fileData = readFile(configFileName);
        string line;
        string key_;
        string value_ = "0";
        istringstream file(fileData);
        try {
            while (getline(file, line)) {
                if (line.empty() || line[0] == '#') {
                    continue;
                };
                istringstream iss(line);
                if (getline(iss, key_, '=') && (key_ == key)) {
                    if (getline(iss, value_)) {
                        return stoi(value_);
                    } else {
                        writeConfig(key, "-1");
                        return -1;
                    };
                };
            };
            return stoi(value_);
        } catch (...) {
            writeConfig(key, "-1");
            return -1;
        };
    };
    return -1;
};

void showUser(string username) {
    // 
    // +-----------------------+
    // | User: KhanhNguyen9872 |
    // +-----------------------+
    //
    int i;
    int j = 5 * getResolutionValue();
    int sizeColumn = 0;

    string text;

    if (username.length() > 10 + j) {
        text = "";
        for(i = 0; i < 10 + j; ++i) {
            if (i >= username.length()) {
                break;
            };
            text = text + username[i];
        };

        if (i >= username.length()) {
            username = text;
        } else {
            username = text + "...";
        };
    };

    username = "User: " + username;

    text = "+-";
    for(i = 0; i < username.length(); ++i) {
        text = text + "-";
    };
    text = text + "-+";

    for(i = 0; i < terminalColumns; ++i) {
        if (i == terminalColumns - 7 - username.length()) {
            break;
        } else {
            sizeColumn = sizeColumn + 1;
        };
    };

    #ifdef _WIN32
        int sizeRow = 1;
    #else
        int sizeRow = 2;
    #endif

    cursorPos_move(sizeColumn, sizeRow);
    cout << text;

    cursorPos_move(sizeColumn, sizeRow + 1);
    cout << "| " << username << " |";

    cursorPos_move(sizeColumn, sizeRow + 2);
    cout << text;
    return;
};

string centerText(string text[], int size) {
    string finalText = "";
    string lineSpace = "";
    int Tmp, i;

    Tmp = (terminalColumns - text[0].length()) / 2;
    for(i=0; i < Tmp; ++i) {
        lineSpace = lineSpace + ' ';
    };

    for(i=0; i < size; ++i) {
        finalText = finalText + lineSpace + text[i] + "\n";  
    };
    return finalText;
};


void showLogoFullTerminal(string logo[], int sizeLogo, bool isClear, bool isShowUser) {
    //
    //   _|  |                                   |     _)           | 
    //  |    |   _` |  __ \   __ \   |   |       __ \   |   __|  _` | 
    //  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | 
    // _|   _| \__,_|  .__/   .__/  \__, |      _.__/  _| _|   \__,_| 
    //                _|     _|     ____/                             
    //                                            By KhanhNguyen9872  
    int i;
    string text = "";

    for(i=0; i <= terminalRows; ++i) {
        if(i == (terminalRows/2) - ((sizeLogo) / 2)) {
            text = text + centerText(logo, sizeLogo);
            break;
        } else {
            text = text + "\n";
        };
    };

    bool _i = false;
    clearTerminal();
    if(isShowUser) {
        color(WHITE);
        #ifdef _WIN32
            showUser(getenv("username"));
        #else
            string user;
            if (getenv("USER")) {
                user = getenv("USER");
            } else {
                user = "root";
            };
            showUser(user);
        #endif
    };
    for(i=0;i < 40; ++i) {
        if (i<5) {
            color(BLACK);
        } else if(i<8) {
            color(DARKGRAY);
        } else if (i<10) {
            color(LIGHTGRAY);
        } else if (i>36) {
            color(DARKGRAY);
        } else if (i>38) {
            color(LIGHTGRAY);
        } else {
            if(!_i) {
                color(WHITE);
                _i = true;
            }
        };
        pauseFrame();
        if (isClear) {
            clearTerminal();
        } else {
            cursorPos_up();
        };
        cout << text;
        showFPS(NULL);
        __sleep__(100);
    };
    clearTerminal();
    return;
};

bool showYesorNo(string text) {
    //
    //   ____________________
    //  |                    |
    //  | Exit to main menu? |
    //  |                    |
    //   ````````````````````
    //
    int p[2];
    bool tmp = true;

    flushStdin();
    while(true) {
        pauseFrame();
        showBoxText(text, tmp);
        #ifdef _TERMUX
            bottomKeymap("| [/] -> YES | [-] -> NO |");
        #else
            bottomKeymap("| [y] -> YES | [n] -> NO |");
        #endif

        if (__kbhit__()) {
            __getch(p);

            #ifdef _TERMUX
                if ((p[1] == '/') && (!p[0])) {
                    return 1;
                } else if ((p[1] == '-') && (!p[0])) {
                    return 0;
                };
            #else
                if (((p[1] == 'y') || (p[1] == 'Y')) && (!p[0])) {
                    return 1;
                } else if (((p[1] == 'n') || (p[1] == 'N')) && (!p[0])) {
                    return 0;
                };
            #endif
        };
        tmp = false;
        __sleep__(200);
    };

    return 0;
};

void banner() {
    string logo[6] = { \
        "   _|  |                                   |     _)           | ", \
        "  |    |   _` |  __ \\   __ \\   |   |       __ \\   |   __|  _` | ", \
        "  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | ", \
        " _|   _| \\__,_|  .__/   .__/  \\__, |      _.__/  _| _|   \\__,_| ", \
        "                _|     _|     ____/                             ", \
        "                                             By KhanhNguyen9872  "};
    showLogoFullTerminal(logo, sizeof(logo)/sizeof(logo[0]), true, false);

    string logo2[15] = { \
        "      .!P#&&&&&&&#P!.      ", \
        "    ~B&#Y~:.   .:~Y#@B~    ", \
        "  :&@5.             .Y@&^  ", \
        " 7@#.                 .B@? ", \
        "~@#                     B@!", \
        "&@:                     .@&", \
        "@@                       @@", \
        "@@~B@&#&&:       .&&#&@B~@@", \
        "@@#@@: B@^       ^@B :@@#@@", \
        "@@ #@. G@^       ^@G .@& &@", \
        "@@ #@. G@^       ^@G .@& @@", \
        "&@#@@: G@^       ^@B .@@#@&", \
        " .~&@@&@@:       :@@&@@&~. ", \
        "                           ", \
        "   Do not use headphones   ", \
    };
    showLogoFullTerminal(logo2, sizeof(logo2)/sizeof(logo2[0]), false, true);

    showFPS(NULL);
    __sleep__(1000);
    return;
};

void showTip(string tip) {
    if (tip.length() >= terminalColumns - 18) {
        return;
    };
    if (tip == "") {
        string listTip[6] = {
            "Fact: The person who doesn't play is the winner!",
            "Tip: You can change the brightness to be more suitable!",
            "Fact: The game was developed by KhanhNguyen9872!",
            "Tip: Reduce resolution for best performance!",
            "Tip: You can change the key according to your preference!",
            "Tip: Try Auto mode for fun!"
        };
        
        int p = rand() % (sizeof(listTip) / sizeof(listTip[0]));
        tip = listTip[p];
    };
    int i;
    string text = "";
    for(i = 0; i < terminalColumns - 16; ++i) {
        text = text + ' ';
    };

    cursorPos_up();
    color(YELLOW);
    int sizeRow = (terminalRows - 3);
    cursorPos_move(0, sizeRow);
    cout << text;
    cursorPos_move(3, sizeRow);
    cout << tip;
    return;
};

void showAnimation(string output[], string animation[], int sizeAnimation, int countUp) {
    bool noSpace;
    int i, j, m;
    int sizeRow = (terminalRows / 4) - countUp;
    int l = terminalRows - 3;
    for(j = 0; j < sizeAnimation; ++j) {
        if (output == NULL) { // print direct to terminal
            #ifdef _WIN32
                cursorPos_move(3, sizeRow + j);
            #else
                cursorPos_move(4, sizeRow + j);
            #endif
            cout << animation[j];
        } else { // insert data to output array
            noSpace = 0;
            for(m = 0; m < animation[j].length(); m++) {
                if (!noSpace) { // remove space char in first string
                    if (animation[j][m] == ' ') {
                        continue;
                    } else {
                        noSpace = 1;
                    };
                };
                if ((sizeRow + j) < l) {
                    output[sizeRow + j][3 + m] = animation[j][m];
                };
            };
        };
    };
    return;
};

void showChangeScene() {
    int i;
    string text = "";
    string text2 = "";
    string listText[10] = {
        "=",
        "/",
        "-",
        "@",
        "+",
        "\\",
        "|",
        "#",
        "%",
        "*"
    };
    string p = listText[rand() % (sizeof(listText) / sizeof(listText[0]))];

    for(i = 0; i < terminalColumns; ++i) {
        text = text + p;
        text2 = text2 + ' ';
    };

    bool randomVar = rand() % 1;

    int k = 5 - getResolutionValue();
    
    color(brightnessData);
    __sleep__(400);
    if (randomVar) {
        cursorPos_up();
        for(i = 0; i < terminalRows; ++i) {
            cout << text;
            #ifndef _WIN32
                cout << flush;
            #endif
            __sleep__(k);
        };

        __sleep__(150);
        cursorPos_up();
        for(i = 0; i < terminalRows; ++i) {
            cout << text2;
            #ifndef _WIN32
                cout << flush;
            #endif
            __sleep__(k);
        };
    } else {
        for(i = terminalRows - 1; i >= 0; --i) {
            cursorPos_move(0, i);
            cout << text;
            #ifndef _WIN32
                cout << flush;
            #endif
            __sleep__(k);
        };

        __sleep__(150);
        for(i = terminalRows - 1; i >= 0; --i) {
            cursorPos_move(0, i);
            cout << text2;
            #ifndef _WIN32
                cout << flush;
            #endif
            __sleep__(k);
        };
    };

    __sleep__(250);
    return;
};

bool checkTerminalActive() {  // Windows API
    #ifdef _WIN32
        return consoleWindow == GetForegroundWindow();
    #else
        return true;
    #endif
};

void exitProgram() {
    if (showYesorNo("Do you want to exit?")) {
        showChangeScene();
        __sleep__(500);
        #ifndef _WIN32
            reset_terminal_mode();
            system("clear");
        #endif
        exit(0);
    };
    return;
};

string getRoad() {
    int i;
    string text = "[";
    for(i = 0; i < terminalColumns - 2; ++i) {
        text = text + "/";
    };
    text = text + "]";
    return text;
};

string getOutput(string output[], int sizeOutput) {
    if (output == NULL) {
        return "";
    };
    int i;
    int k = terminalRows - 2;
    string fullOutput = "";
    for(i = 0; i < sizeOutput; ++i) {
        fullOutput = fullOutput + output[i];
        if (i != sizeOutput - 1) {
            fullOutput = fullOutput + "\n";
        };
    };
    return fullOutput;
};

void wipeOutput(string output[], int sizeOutput) {
    if (output == NULL) {
        return;
    };
    string lineBlank = "";
    int i;
    for(i = 0; i < terminalColumns; ++i) {
        lineBlank = lineBlank + ' ';
    };
    for(i = 0; i < sizeOutput; ++i) {
        output[i] = lineBlank;
    };
    return;
};

void showBackground(string output[], int countStart, int maxRow) {
    string text;
    int i, j, row, count;
    j = maxRow - sizeBackground;
    for(row = 0; row < sizeBackground; row++) {
        count = countStart;
        text = "";
        for(i = 0; i < terminalColumns; ++i) {
            if (output == NULL) { // print directly (slow performance)
                text = text + backGround[row][count];
            } else { // add to Output array
                output[j + row][i] = backGround[row][count];
            };
            count = count + 1;
            if(count > backGround[row].length() - 1) {
                count = 0;
            };
        };
        if (output == NULL) {
            cursorPos_move(0, j + row);
            cout << text;
        };
    };
    return;
};

void loadingFrame(int progress, bool isShowBird) {
    //
    //   __________
    //  |==        |
    //   ``````````
    //   LOADING...
    //

    // 
    // 
    //   ( O>   |    \         | 
    //  / @ @\  |   >@@( O>    | 
    //   ^ ^    |    /         | 
    //
    //
    //
    // [/////////////////////////////////////////]

    pauseFrame();

    int sizeAnimation = sizeof(skinFlyAnimation[settingsData[4]]) / sizeof(skinFlyAnimation[settingsData[4]][0]);
    int i, j, k;

    string text = "";
    int sizeRow = terminalRows-5;
    int sizeColumn = terminalColumns - 15;

    color(LIGHTCYAN);

    cursorPos_move(sizeColumn, sizeRow);
    cout << " __________ ";

    cursorPos_move(sizeColumn, sizeRow + 1);
    text = "|";
    for(k = 10; k <= 100; k = k + 10) {
        if(progress >= k) {
            text = text + "=";
        } else {
            text = text + ' ';
        };
    };
    text = text + "|";
    cout << text;

    cursorPos_move(sizeColumn, sizeRow + 2);
    cout << " `````````` ";

    cursorPos_move(sizeColumn, sizeRow + 3);
    cout << " LOADING";

    text = "";
    for(k=0; k < tmp_int[0]; ++k) {
        text = text + ".";
    };
    for(k=0; k < 4 - tmp_int[0]; ++k) {
        text = text + ' ';
    };
    cout << text + "\n";

    tmp_int[0] = tmp_int[0] + 1;
    if(tmp_int[0] > 4) {
        tmp_int[0] = 0;
    };

    if(isShowBird) {
        color(CYAN);
        text = getRoad();
        if (settingsData[13]) {
            showBackground(NULL, 0, terminalRows - 6);
        };
        showAnimation(NULL, skinFlyAnimation[settingsData[4]][tmp_int[1]], sizeof(skinFlyAnimation[settingsData[4]][tmp_int[1]]) / sizeof(skinFlyAnimation[settingsData[4]][tmp_int[1]][0]), 0);
    
        cursorPos_move(0, terminalRows - 6);
        cout << text;

        if ((tmp_int[1] >= 3) || (tmp_int[1] <= 0) ) {
            tmp_int[2] = !tmp_int[2];
        };

        if(!tmp_int[2]) {
            tmp_int[1] = tmp_int[1] + 1;
        } else {
            tmp_int[1] = tmp_int[1] - 1;
        };
    };

    showFPS(NULL);

    if (progress >= 100) {
        showChangeScene();
    };

    return;
};

string menuText(string text[], int size, int type_menu, int choose) {
    //      
    // =>> |   Start  | <<=
    //     | Settings |
    //     |  Credit  |
    //     |   Exit   |
    //

    string finalString = "";
    string lineSpace = "";
    string text2;
    int i, j;

    // animation
    string textLeft = "";
    string textRight = "";
    if ((type_menu != aniShowMenu[0]) || (choose != aniShowMenu[1])) {
        aniShowMenu[0] = type_menu;
        aniShowMenu[1] = choose;
        aniShowMenu[2] = 0;
    };

    switch(aniShowMenu[2]) {
        case 0:
            textLeft = ">    |  ";
            textRight = "  |    <";
            break;
        case 1:
            textLeft = " >   |  ";
            textRight = "  |   < ";
            break;
        case 2:
            textLeft = "  >  |  ";
            textRight = "  |  <  ";
            break;
        case 3:
            textLeft = "   > |  ";
            textRight = "  | <   ";
            break;
        case 4:
            textLeft = "    >|  ";
            textRight = "  |<    ";
            break;
        case 5:
            textLeft = "    |>  ";
            textRight = "  <|    ";
            break;
        case 6:
            textLeft = "   |=>  ";
            textRight = "  <=|   ";
            break;
        case 7:
            textLeft = "  |==>  ";
            textRight = "  <==|  ";
            break;
        case 8:
            textLeft = " -|==>  ";
            textRight = "  <==|- ";
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            textLeft = " -|==>  ";
            textRight = "  <==|- ";
            break;
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
            textLeft = "-|==>   ";
            textRight = "   <==|-";
            break;
        default:
            break;
    };
    if(aniShowMenu[2] < 9) {
        aniShowMenu[2] = aniShowMenu[2] + 1;
    } else if(aniShowMenu[2] < 28) {
        aniShowMenu[2] = aniShowMenu[2] + 1;
    } else {
        aniShowMenu[2] = 9;
    };

    //
    int k = 0;
    for(i = 0; i < size; ++i) {
        if (text[i].length() > k) {
            k = text[i].length();
        };
    };

    for(i = 0; i < ((terminalColumns - k) / 2) - 6; ++i) {
        lineSpace = lineSpace + ' ';
    };
    
    for(i = 0; i < size; ++i) {
        text2 = "";
        for(j = 0; j < (k - text[i].length()); ++j) {
            text2 = text2 + ' ';
        };
        if(i == choose) {
            finalString = finalString + lineSpace + textLeft + text[i] + text2 + textRight + "\n";
        } else {
            finalString = finalString + lineSpace + "     |  " + text[i] + text2 + "  |     " + "\n";
        };
    };
    return finalString;
};

void lockSizeTerminal() {
    if (!settingsData[9]) {
        return;
    };
    int columns = 0, rows = 0;
    while(true) {
        getTerminalSize(&columns, &rows);
        #ifdef _WIN32
            if ((columns != terminalColumns) || (rows != terminalRows)) {
                resizeTerminal(terminalColumns, terminalRows);
                disableMaximizeButton();
            };
        #else
        if ((columns < terminalColumns) || (rows < terminalRows)) {
            resizeTerminal(columns, rows);
        };
        #endif
        __sleep__(300);
    };
    return;
};

string getNameKey(short value, int isTwoChar) {
    if (isTwoChar) {
        switch(value) {
            #ifdef _WIN32
                case 59:
                case 60:
                case 61:
                case 62:
                case 63:
                case 64:
                case 65:
                case 66:
                case 67:
                case 68:
                    return "F" + to_string(value - 58);
                case 71:
                    return "HOME";
                case 79:
                    return "END";
                case 82:
                    return "INSER";
                case 83:
                    return "DEL";
                case 73:
                    return "PG UP";
                case 81:
                    return "PG DN";
                case 72:
                    return "UP";
                case 80:
                    return "DOWN";
                case 75:
                    return "LEFT";
                case 77:
                    return "RIGHT";
            #else
                case 80:
                case 81:
                case 82:
                case 83:
                    switch(isTwoChar) {
                        case 79:
                            return "F" + to_string(value - 79);
                    };
                case 65:
                    switch(isTwoChar) {
                        case 118:
                            return "UP";
                    };
                case 66:
                    switch(isTwoChar) {
                        case 118:
                            return "DOWN";
                    };
                case 67:
                    switch(isTwoChar) {
                        case 118:
                            return "RIGHT";
                    };
                case 68:
                    switch(isTwoChar) {
                        case 118:
                            return "LEFT";
                    };
                case 70:
                    switch(isTwoChar) {
                        case 118:
                            return "END";
                    };
                case 72:
                    switch(isTwoChar) {
                        case 118:
                            return "HOME";
                    };
                case 126:   // 51
                    switch(isTwoChar) {
                        case 169: // 27 + 91 + 51
                            return "DEL";
                        case 168:
                            return "INSERT";
                        case 171:
                            return "PG UP";
                        case 172:
                            return "PG DN";
                        case 220:
                            return "F5";
                        case 222:
                            return "F6";
                        case 223:
                            return "F7";
                        case 224:
                            return "F8";
                        case 216:
                            return "F9";
                        case 217:
                            return "F10";
                        default:
                            return "NULL";
                    };
            #endif
            default:
                return "NULL";
        };
    } else {
        switch(value) {
            case 9:
                return "TAB";
            case 27:
                return "ESC";
            case 32:
                return "SPACE";

            #ifdef _WIN32
                case 8:
                    return "BACKS";
                case 13:
                    return "ENTER";
            #else
                case 10:
                    return "ENTER";
                case 127:
                    return "BACKS";
            #endif
            default:
                break;
        };
    };
    return string(1, value);
};

bool setKeymap(int value, int key, int isTwoChar) {
    #ifdef _WIN32
        int keyAllow[5] = {
            8, 9, 13, 27, 32
        };
    #else
        int keyAllow[5] = {
            9, 10, 27, 32, 127
        };
    #endif
    int i;
    if(isTwoChar) {
        if (getNameKey(key, isTwoChar) != "NULL") {
            keymapData[value][0] = isTwoChar;
            keymapData[value][1] = key;
            return 1;
        };
    } else if (key > 7) {
        for(i = 0; i < sizeof(keyAllow) / sizeof(keyAllow[0]); ++i) {
            if (key == keyAllow[i]) {
                keymapData[value][0] = 0;
                keymapData[value][1] = key;
                return 1;
            };
        };
        if ((key >= ' ') && (key <= '~') ) {
            keymapData[value][0] = 0;
            keymapData[value][1] = key;
            return 1;
        };
    };
    return 0;
};

void stringToOutput(string str, string output[], int sizeOutput) {
    int size = str.length();
    int row = 0;
    int i; 
    int column = 0;
    for(i = 0; i < size; ++i) {
        if (row >= sizeOutput) {
            return;
        };
        if (str[i] == '\n') {
            column = 0;
            row = row + 1;
            continue;
        };
        output[row][column] = str[i];
        column = column + 1;
    };
    return;
};

void showMenu(string titleMenu, string* menu, int sizeMenu, int type_menu, int *chooseMenu, string addTextBottom, bool isFullTextBottom) {
    //   ___ _                       ___ _        _ 
    //  | __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |
    //  | _|| / _` | '_ \ '_ \ || | | _ \ | '_/ _` |
    //  |_| |_\__,_| .__/ .__/\_, | |___/_|_| \__,_|
    //             |_|  |_|   |__/                  
    //
    string text = "";
    int i;

    if(smallLogo == "") {
        string logo[6] = {
            " ___ _                       ___ _        _ ", \
            "| __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |", \
            "| _|| / _` | '_ \\ '_ \\ || | | _ \\ | '_/ _` |", \
            "|_| |_\\__,_| .__/ .__/\\_, | |___/_|_| \\__,_|", \
            "           |_|  |_|   |__/                  ", \
            "" \
        };
        for(i = 0; i < (logo[4].length() - version_code.length() - 1) - 2; ++i) {
            text = text + logo[4][i];
        };
        logo[4] = text + "v" + version_code;
        smallLogo = centerText(logo, sizeof(logo)/sizeof(logo[0]));
    };

    text = "";
    int titleMenuSize = 0;

    if(titleMenu != "") {
        string p[1] = {
            titleMenu
        };
        titleMenu = "  " + centerText(p, sizeof(p)/sizeof(p[0]));
        titleMenuSize = 1;
    };

    int padding = terminalRows - 7 - sizeMenu - titleMenuSize;

    int j = 2;

    for(i=0; i <= padding; ++i) {
        if(i == (j + (getResolutionValue() * 2))) {
            text = text + smallLogo + titleMenu + "\n" + menuText(menu, sizeMenu, type_menu, *chooseMenu);
        } else if (i == padding - 1) {
            break;
        } else {
            text = text + "\n";
        };
    };

    int sizeOutput = terminalRows - 2;
    string output[sizeOutput];
    wipeOutput(output, sizeOutput);
    stringToOutput(text, output, sizeOutput);
    showFPS(output);
    text = getOutput(output, sizeOutput);

    pauseFrame();
    color(brightnessData);
    cursorPos_up();
    cout << text;
    #ifndef _WIN32
        cout << flush;
    #endif
    if (isFullTextBottom) {
        bottomKeymap(addTextBottom);
    } else {
        bottomKeymap("| [" + getNameKey(keymapData[0][1], keymapData[0][0]) + "][" + getNameKey(keymapData[1][1], keymapData[1][0]) + "] -> UP/DOWN | [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> ENTER | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> BACK |" + addTextBottom);
    };
    return;
};

void credit() {
    string credit_info[15] = {
        "FLAPPY BIRD",
        "",
        "Admin: Nguyen Van Khanh",
        "",
        "Design: Nguyen Van Khanh",
        "",
        "Music: Nguyen Van Khanh",
        "",
        "Facebook: fb.me/khanh10a1",
        "",
        "Github: github.com/KhanhNguyen9872"
        "",
        "",
        "KhanhNguyen9872. All rights reversed!",
        ""
    };

    int count = 0;
    int i, j, k;
    int line = sizeof(credit_info) / sizeof(credit_info[0]);
    string text, tmp;

    clearTerminal();
    showFPS(NULL);
    __sleep__(350);
    color(YELLOW);
    
    while(true) {
        text = "";
        for(i = 0; i < (terminalRows - count); ++i) {
            if (i == (terminalRows - 1 - count)) {
                if ((count - 1) < line) {
                    tmp = "";
                    for(k = 0; k < count; ++k) {
                        for(j = 0; j < ((terminalColumns - credit_info[k].length()) / 2); ++j) {
                            tmp = tmp + ' ';
                        };
                        tmp = tmp + credit_info[k] + "\n";
                    };
                };
                text = text + tmp;
                count = count + 1;
                
                break;
            } else {
                text = text + "\n";
            };
        };
        if ((terminalRows - count - 1) == 0) {
            color(MAGENTA);
            tmp = ">> Press any key to exit <<";
            text = "";
            for(i=0; i < (terminalColumns - tmp.length()) / 2; ++i) {
                text = text + ' ';
            };
            text = text + tmp;
            cursorPos_move(0, (terminalRows - 3));
            cout << text;
            #ifndef _WIN32
                cout << flush;
            #endif
            anyKey();
            return;
        };
        clearTerminal();
        cout << text;
        showFPS(NULL);
        __sleep__(250);
    };
    return;
};

void setBrightness(int value) {
    if (value == 3) {
        brightnessData = WHITE;
    } else if (value == 2) {
        brightnessData = LIGHTGRAY;
    } else if (value == 1) {
        brightnessData = DARKGRAY;
    } else {
        value = 3;
        brightnessData = WHITE;
    };
    writeConfig("brightness", to_string(value));
    return;
};

int getBrightness() {
    if (brightnessData == WHITE) {
        return 3;
    } else if (brightnessData == LIGHTGRAY) {
        return 2;
    } else if (brightnessData == DARKGRAY) {
        return 1;
    } else {
        setBrightness(3);
        return 3;
    };
};

void changeKeymapping(int value) {
    //
    //  _______________
    // |               |
    // | INPUT NEW KEY |
    // |               |
    //  ```````````````
    //
    int i, j = 1;
    string text;
    string lineSpace = "";
    int p[2];
    showBoxText("INPUT NEW KEY", false);
    bottomKeymap("");
    flushStdin();
    __getch(p);
    for(i = 0; i < sizeof(keymapData) / sizeof(keymapData[0]); ++i) {
        if((p[0] == keymapData[i][0]) && (p[1] == keymapData[i][1])) {
            j = 0;
            break;
        };
    };
    if(j) {
        if (setKeymap(value, p[1], p[0])) {
            writeConfig("key" + to_string(value), to_string(500 + p[1]));
        } else {
            flushStdin();
            errorBox("Key unavailable", "", true);
        };
    } else {
        if((p[0] == keymapData[value][0]) && (p[1] == keymapData[value][1])) {
            return;
        } else {
            flushStdin();
            errorBox("Key already set", "", true);
        };
    };
    flushStdin();
    return;
};

void keymappingSettings() {
    int i, j, k;
    int choose = 0;
    string text;
    string template_menu[7] = {
        "UP",
        "DOWN",
        "LEFT",
        "RIGHT",
        "EXIT/BACK",
        "JUMP",
        "ENTER"
    };
    int sizeMenu = sizeof(template_menu) / sizeof(template_menu[0]);
    string *menu = new string[sizeMenu];
    clearTerminal();
    while(true) {
        if(choose == -1) {
            flushStdin();
            return;
        };
        
        for(i = 0; i < sizeMenu; ++i) {
            k = keymapData[i][1];
            if (keymapData[i][0]) {
                k = k + 500;
            };
            text = "(" + to_string(k) + ")";
            k = text.length();
            for(j = 0; j < 5 - k; ++k) {
                text = text + ' ';
            };
            text = text + " '" + getNameKey(keymapData[i][1], keymapData[i][0]) + "'";
            k = text.length();
            for(j = 0; j < 13 - k; ++j) {
                text = text + ' ';
            };
            
            menu[i] = text + "  -->  " + template_menu[i];
        };
        showMenu("| Keymapping Settings |", menu, sizeMenu, -2, &choose, " [F12] -> RESET |", false);
        inputMenu(&choose, sizeMenu - 1, -2);
        __sleep__(50);
    };
    return;
};

void showBird(string output[], int countAnimation[2], int sizeInAnimation, int skinIndex, int countGoUp) {
    int index;
    if (countAnimation == NULL) {
        index = 0;
    } else {
        index = countAnimation[0];
    };
    showAnimation(output, skinFlyAnimation[skinIndex][index], sizeInAnimation, countGoUp);

    if (countAnimation != NULL) {
        if ((countAnimation[0] >= 3) || (countAnimation[0] <= 0) ) {
            countAnimation[1] = !countAnimation[1];
        };

        if(!countAnimation[1]) {
            countAnimation[0] = countAnimation[0] + 1;
        } else {
            countAnimation[0] = countAnimation[0] - 1;
        };
    };
    return;
};

void brightnessSettings() {
    //  
    //  ______________
    // [=========     ]
    //  ``````````````
    //
    if (settingsData[7]) {
        int sizeBar = 15;
        int max = 3;

        int currentBrightness = getBrightness();
        
        int i, j;
        int sizeOutput = terminalRows - 2;
        string output[sizeOutput];
        string road = getRoad();

        string text;
        string tmp;
        string lineSpace = "";
        for(j = 0; j < (terminalColumns - sizeBar) / 2; ++j) {
            lineSpace = lineSpace + ' ';
        };
        string perProcess = "";
        for(i = 0; i < (sizeBar / max); ++i) {
            perProcess = perProcess + "=";
        };
        string blankPerProcess = "";
        for(i = 0; i < (sizeBar / max); ++i) {
            blankPerProcess = blankPerProcess + ' ';
        };
        while(true) {
            if (currentBrightness == -1) {
                return;
            };
            text = "";
            for(i = 0; i < terminalRows - 4; ++i) {
                if ((terminalRows / 2) - 2 == i) {
                    text = text + lineSpace + ' ';
                    for(j = 0; j < sizeBar; ++j) {
                        text = text + "_";
                    };
                    text = text + "\n";

                    if(currentBrightness > 1) {
                        tmp = string(lineSpace);
                        tmp.resize(lineSpace.length() - 5);
                        text = text + tmp + "<--  ";
                    } else {
                        text = text + lineSpace;
                    };

                    text = text + "[";

                    for(j = 0; j < currentBrightness; ++j) {
                        text = text + perProcess;
                    };
                    for(j = 0; j < max - currentBrightness; ++j) {
                        text = text + blankPerProcess;
                    };

                    text = text + "]";
                    if(currentBrightness < max) {
                        text = text + "  -->";
                    };

                    text = text + "\n" + lineSpace + ' ';

                    for(j = 0; j < sizeBar; ++j) {
                        text = text + "`";
                    };
                    text = text + "\n";
                } else {
                    text = text + "\n";
                };
            };
            
            pauseFrame();
            color(brightnessData);
            wipeOutput(output, sizeOutput);
            stringToOutput(text, output, sizeOutput);
            if (settingsData[13]) {
                showBackground(output, 0, terminalRows - 3);
                output[sizeOutput - 1] = road;
            };
            showFPS(output);
            text = getOutput(output, sizeOutput);
            cursorPos_up();
            cout << text;
            bottomKeymap("| [" + getNameKey(keymapData[2][1], keymapData[2][0]) + "][" + getNameKey(keymapData[3][1], keymapData[3][0]) +"] -> LOW/HIGH | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> BACK |");
            inputMenu(&currentBrightness, 2, 2);
            __sleep__(50);
        };
    } else {
        errorBox("COLOR DISABLED", "", true);
    };
    return;
};

void resolutionSettings() {
    string menu[6] = {
        "80 x 20",
        "100 x 26",
        "120 x 30",
        "140 x 36",
        "160 x 40"
        #ifdef _TERMUX
            ,"113 x 20"
        #endif
    };

    #ifdef _TERMUX
        int sizeMenu = 6;
    #else
        int sizeMenu = 5;
    #endif
    
    int choose = getResolutionValue();
    string titleMenu;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        titleMenu = "| Current resolution: " + to_string(terminalColumns) + " x " + to_string(terminalRows) + " |";
        showMenu(titleMenu, menu, sizeMenu, 3, &choose, "", false);
        inputMenu(&choose, sizeMenu - 1, 3);
        __sleep__(50);
    };
    return;
};

void highScore() {
    int i;
    string menu[sizelistHighScore];
    int sizeMenu = sizeof(menu) / sizeof(menu[0]);
    for(i = 0; i < sizeMenu; ++i) {
        menu[i] = to_string(listHighScore[i]);
    };
    int choose = 0;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        showMenu("| High score |", menu, sizeMenu, 4, &choose, "| [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "][" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> MAIN MENU |", true);
        inputMenu(&choose, sizeMenu - 1, 4);
        __sleep__(50);
    };
    return;
};

void author() {
    #ifdef _WIN32
        system("start \"\" \"https://github.com/KhanhNguyen9872/\"");
    #else
        system("xdg-open \"https://github.com/KhanhNguyen9872/\"");
    #endif
    return;
};

bool setSkin(int index, bool isSkinBird) {
    if (!settingsData[11]) {
        return 1;
    };
    if (index >= 0) {
        if ((isSkinBird) && (index < sizeBirdAnimation)) {
            settingsData[4] = index;
            writeConfig("skinbird", to_string(index));
            return 1;
        };
        if ((!isSkinBird) && (index < sizeSkinWall)) {
            settingsData[15] = index;
            writeConfig("skinwall", to_string(index));
            return 1;
        };
    };
    return 0;
};

void showWall(string output[], int skinValue, int column, int up, int down) {
    if (output == NULL) {
        return;
    };
    string text;
    int i, j, k;

    // up
    text = skinWall[skinValue][0];
    for(i = 0; i < up - 2; ++i) {
        for(k = 0; k < text.length(); ++k) {
            if ((column + k) <= terminalColumns) {
                output[i][column + k] = text[k]; 
            };
        };
    };
    if (up - 2 >= 0) {
        text = skinWall[skinValue][1];
        for(k = 0; k < text.length(); ++k) {
            if (((up - 2) >= 0) && ((column + k) <= terminalColumns))  {
                output[up - 2][column + k] = text[k]; 
            };
        };
    };
    if (up - 1 >= 0) {
        text = skinWall[skinValue][2];
        for(k = 0; k < text.length(); ++k) {
            output[up - 1][column + k - 1] = text[k]; 
        };
    };

    // down
    text = skinWall[skinValue][5];
    j = terminalRows - 3;
    for(i = down + 3; i < j; ++i) {
        for(k = 0; k < text.length(); ++k) {
            if ((column + k) <= terminalColumns) {
                output[i][column + k] = text[k]; 
            };
        };
    };
    text = skinWall[skinValue][3];
    for(k = 0; k < text.length(); ++k) {
        if (((down + 1) < j) && ((column + k - 1) <= terminalColumns)) {
            output[down + 1][column + k - 1] = text[k]; 
        };
    };
    text = skinWall[skinValue][4];
    for(k = 0; k < text.length(); ++k) {
        if (((down + 2) < j) && ((column + k) <= terminalColumns)) {
            output[down + 2][column + k] = text[k];
        };
    };
    return;
};

void previewSkin(int index, bool isSkinBird) {
    int i = terminalRows - 2;
    
    int choose = 0;
    int countAnimation[2] = {1, 0};
    int sizeInAnimation;
    int j = 5, k = 0;
    bool r = false;
    if (isSkinBird) {
        sizeInAnimation = sizeof(skinFlyAnimation[index][countAnimation[0]]) / sizeof(skinFlyAnimation[index][countAnimation[0]][0]);
    };
    string road = getRoad();
    string output[i];
    string finalOutput;
    while(true) {
        if(choose == -1) {
            break;
        };
        if(choose == -2) {
            if (setSkin(index, isSkinBird)) {
                showBoxText("Completed", true);
                bottomKeymap("Press any key to continue!");
                anyKey();
            } else {
                errorBox("Cannot set skin", "", true);
            };
            break;
        };

        pauseFrame();
        wipeOutput(output, i);
        if (settingsData[13]) {
            showBackground(output, 0, terminalRows - 3);
        };

        if (isSkinBird) {
            showBird(output, countAnimation, sizeInAnimation, index, k);
            if (k <= -2) {
                k = k + 2;
            } else {
                k = k - 1;
            };
        } else {
            k = j + 6;
            showWall(output, index, 12, j, k);
            if (r) {
                j = j + 1;
            } else {
                j = j - 1;
            };
            if (j <= 0) {
                r = true;
            };
            if ((k + 3) >= i) {
                r = false;
            };
        };
        output[i - 1] = road;
        showFPS(output);
        finalOutput = getOutput(output, i);
        
        cursorPos_up();
        cout << finalOutput;
        bottomKeymap("| [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> SET | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> BACK |");
        inputMenu(&choose, 0, -7);
        __sleep__(200);
    };
    flushStdin();
    return;
};

void changeSkin(bool isSkinBird) {
    if (!settingsData[11]) {
        errorBox("SKIN DISABLED", "", true);
        return;
    };
    string text;
    int size, inputCase;
    if (isSkinBird) {
        size = sizeBirdAnimation;
        text = "skin_bird_";
        inputCase = 5;
    } else {
        size = sizeSkinWall;
        text = "skin_wall_";
        inputCase = 9;
    };

    string space = "";
    int i, j;
    int choose = 0;
    string menu[size + 1];

    if (size > 9) {
        space = space + ' ';
    } else if (size > 99) {
        space = space + "  ";
    };

    j = 0;
    for(i = 0; i < size; ++i) {
        menu[i] = text + to_string(i + 1) + space;
        if (j < menu[i].length()) {
            j = menu[i].length();
        };
    };
    j = j - 6;
    menu[size] = "Back";
    for(i = 0; i < j; ++i) {
        menu[size] = menu[size] + ' ';
    };
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        if (isSkinBird) {
            j = settingsData[4] + 1;
        } else {
            j = settingsData[15] + 1;
        };
        showMenu("| Current skin: " + text + to_string(j) + " |", menu, size + 1, inputCase, &choose, "", false);
        inputMenu(&choose, size, inputCase);
        __sleep__(50);
    };
    return;
};

void optionsSkin() {
    if (!settingsData[11]) {
        errorBox("SKIN DISABLED", "", true);
        return;
    };
    
    string menu[3] = {
        "Skin Bird",
        "Skin Wall",
        "Back"
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };

        showMenu("| Change Skin |", menu, sizeMenu, 8, &choose, "", false);
        inputMenu(&choose, sizeMenu - 1, 8);
        __sleep__(50);
    };
    return;
};

string getNameGameSpeed(int index) {
    switch(index) {
        case 1:
            return "Low";
        case 2:
            return "Medium";
        case 3:
            return "High";
        case 4:
            return "Ultra";
        case 5:
            return "Extreme";
        case 6:
            return "30 FPS";
        case 7:
            return "60 FPS";
        case 8:
            return "Unlimited";
        default:
            break;
    };
    return "NULL";
};

int getValueGameSpeed(int index) {    // value is 200, 150, 100 or 50
    int valueData[8] = {
        175, 150, 125, 100, 75, 20, 5, 0
    };
    return valueData[index - 1];
};

void setGameSpeed(int value) {
    if (isInGame) {
        errorBox("Unavailable in game", "You must return to main menu first!", true);
        return;
    };
    
    if ((value < 1) || (getNameGameSpeed(value) == "NULL")) {
        writeConfig("speed", "2");
        return;
    };

    settingsData[17] = value;
    writeConfig("speed", to_string(value));
    return;
};

void getFPS() {
    while(true) {
        FPS = frameFPS;
        frameFPS = 0;
        __sleep__(1000);
    };
};

void moreSettingsMenu() {
    string text;
    string menu[8] = {
        "Auto mode [ ]",
        "Show firework [ ]",
        "Show background [ ]",
        "Show X/Y [ ]",
        "",
        "",
        "Show FPS [ ]",
        "Back"
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };

        menu[4] = "Number firework [" + to_string(settingsData[16]) + "]";

        if (settingsData[3]) {
            menu[0][11] = 'X';
        } else {
            menu[0][11] = ' ';
        };

        if (settingsData[12]) {
            menu[1][15] = 'X';
        } else {
            menu[1][15] = ' ';
        };

        if (settingsData[13]) {
            menu[2][17] = 'X';
        } else {
            menu[2][17] = ' ';
        };

        if (settingsData[14]) {
            menu[3][10] = 'X';
        } else {
            menu[3][10] = ' ';
        };

        if (settingsData[18]) {
            menu[6][10] = 'X';
        } else {
            menu[6][10] = ' ';
        };

        menu[5] = "Speed [" + getNameGameSpeed(settingsData[17]) + "]";
        text = "| [" + getNameKey(keymapData[0][1], keymapData[0][0]) + "][" + getNameKey(keymapData[1][1], keymapData[1][0]) + "] -> UP/DOWN |";
        if (choose == 5) {
            text = text + " [" + getNameKey(keymapData[2][1], keymapData[2][0]) + "][" + getNameKey(keymapData[3][1], keymapData[3][0]) + "] -> CHANGE |";
        } else {
            text = text + " [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> ENTER |";
        };
        text = text + " [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> BACK |";
        
        showMenu("| Settings |", menu, sizeMenu, 7, &choose, text, true);
        inputMenu(&choose, sizeMenu - 1, 7);
        __sleep__(50);
    };
    return;
};

void settingsMenu() {
    string menu[8] = {
        "Music [ ]",
        "SFX [ ]",
        "Brightness",
        "Keymapping",
        "Resolution",
        "Difficult",
        "More...",
        "Back"
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };

        if (settingsData[0]) {
            menu[0][7] = 'X';
        } else {
            menu[0][7] = ' ';
        };
        
        if (settingsData[1]) {
            menu[1][5] = 'X';
        } else {
            menu[1][5] = ' ';
        };

        showMenu("| Settings |", menu, sizeMenu, 1, &choose, "", false);
        inputMenu(&choose, sizeMenu - 1, 1);
        __sleep__(50);
    };
    return;
};

string getNameDifficult(int value) {
    switch(value) {
        case 0:
            return "Chill";
        case 1:
            return "Easy";
        case 2:
            return "Normal";
        case 3:
            return "Hard";
        case 4:
            return "Hell";
        default:
            break;      
    };
    return "NULL";
};

void changeDifficult() {
    if (isInGame) {
        errorBox("Unavailable in game", "You must return to main menu first!", true);
        return;
    };
    int i;
    string menu[6];
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    for(i = 0; i < (sizeMenu - 1); i++) {
        menu[i] = getNameDifficult(i);
    };
    menu[5] = "Back";
    int choose = settingsData[19];
    string titleMenu;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        titleMenu = "| Current difficult: " + menu[settingsData[19]] + " |";
        showMenu(titleMenu, menu, sizeMenu, 10, &choose, "", false);
        inputMenu(&choose, sizeMenu - 1, 10);
        __sleep__(50);
    };
    return;
};

int pausedMenu(bool isShowPauseInGame) {
    int choose = 0;

    // paused ingame
    flushStdin();
    if (isShowPauseInGame) {
        while(true) {
            if (choose == -1) {
                color(brightnessData);
                return 0;
            };
            if (choose == -2) {
                break;
            };
            pauseFrame();
            showFPS(NULL);
            showBoxText("PAUSED", false);
            bottomKeymap("| [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "][" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> RESUME | [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> PAUSED MENU |");
            inputMenu(&choose, 0, -5);
            __sleep__(100);
        };
    };

    // paused menu
    choose = 0;
    string menu[6] = {
        "Continue",
        "New game",
        "High score",
        "Settings",
        "Main menu",
        "Exit"
    };

    int sizeMenu = sizeof(menu) / sizeof(menu[0]);

    clearTerminal();
    while(true) {
        if (choose == -1) {
            return 0;
        };
        if (choose == -2) {
            return 1;
        };
        if (choose == -3) {
            return 2;
        };
        showMenu("| Paused |", menu, sizeMenu, -4, &choose, "", false);
        inputMenu(&choose, sizeMenu - 1, -4);
        __sleep__(50);
    };
    return 1;
};

void launchPaused(int *chooseMenu) {
    bool tmp = true;
    int tmp2 = 0;
    while(true) {
        tmp2 = pausedMenu(tmp);
        if (tmp2 == 1) {
            if (showYesorNo("Back to main menu?")) {
                *chooseMenu = -1;
                break;
            } else {
                tmp = false;
            };
        } else if (tmp2 == 2) {
            if (showYesorNo("New game?")) {
                *chooseMenu = -2;
                break;
            } else {
                tmp = false;
            };
        } else {
            *chooseMenu = -3;
            break;
        };
    };
    return;
};

void moreOptions() {
    string menu[2] = {
        "Go to Github",
        "Back"
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        showMenu("", menu, sizeMenu, 6, &choose, "", false);
        inputMenu(&choose, sizeMenu - 1, 6);
        __sleep__(50);
    };
    return;
};

void mainMenu() {
    isInGame = false;
    string menu[7] = {
        "Start",
        "High score",
        "Skin",
        "More...",
        "Settings",
        "Credit",
        "Exit"
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int chooseMenu = 0;
    playSound(soundMainmenu, false);
    flushStdin();
    while(true) {
        showMenu("", menu, sizeMenu, 0, &chooseMenu, "| [" + getNameKey(keymapData[0][1], keymapData[0][0]) + "][" + getNameKey(keymapData[1][1], keymapData[1][0]) + "] -> UP/DOWN | [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> ENTER |", true);
        inputMenu(&chooseMenu, sizeMenu - 1, 0);
        __sleep__(50);
    };
    return;
};

void startOptions() {
    string menu[3] = {
        "Offline",
        "Online",
        "Back"
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    flushStdin();
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        showMenu("| Start game |", menu, sizeMenu, -8, &choose, "", false);
        inputMenu(&choose, sizeMenu - 1, -8);
        __sleep__(50);
    };
    return;
};

void connectIP(string ip) {
    errorBox(ip, "", true);
    return;
};

string showBoxInput(string title, string ex, string _bottomKeymap, int max_size, bool onlyInteger) {
    int sizeEx = ex.length();
    if(sizeEx > 0) {
        sizeEx = sizeEx + 2;
    };
    string errorStr = "";
    int p[2] = {-1, -1};
    int i, j;
    int sizeShowInput;
    int column = 20;
    int row;  // (8 / 2) - 1
    int sizeBox = terminalColumns - (column * 2);
    string text;
    string text2;
    int sizeOutput = terminalRows - 2;
    string output[sizeOutput];
    string fullOutput;
    string nameKey;
    string userInput = "";
    string road = getRoad();
    int lengthUserInput = 0;
    int indexCursor = 0;
    int secondIndexCursor = 0;
    FPS = 0;
    frameFPS = 0;
    flushStdin();
    while(true) {
        if ((!p[0]) && (p[1] == 27)) {
            clearTerminal();
            flushStdin();
            return "";
        };
        sizeShowInput = (sizeBox - 8 - sizeEx);
        row = (terminalRows / 2) - 6;
        wipeOutput(output, sizeOutput);
        if (settingsData[13]) {
            showBackground(output, 0, terminalRows - 3);
            output[sizeOutput - 1] = road;
        };
        lengthUserInput = userInput.length();
        // -1
        for(i = 0; i < sizeBox; ++i) {
            output[row][column + i] = '_';
        };
        // 0
        row = row + 1;
        output[row][column - 1] = '|';
        text = "Length: " + to_string(lengthUserInput);
        for(i = 0; i < text.length(); ++i) {
            output[row][column + 1 + i] = text[i];
        };
        output[row][terminalColumns - column] = '|';
        output[row][terminalColumns - column + 1] = '\\';
        // 1
        row = row + 1;
        output[row][column - 1] = '|';
        for(i = 0; i < sizeBox; ++i) {
            output[row][column + i] = '`';
        };
        output[row][terminalColumns - column] = '|';
        output[row][terminalColumns - column + 1] = '\\';
        // 2
        // row = row + 1;
        // output[row][column - 1] = '|';
        
        // output[row][terminalColumns - column] = '|';
        // output[row][terminalColumns - column + 1] = '\\';
        // 3 - show title
        row = row + 1;
        output[row][column - 1] = '|';
        text = title;
        for(i = 0; i < text.length(); ++i) {
            output[row][column + (i + 4) - 1] = text[i];
        };
        output[row][terminalColumns - column] = '|';
        output[row][terminalColumns - column + 1] = '\\';
        // 4 
        row = row + 1;
        output[row][column - 1] = '|';
        for(i = 0; i < sizeBox - 6 - sizeEx; ++i) {
            output[row][column + i + 3 + sizeEx] = '_';
        };
        output[row][terminalColumns - column] = '|';
        output[row][terminalColumns - column + 1] = '\\';
        // 5 - show input
        row = row + 1;
        output[row][column - 1] = '|';
        for(i = 0; i < ex.length(); ++i) {
            output[row][column + 3 + i] = ex[i];
        };
        if (ex.length() > 0) {
            output[row][column + 3 + ex.length() + 1] = '|';
        } else {
            output[row][column + 2] = '|';
        };
        text = userInput;
        j = secondIndexCursor;
        if (j > 0) {
            text2 = "";
            for(i = 0; i < text.length() - j; ++i) {
                text2 = text2 + text[i];
            };
            text = text2;
            j = 0;
        };
        while (text.length() > (sizeShowInput + 1)) {
            text.erase(0, 1);
        };
        for(i = 0; i < text.length(); ++i) {
            output[row][column + 3 + i + sizeEx] = text[i];
        };
        output[row][terminalColumns - column - 3] = '|';
        output[row][terminalColumns - column] = '|';
        output[row][terminalColumns - column + 1] = '\\';
        // 6
        row = row + 1;
        output[row][column - 1] = '|';
        for(i = 0; i < sizeBox - 6 - sizeEx; ++i) {
            output[row][column + i + 3 + sizeEx] = '`'; 
        };
        output[row][column + 3 + indexCursor + sizeEx] = '^';
        output[row][terminalColumns - column] = '|';
        output[row][terminalColumns - column + 1] = '\\';
        // errorStr
        if (errorStr != "") {
            row = row + 1;
            output[row][column - 1] = '|';
            output[row][column + sizeBox] = '|';
            output[row][column + sizeBox + 1] = '\\';

            errorStr = "> " + errorStr;
            for(i = 0; i < errorStr.length(); ++i) {
                output[row][column + i + 3] = errorStr[i];
            };
            errorStr = "";
        };
        // 7
        row = row + 1;
        output[row][column - 1] = '|';
        output[row][column + sizeBox] = '|';
        output[row][column + sizeBox + 1] = '\\';
        for(i = 0; i < sizeBox; ++i) {
            output[row][column + i] = '_';
        };
        // 8
        row = row + 1;
        for(i = -1; i < sizeBox + 2; ++i) {
            output[row][column + i] = '\\';
        };

        // output
        fullOutput = getOutput(output, sizeOutput);
        cursorPos_up();
        cout << fullOutput;
        #ifndef _WIN32
            cout << flush;
        #endif
        showFPS(NULL);
        bottomKeymap(_bottomKeymap);
        // input
        // if (__kbhit__()) {
            __getch(p);
            if (!p[0]) {
                switch(p[1]) {
                    #ifdef _WIN32
                        case 8: // backspace
                    #else
                        case 127:
                    #endif
                        if (lengthUserInput > 0) {
                            if (indexCursor > (sizeShowInput + 1)) {
                                userInput.pop_back();
                            } else {
                                text = "";
                                for(i = 0; i < lengthUserInput; ++i) {
                                    if (lengthUserInput > (sizeShowInput + 1)) {
                                        if (i == (lengthUserInput - (sizeShowInput + 1) + (indexCursor - 1) - secondIndexCursor)) {
                                            if ((i == 0) || (secondIndexCursor == (lengthUserInput - (sizeShowInput + 1)))) {
                                                if (indexCursor > 0) {
                                                    indexCursor = indexCursor - 1;
                                                };
                                                if (secondIndexCursor > 0) {
                                                    secondIndexCursor = secondIndexCursor - 1;
                                                };
                                            };
                                            continue;
                                        };
                                    } else {
                                        if (i == (indexCursor - 1)) {
                                            continue;
                                        };
                                    };
                                    text = text + userInput[i];  
                                };
                                userInput = text;
                            };
                            if ((indexCursor > 0) && (lengthUserInput <= (sizeShowInput + 1))) {
                                indexCursor = indexCursor - 1;
                            };
                        };
                        break;
                    #ifdef _WIN32
                        case 13: // enter
                    #else
                        case 10:
                    #endif
                            if (lengthUserInput > 0) {
                                return userInput;
                            } else {
                                errorStr = "Empty input";
                            };
                    default:
                        if ((p[1] == 32) && (!p[0])) {
                            nameKey = ' ';
                        } else {
                            nameKey = getNameKey(p[1], p[0]);
                        };
                        if (nameKey.length() == 1) { // get one char or space (32)
                            if ((max_size == -1) || (lengthUserInput < max_size)) {
                                i = false;
                                if (onlyInteger) {
                                    if (((p[1] >= '0') && (p[1] <= '9')) || (p[1] == '-')) {
                                        i = true;
                                    } else {    // only accept integer
                                        errorStr = "Only accept integer number [0-9]";
                                    };
                                } else {
                                    i = true;
                                };
                                if (i) { // add input
                                    if (indexCursor < 1) {
                                        userInput = nameKey + userInput;
                                    } else if (indexCursor > sizeShowInput) {
                                        userInput = userInput + nameKey;
                                    } else {
                                        text = "";
                                        for(i = 0; i < lengthUserInput; ++i) {
                                            text = text + userInput[i];
                                            if (lengthUserInput > (sizeShowInput + 1)) {
                                                if (i == (lengthUserInput - (sizeShowInput + 1) + (indexCursor - 1) - secondIndexCursor)) {
                                                    text = text + nameKey;
                                                };
                                            } else {
                                                if (i == (indexCursor - 1)) {
                                                    text = text + nameKey;
                                                };
                                            };
                                        };
                                        userInput = text;
                                    };
                                    
                                    i = false;
                                    if (lengthUserInput <= sizeShowInput) {
                                        i = true;
                                    } else if ((secondIndexCursor == (lengthUserInput - (sizeShowInput + 1))) && (indexCursor <= sizeShowInput)) {
                                        secondIndexCursor = secondIndexCursor + 1;
                                        i = true;
                                    };
                                    if (i) {
                                        indexCursor = indexCursor + 1;
                                    };
                                };
                            } else {    // input max size
                                errorStr = "Max length: " + to_string(max_size) + " character";
                            };
                        };
                        break;
                };
            } else {
                switch(p[1]) {
                    #ifdef _WIN32
                        case 75: // left
                    #else
                        case 68:
                    #endif
                            if ((indexCursor == 1) && (lengthUserInput > sizeShowInput) && (secondIndexCursor < (lengthUserInput - (sizeShowInput + 1)))) {
                                secondIndexCursor = secondIndexCursor + 1;
                            } else if (indexCursor > 0) {
                                indexCursor = indexCursor - 1;
                            };
                            break;
                    #ifdef _WIN32
                        case 77: // right
                    #else
                        case 67:
                    #endif
                            if ((indexCursor < lengthUserInput) && (indexCursor <= sizeShowInput)) {
                                indexCursor = indexCursor + 1;
                            } else {
                                if (secondIndexCursor > 0) {
                                    secondIndexCursor = secondIndexCursor - 1;
                                };
                            };
                            break;
                    
                };
            };
        // };
        // __sleep__(25);
    };
    return NULL;
};

void boxInputIP() {
    string ip = "";
    while(true) {
        ip = showBoxInput("IP Online", "tcp://", "| [ESC] -> Back | [ENTER] -> Connect | [<- / ->] Move | [BACKS] -> Delete |", 50, false);
        if (ip == "") {
            flushStdin();
            return;
        } else {
            connectIP(ip);
        };
    };
    return;
};

void configError(int key) {
    resizeTerminal(terminalColumns, terminalRows);
    thread lockSizeTer(lockSizeTerminal);
    int j;
    int size = sizeof(keymapData) / sizeof(keymapData[0]);
    for(j = 0; j < size; ++j) {
        writeConfig("key" + to_string(j), "-1");
    };
    clearTerminal();
    errorBox("Keymap Error [key" + to_string(key) + "]", "Keymap configuration has been reset! You can restart the game now!", false);
    while(true) {
        __getch__();
    };
    exit(-1);
    return;
};

void loadConfig() {
    if (settingsData[6]) {
        int isTwoChar;
        int i, j, k;
        int size = sizeof(keymapData) / sizeof(keymapData[0]);
        int newKeymapData[size][2];
        for(i = 0; i < sizeof(newKeymapData) / sizeof(newKeymapData[0]); ++i) {
            newKeymapData[i][1] = -1;
            newKeymapData[i][0] = 0;
        };
        if (settingsData[9]) {
            setResolution(readConfig("resolution"));
        } else {
            setResolution(-1);
        };
        if (settingsData[8]) {
            settingsData[0] = readConfig("music");
            settingsData[1] = readConfig("sfx");
        };
        if (settingsData[7]) {
            setBrightness(readConfig("brightness"));
        };
        if (settingsData[10]) {
            settingsData[3] = readConfig("automode");
        };
        // skin bird
        if (!setSkin(readConfig("skinbird"), true)) {
            writeConfig("skinbird", to_string(settingsData[4]));  
        };
        // skin wall
        if (!setSkin(readConfig("skinwall"), false)) {
            writeConfig("skinwall", to_string(settingsData[15]));  
        };
        // Difficult
        i = readConfig("difficult");
        if ((i < 1) || (i > 5)) {
            writeConfig("difficult", to_string(settingsData[19] + 1));
        } else {
            settingsData[19] = i - 1;
        };
        // show firework
        i = readConfig("showfw");
        if (i < 1) {
            writeConfig("showfw", to_string(settingsData[12] + 1));
        } else if (i == 2) {
            settingsData[12] = true;
        } else {
            settingsData[12] = false;
        };
        i = readConfig("totalfirework");
        if ((i > 0) && (i < 51)) {
            settingsData[16] = i;
        } else {
            writeConfig("totalfirework", to_string(settingsData[16]));
        };
        // show background
        i = readConfig("showbg");
        if (i < 1) {
            writeConfig("showbg", to_string(settingsData[13] + 1));
        } else if (i == 2) {
            settingsData[13] = true;
        } else {
            settingsData[13] = false;
        };
        // show xy
        i = readConfig("showxy");
        if (i < 1) {
            writeConfig("showxy", to_string(settingsData[14] + 1));
        } else if (i == 2) {
            settingsData[14] = true;
        } else {
            settingsData[14] = false;
        };
        // speed
        setGameSpeed(readConfig("speed"));
        // show fps
        i = readConfig("showfps");
        if (i < 1) {
            writeConfig("showfps", to_string(settingsData[18] + 1));
        } else if (i == 2) {
            settingsData[18] = true;
        } else {
            settingsData[18] = false;
        };
        
        for(i = 0; i < size; ++i) {
            #ifdef _WIN32
                isTwoChar = false;
                j = readConfig("key" + to_string(i));
                if(j > 500) {
                    j = j - 500;
                    isTwoChar = true;
                };
                if (j > 7) {
                    for(k = 0; k < size; ++k) {
                        if((j == newKeymapData[k][1]) && (isTwoChar == newKeymapData[k][0])) {
                            configError(i);
                        };
                    };
                };
                if (setKeymap(i, j, isTwoChar)) {
                    newKeymapData[i][0] = isTwoChar;
                    newKeymapData[i][1] = j;
                } else {
                    writeConfig("key" + to_string(i), "-1");
                };
            #else

            #endif
        };
        // check keymap after set
        for(i = 0; i < size; ++i) {
            for(j = 0; j < size; ++j) {
                if(i == j) {
                    continue;
                };
                if(keymapData[i] == keymapData[j]) {
                    configError(i);
                };
            };
        };
    } else {
        setResolution(0);
    };
    return;
};

void loadHighScore() {
    int i;
    for(i = 0; i < sizelistHighScore; ++i) {
        listHighScore[i] = 0;
    };

    int countLine = 0;
    string line;
    string data = readFile(scoreFileName);
    if (data == "") {
        return;
    };
    istringstream file(data);
    while(getline(file, line)) {
        try {
            i = stoi(line);
            countLine = countLine + 1;
        } catch (...) {
            continue;
        };
    };

    i = 0;
    int *tmpList = new int[countLine];
    istringstream f(data);
    while(getline(f, line)) {
        tmpList[i] = stoi(line);
        i = i + 1;
    };

    int maxIndex;
    int j;
    int max;
    for(i = sizelistHighScore - 1; i >= 0 ; --i) {
        maxIndex = 0;
        max = tmpList[maxIndex];
        for(j = 1; j < countLine; ++j) {
            if (max < tmpList[j]) {
                max = tmpList[j];
                maxIndex = j;
            };
        };

        if (listHighScore[i] < max) {
            listHighScore[i] = max;
        };
        tmpList[maxIndex] = -1;
    };
    return;
};

int getHighScore(int currentScore) {
    int i;
    int j, k, l;
    j = listHighScore[0];
    for(i = 1; i < sizelistHighScore; ++i) {
        if (j < listHighScore[i]) {
            j = listHighScore[i];
            k = listHighScore[i];
        };
    };
    for(i = 0; i < sizelistHighScore; ++i) {
        l = listHighScore[i] - currentScore;

        if (l > 0 && l < k) {
            j = listHighScore[i];
            k = l;
        };
    };
    return j;
};

void showHighScore(string output[], int highScore, bool isHigher) {
    if (output == NULL) {
        return;
    };
    if (highScore > 0) {
        string text;
        string highScore_str;
        if (isHigher) {
            highScore_str = "| HIGH SCORE: ";
        } else {
            highScore_str = "| TARGET: ";
        };
        highScore_str = highScore_str + to_string(highScore) + " |";
        int highScore_length = highScore_str.length() - 4;

        int i, k;
        int j = terminalColumns - highScore_length - 6;
        
        text = "+-";
        for(i = 0; i < highScore_length; ++i) {
            text = text + "-";
        };
        text = text + "-+";

        if (isHigher) {
            i = 2;
        } else {
            i = 0;
        };

        for(k = 0; k < text.length(); ++k) {
            output[3 - i][j + k] = text[k];
        };
        
        for(k = 0; k < highScore_str.length(); ++k) {
            output[4 - i][j + k] = highScore_str[k];
        };

        for(k = 0; k < text.length(); ++k) {
            output[5 - i][j + k] = text[k];
        };
    };
    return;
};

void showScore(string output[], int score, int higherScore, bool isHigher) {
    if (output == NULL) {
        return;
    };
    showHighScore(output, higherScore, isHigher);
    if (!isHigher) {
        string text;
        string score_str = "| SCORE: " + to_string(score) + " |";
        int score_length = score_str.length() - 4;

        int i, k;
        int j = terminalColumns - score_length - 6;
        
        text = "+-";
        for(i = 0; i < score_length; ++i) {
            text = text + "-";
        };
        text = text + "-+";

        for(k = 0; k < text.length(); ++k) {
            output[1][j + k] = text[k];
        };

        for(k = 0; k < score_str.length(); ++k) {
            output[2][j + k] = score_str[k];
        };

        for(k = 0; k < text.length(); ++k) {
            output[3][j + k] = text[k];
        };
    };
    return;
};

void showAllWall(string output[], int *nextWall, int *score, int countWall) {
    if (output == NULL) {
        return;
    };
    int i, j;

    for(i = 0; i < sizelistWall; ++i) {
        if ((listWall[i][0] > -5) && (listWall[i][0] < terminalColumns + 2)) {
            // display Wall
            showWall(output, settingsData[15], listWall[i][0], listWall[i][1], listWall[i][2]);
            if(gameStarted) {
                listWall[i][0] = listWall[i][0] - 1; // decrease terminalColumn

                if (listWall[i][0] == -2) {
                    *nextWall = i + 1;
                    *score = *score + 1;
                    if (*nextWall > sizelistWall - 1) {
                        *nextWall = 0;
                    };
                    if (*score > 2000000000) {
                        gameStarted = false;
                    };
                };
            };
        } else if (gameStarted) { 
            if (listWall[i][1] > -5) { // remove Wall cannot display
                for(j = 0; j < 3; ++j) {
                    listWall[i][j] = -5;
                };
            };
        };
    };
    return;
};

bool gameOver(int score, int y, int minY, int maxY) {
    if(!settingsData[3]) {
        int i;
        for(i = sizelistHighScore - 1; i >= 0; --i) {
            if (score > listHighScore[i]) {
                writeFile(scoreFileName, readFile(scoreFileName) + to_string(score) + "\n");
                listHighScore[i] = score;
                break;
            };
        };
    };
    int choose = 0;
    while(true) {
        if (choose == -1) {
            return 1;
        };
        if (choose == -2) {
            return 0;
        };
        pauseFrame();
        showFPS(NULL);
        showBoxText("Game over", false);
        bottomKeymap("| [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> TRY AGAIN | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> MAIN MENU | Diff: " + getNameDifficult(settingsData[19]) + " | Y: [" + to_string(minY) + "][" + to_string(y) + "][" + to_string(maxY) + "] |");
        inputMenu(&choose, 0, -6);
        __sleep__(100);
    };
    return 0;
};

void resetWall() {
    int i, j;
    for(i = 0; i < sizelistWall; ++i) {
        for(j = 0; j < sizeof(listWall[i]) / sizeof(listWall[i][0]); ++j) {
            listWall[i][j] = -5;
        };
    };
    return;
};

void addWall(int *countWall) {
    if (*countWall >= sizelistWall) {
        return;
    };
    if (listWall[*countWall][0] > -1) {
        return;
    };
    listWall[*countWall][0] = terminalColumns + 1;
    listWall[*countWall][1] = (rand() % (terminalRows - 9)); //up
    listWall[*countWall][2] = listWall[*countWall][1] + 8 - settingsData[19]; // down

    *countWall = *countWall + 1;
    if (*countWall >= sizelistWall) {
        *countWall = 0;
    };
    return;
};

void checkWall(int nextWall, int y, int maxUp, bool *isOver) {
    if(!gameStarted) {
        return;
    };
    if (listWall[nextWall][0] == -2) {
        return;
    };
    if (listWall[nextWall][0] < 9) { // < size Animation Bird + 1 (+ 1)
        if ((listWall[nextWall][1] < 0) || (listWall[nextWall][2] < 0)) { // wall not initial
            return;
        };
        maxUp = maxUp + 1;
        int up = maxUp - listWall[nextWall][1];
        int down = maxUp - listWall[nextWall][2];
        if (((y >= up) && (y < up + 3)) || ((y <= down) && (y > down - 3))) { // 3 is row of Bird Animation
            *isOver = 1;
            return;
        };
        if (listWall[nextWall][0] < 8) { // < size Animation Bird + 1
            if ((up <= y) || (down >= y)) {
                *isOver = 1;
                return;
            };
        };
    };
    return;
};

void showFirework(string output[], int firework[3], int sizeOutput) {
    if (output == NULL) {
        return;
    };
    
    // firework[0] -> Column | firework[1] -> Row | firework[2] -> where firework BUMMM
    if (firework[0] == -1) {    // define locate firework appear
        if ((rand() % 4) == 2) {
            do { // Column
                firework[0] = rand() % (terminalColumns - 1); // right
            } while (firework[0] < 1); // left

            firework[1] = sizeOutput - 1; // Row
            firework[2] = (rand() % ((firework[1] / 2) + (firework[1] / 4) + 1)); // BUMMM
        } else {
            return;
        };
    };

    if (firework[2] < 0) {
        switch(firework[2]) {
            //
            //
            //   `.:.`
            //  ---O---
            //   .`:`.
            //
            //
            case -6:
            case -7:
            case -8:
            case -9:
            case -10:
            case -11:
            case -12:
                // left
                if ((firework[0] - 2) > -1) {
                    output[firework[1]][firework[0] - 2] = '-';
                };
                if (firework[2] < -7) {
                    if ((firework[0] - 3) > -1) {
                        output[firework[1]][firework[0] - 3] = '-';
                    };
                };

                // right
                if ((firework[0] + 2) <= terminalColumns) {
                    output[firework[1]][firework[0] + 2] = '-';
                };
                if (firework[2] < -8) {
                    if ((firework[0] + 3) <= terminalColumns) {
                        output[firework[1]][firework[0] + 3] = '-';
                    };
                };

                if ((firework[1] - 1) > -1) {
                    // up
                    output[firework[1] - 1][firework[0]] = ':';

                    if (firework[2] < -9) {
                        if ((firework[1] - 2) > -1) {
                            output[firework[1] - 2][firework[0]] = '.';
                        };
                    };

                    // up left
                    if (firework[2] < -7) {
                        if ((firework[0] - 2) > -1) {
                            output[firework[1] - 1][firework[0] - 2] = '`';
                        };
                    };
                    if ((firework[0] - 1) > -1) {
                        output[firework[1] - 1][firework[0] - 1] = '.';
                    };
                    
                    // up right
                    if ((firework[0] + 1) <= terminalColumns) {
                        output[firework[1] - 1][firework[0] + 1] = '.';
                    };
                    if (firework[2] < -8) {
                        if ((firework[0] + 2) <= terminalColumns) {
                            output[firework[1] - 1][firework[0] + 2] = '`';
                        };
                    };
                };

                if (firework[1] + 1 <= terminalRows - 3) {
                    // down
                    output[firework[1] + 1][firework[0]] = ':';

                    if (firework[2] < -8) {
                        if ((firework[1] + 2) <= terminalRows - 3) {
                            output[firework[1] + 2][firework[0]] = '`';
                        };
                    };

                    // down left
                    if (firework[2] < -8) {
                        if ((firework[0] - 2) > -1) {
                            output[firework[1] + 1][firework[0] - 2] = '.';
                        };
                    };
                    if ((firework[0] - 1) > -1) {
                        output[firework[1] + 1][firework[0] - 1] = '`';
                    };

                    // down right
                    if ((firework[0] + 1) <= terminalColumns) {
                        output[firework[1] + 1][firework[0] + 1] = '`';
                    };
                    if (firework[2] < -9) {
                        if ((firework[0] + 2) <= terminalColumns) {
                            output[firework[1] + 1][firework[0] + 2] = '.';
                        };
                    };
                };
            case -3:
            case -4:
            case -5:
                // left
                if (firework[2] < -4) {
                    if ((firework[0] - 1) > -1) {
                        output[firework[1]][firework[0] - 1] = '-';
                    };
                };

                // right
                if ((firework[0] + 1) <= terminalColumns) {
                    output[firework[1]][firework[0] + 1] = '-';
                };

                if (firework[2] > -7) {
                    if ((firework[1] - 1) > -1) {
                        // up
                        output[firework[1] - 1][firework[0]] = '.';

                        // up left
                        if ((firework[0] - 1) > -1) {
                            output[firework[1] - 1][firework[0] - 1] = '.';
                        };

                        // up right
                        if (firework[2] < -5) {
                            if ((firework[0] + 1) <= terminalColumns) {
                                output[firework[1] - 1][firework[0] + 1] = '.';
                            };
                        };
                    };

                    if ((firework[1] + 1) <= terminalRows - 3) {
                        // down
                        output[firework[1] + 1][firework[0]] = '`';

                        // down left
                        if (firework[2] < -4) {
                            if ((firework[0] - 1) > -1) {
                                output[firework[1] + 1][firework[0] - 1] = '`';
                            };
                        };
                        // down right
                        if ((firework[0] - 1) > -1) {
                            output[firework[1] + 1][firework[0] + 1] = '`';
                        };
                    };
                };
            case -1:
            case -2:
                // middle
                output[firework[1]][firework[0]] = 'O';
        };

        firework[2] = firework[2] - 1;
        if (firework[2] <= -12) {
            firework[0] = -1;
        };
        return;
    };

    output[firework[1]][firework[0]] = '|';
    if (firework[1] <= firework[2]) {
            firework[2] = -1;
    } else {
        firework[1] = firework[1] - 1;
        if (firework[1] < 0) {
            firework[0] = -1;
        };
    };

    return;
};

void flappyBird() { 
    int i, j;
    isInGame = true;
    bool isOver = 0;
    gameStarted = false;
    
    int x = 0;
    int y = 0;
    int minY = 0;
    int maxY = 0;
    int oldX = x;
    int choose = 0;
    int maxUp = terminalRows / 4;
    int speed = getValueGameSpeed(settingsData[17]);
    
    // background
    int countStart = rand() % sizeBackground;
    
    // bird
    int countAnimation[2] = {1, 0};
    int sizeBird = sizeof(skinFlyAnimation[settingsData[4]]) / sizeof(skinFlyAnimation[settingsData[4]][0]);
    int sizeInAnimation = sizeof(skinFlyAnimation[settingsData[4]][countAnimation[0]]) / sizeof(skinFlyAnimation[settingsData[4]][countAnimation[0]][0]);

    // wall
    int nextWall = 0;
    int countWall = 0;
    int wallCreateDistance = 22 + (getResolutionValue() * 4);
    int distance = wallCreateDistance - 4;

    // road
    string lineMap = getRoad();
    
    // score
    int score = 0;
    int highScore_ = getHighScore(score);
    bool highScoreIsScore = 0;

    // output data
    // string output[terminalRows - 2];
    int sizeOutput = terminalRows - 2;
    string *output = new string[sizeOutput];    // memory leak when return function (but when deallocate array, [Access Violation Error] occurred)
    string text;
    string outputGame;

    // firework
    int fireworkData[settingsData[16]][3];
    for(i = 0; i < settingsData[16]; ++i) {
        for(j = 0; j < 3; ++j) {
            fireworkData[i][j] = -1;
        };
    };

    // prepare
    resetWall();
    color(brightnessData);
    showChangeScene();
    stopSound();

    flushStdin();
    while(true) {
        switch(choose) {
            case -1: // return to main menu
                stopSound();
                resetWall();
                isInGame = false;
                gameStarted = false;
                showChangeScene();
                disableCloseButton(false);
                playSound(soundMainmenu, false);
                flushStdin();
                return;
            case -2:  // new game
                choose = 0;
                stopSound();
                resetWall();
                score = 0;
                distance = wallCreateDistance - 4;
                x = 0;
                y = 0;
                isOver = 0;
                nextWall = 0;
                countWall = 0;
                highScoreIsScore = 0;
                countAnimation[0] = 1;
                countAnimation[1] = 0;
                highScore_ = getHighScore(score);
                countStart = rand() % sizeBackground;
                oldX = 0;
                gameStarted = 0;
                disableCloseButton(false);
                flushStdin();
                continue;
            case -3: // continue option in pausedMenu
                stopSound();
                gameStarted = true;
                choose = 0;
                continue;
        };

        pauseFrame();
        if(gameStarted) {
            maxY = maxUp + 1 - listWall[nextWall][1];
            minY = maxUp + 1 - listWall[nextWall][2];
            if ((y == -(terminalRows - sizeBird - maxUp - 1)) || (isOver) || (y > maxUp)) { // game over
                playSound(soundBirdDead, true);
                #ifndef _WIN32
                    y = y - 1;
                #endif
                showAnimation(NULL, skinDeadAnimation[settingsData[4]], sizeof(skinDeadAnimation[settingsData[4]]) / sizeof(skinDeadAnimation[settingsData[4]][0]), y);
                gameStarted = false;
                disableCloseButton(false);
                if (gameOver(score, y, minY, maxY)) {
                    choose = -1;
                    continue;
                } else {
                    choose = -2;
                };
            };
        };
        if ((settingsData[3]) && (gameStarted)) { // auto mode activated
            if ((minY == maxUp + 6) && (maxY == maxUp + 6)) { // maxUp + 1 - (-5)
                // skip due to noWall
                if (y < 1) {
                    playSound(soundBirdFlyUp, true);
                    y = y + 2;
                } else {
                    y = y - 1;
                };
            } else {
                i = minY + 2;
                switch(settingsData[19]) { // diff
                    case 3:
                    case 4:
                        i = i - 1;
                        break;
                };
                if (y <= i) {
                    playSound(soundBirdFlyUp, true);
                    y = y + 2;
                } else {
                    y = y - 1;
                };
            };
            choose = 0;
        } else if (choose == -9) { // received by SPACE button
            choose = 0;
            if(!gameStarted) {
                if (!settingsData[3]) {
                    disableCloseButton(true);
                };
                gameStarted = true;
                continue;
            };
            if (!settingsData[3]) {
                playSound(soundBirdFlyUp, true);
                if (y == maxUp - 1) {
                    y = y + 1;
                } else if (y < maxUp) {
                    y = y + 2;
                };
            };
        } else {
            if (gameStarted) {
                y = y - 1;
            };
        };
        if(gameStarted) {
            if((x - oldX) > 50) {
                countStart = countStart + 1;
                if(countStart > sizeBackground - 1) {
                    countStart = 0;
                };
                oldX = x;
            };
            if (x > 1000000000) {
                x = 0;
            } else {
                x = x + 1;
            };
            if (distance >= wallCreateDistance) {
                addWall(&countWall);
                distance = 0;
            };

            distance = distance + 1;
            if (!highScoreIsScore) {
                if (score > highScore_) {
                    highScore_ = getHighScore(score);
                };
                if (score > highScore_) {
                    highScoreIsScore = 1;
                };
            } else {
                highScore_ = score;
            };
        };

        text = "| ";
        if (gameStarted) {
            if (settingsData[3]) {
                text = text + "AUTO MODE";
            } else {
                text = text + "[" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> GO UP";
            };
        } else {
            text = text + "[" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> PLAY";
        };
        text = text + " | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> PAUSE |";
        if (settingsData[14]) {
            text = text + " Diff: " + getNameDifficult(settingsData[19]) + " | X: " + to_string(x) + " | Y: [" + to_string(minY) + "][" + to_string(y) + "][" + to_string(maxY) + "] |";
        };
        wipeOutput(output, sizeOutput);
        if (settingsData[13]) {
            showBackground(output, countStart, terminalRows - 3);
        };
        if (settingsData[12]) {
            if (gameStarted) {
                for(i = 0; i < settingsData[16]; ++i) {
                    showFirework(output, fireworkData[i], sizeOutput);
                };
            };
        };
        showAllWall(output, &nextWall, &score, countWall);
        showBird(output, countAnimation, sizeInAnimation, settingsData[4], y);
        showScore(output, score, highScore_, highScoreIsScore);
        output[sizeOutput - 1] = lineMap;
        showFPS(output);
        outputGame = getOutput(output, sizeOutput);

        // output
        cursorPos_up();
        cout << outputGame;

        bottomKeymap(text);
        inputMenu(&choose, 0, -3);
        checkWall(nextWall, y, maxUp, &isOver);
        __sleep__(speed);

        if ((!checkTerminalActive()) && (!settingsData[3]) && (gameStarted)) {
            launchPaused(&choose);
        };
    };
    return;
};

void inputMenu(int *chooseMenu, int max, int type_menu) {
    if (__kbhit__()) {
        int p[2];
        __getch(p);
        if ((p[1] == keymapData[0][1]) && (p[0] == keymapData[0][0])) /* UP */ {
            switch (type_menu) {
                case -8: // startOptions
                case -4: // pausedMenu
                case -2: // keymappingSettings
                case 0:  // mainMenu
                case 1:  // settingsMenu 
                case 3:  // resolutionSettings
                case 4:  // highScore
                case 5:  // changeSkin (bird)
                case 6:  // moreOptions
                case 7:  // moreSettingsMenu
                case 8:  // optionsSkin
                case 9:  // changeSkin (wall)
                case 10: // changeDifficult
                    if(*chooseMenu > 0) {
                        *chooseMenu = *chooseMenu - 1;
                    } else {
                        *chooseMenu = max;
                    };
                    break;
            };
            return;
        } else if ((p[1] == keymapData[2][1]) && (p[0] == keymapData[2][0])) /* LEFT */ {
            switch (type_menu) {
                case 2: // brightnessSettings
                    if(*chooseMenu > 1) {
                        *chooseMenu = *chooseMenu - 1;
                        setBrightness(*chooseMenu);
                    };
                    break;
                case 7:
                    switch(*chooseMenu) {
                        case 5:
                            setGameSpeed(settingsData[17] - 1);
                            break;
                    };
                    break;
            };
            return;
        } else if ((p[1] == keymapData[3][1]) && (p[0] == keymapData[3][0])) /* RIGHT */ {
            switch (type_menu) {
                case 2:
                    if(*chooseMenu <= max) {
                        *chooseMenu = *chooseMenu + 1;
                        setBrightness(*chooseMenu);
                    };
                    break;
                case 7:
                    switch(*chooseMenu) {
                        case 5:
                            setGameSpeed(settingsData[17] + 1);
                            break;
                    };
                    break;
            };
            return;
        } else if ((p[1] == keymapData[1][1]) && (p[0] == keymapData[1][0])) /* DOWN */ {
            switch (type_menu) {
                case -8:
                case -4:
                case -2:
                case 0:
                case 1:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                    if(*chooseMenu < max) {
                        *chooseMenu = *chooseMenu + 1;
                    } else {
                        *chooseMenu = 0;
                    };
                    break;
            };
            return;
        } else if ((p[1] == keymapData[6][1]) && (p[0] == keymapData[6][0])) /* ENTER */ {
            switch(type_menu) {
                case -8:
                    switch(*chooseMenu) {
                        case 0:
                            flappyBird();
                            *chooseMenu = -1;
                            break;
                        case 1:
                            boxInputIP();
                            break;
                        case 2:
                            *chooseMenu = -1;
                            break;
                    };
                    break;
                case -7:
                case -6:
                case -5:
                    *chooseMenu = -2;
                    break;
                case -4:
                    switch(*chooseMenu) {
                        case 0:
                            *chooseMenu = -1;
                            break;
                        case 1:
                            *chooseMenu = -3;
                            break;
                        case 2:
                            highScore();
                            break;
                        case 3:
                            settingsMenu();
                            break;
                        case 4:
                            *chooseMenu = -2;
                            break;
                        case 5:
                            exitProgram();
                            break;
                    };
                    break;
                case -2:
                    changeKeymapping(*chooseMenu);
                    break;
                case 0:
                    switch(*chooseMenu) {
                        case 0:
                            startOptions();
                            break;
                        case 1:
                            highScore();
                            break;
                        case 2:
                            optionsSkin();
                            break;
                        case 3:
                            moreOptions();
                            break;
                        case 4:
                            settingsMenu();
                            break;
                        case 5:
                            credit();
                            break;
                        case 6:
                            exitProgram();
                            break;
                    };
                    break;
                case 1:
                    switch(*chooseMenu) {
                        case 0:
                            if (settingsData[8]) {
                                settingsData[0] = !settingsData[0];
                                if (!isInGame) {
                                    playSound(soundMainmenu, false);
                                };
                                if (!settingsData[0]) {
                                    stopSound();
                                };
                                writeConfig("music", to_string(settingsData[0]));
                            } else {
                                errorBox("SOUND DISABLED", "", true);
                            };
                            break;
                        case 1:
                            if (settingsData[8]) {
                                settingsData[1] = !settingsData[1];
                                writeConfig("sfx", to_string(settingsData[1]));
                            } else {
                                errorBox("SOUND DISABLED", "", true);
                            };
                            break;
                        case 2:
                            brightnessSettings();
                            break;
                        case 3:
                            keymappingSettings();
                            break;
                        case 4:
                            if(isInGame) {
                                errorBox("Unavailable in game", "You must return to main menu first!", true);
                            } else {
                                if (settingsData[9]) {
                                    resolutionSettings();
                                } else {
                                    errorBox("RESOLUTION DISABLED", "", true);
                                };
                            };
                            break;
                        case 5: // diff
                            changeDifficult();
                            break;
                        case 6:
                            moreSettingsMenu();
                            break;
                        case 7:
                            *chooseMenu = -1;
                            break;
                    };
                    break;
                case 3:
                    if(*chooseMenu > -1) {
                        setResolution(*chooseMenu);
                    };
                    break;
                case 5:
                    if (*chooseMenu == sizeBirdAnimation) {
                        *chooseMenu = -1;
                    } else {
                        previewSkin(*chooseMenu, true);
                    };
                    break;
                case 6:
                    switch (*chooseMenu) {
                        case 0:
                            author();
                            break;
                        case 1:
                            *chooseMenu = -1;
                            break;
                    };
                    break;
                case 7:
                    switch (*chooseMenu) {
                        case 0:
                            if (settingsData[10]) {
                                if(isInGame) {
                                    errorBox("Unavailable in game", "You must return to main menu first!", true);
                                } else {
                                    bool i = false;
                                    if (settingsData[3]) {
                                        i = true;
                                    };
                                    if (!i) {
                                        if (showYesorNo("Do you want to enable auto mode?")) {
                                            i = true;
                                        };
                                    };
                                    
                                    if (i) {
                                        settingsData[3] = !settingsData[3];
                                        writeConfig("automode", to_string(settingsData[3]));
                                    };
                                };
                            } else {
                                errorBox("AUTO DISABLED", "", true);
                            };
                            break;
                        case 1:
                            settingsData[12] = !settingsData[12];
                            writeConfig("showfw", to_string(settingsData[12] + 1));
                            break;
                        case 2:
                            settingsData[13] = !settingsData[13];
                            writeConfig("showbg", to_string(settingsData[13] + 1));
                            break;
                        case 3:
                            settingsData[14] = !settingsData[14];
                            writeConfig("showxy", to_string(settingsData[14] + 1));
                            break;
                        case 4:
                            if (isInGame) {
                                errorBox("Unavailable in game", "You must return to main menu first!", true);
                            } else {
                                if (settingsData[12]) {
                                    int num;
                                    string p = "";
                                    
                                    p = showBoxInput("Number of firework [1 - 50]", "", "| [ESC] -> Back | [ENTER] -> Save | [<- / ->] Move cursor | [BACKS] -> Delete |" , 2, true);
                                    if (p == "") {
                                        return;
                                    };
                                    num = stoi(p);
                                    if ((num > 0) && (num < 51)) {
                                        settingsData[16] = num;
                                        writeConfig("totalfirework", to_string(settingsData[16]));
                                    } else {
                                        errorBox("Wrong number", "", true);
                                        break;
                                    };
                                } else {
                                    errorBox("Firework Disabled", "You must enable [Show firework] first!", true);
                                };
                            };
                            break;
                        case 5: // game speed
                            break;
                        case 6: // show FPS
                            settingsData[18] = !settingsData[18];
                            writeConfig("showfps", to_string(settingsData[18] + 1));
                            break;
                        case 7:
                            *chooseMenu = -1;
                            break;
                    };
                    break;
                case 8:
                    switch (*chooseMenu) {
                        case 0:
                            changeSkin(true);
                            break;
                        case 1:
                            changeSkin(false);
                            break;
                        case 2:
                            *chooseMenu = -1;
                            break;
                    };
                    break;
                case 9:
                    if (*chooseMenu == sizeSkinWall) {
                        *chooseMenu = -1;
                    } else {
                        previewSkin(*chooseMenu, false);
                    };
                    break;
                case 10:
                    if (*chooseMenu == 5) {
                        *chooseMenu = -1;
                        break;
                    };
                    settingsData[19] = *chooseMenu;
                    writeConfig("difficult", to_string(settingsData[19] + 1));
                    showBoxText("Completed", true);
                    bottomKeymap("Press any key to continue!");
                    anyKey();
                    *chooseMenu = -1;
                    break;
            };
            return;
        } else if ((p[1] == keymapData[4][1]) && (p[0] == keymapData[4][0])) /* ESC */ {
            switch(type_menu) {
                case -8:
                case -7:
                case -6:
                case -5:
                case -4:
                case -2:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                    *chooseMenu = -1;
                    break;
                case -3: // flappyBird
                    launchPaused(&*chooseMenu);
                    break;
            };
            return;
        } else if ((p[1] == keymapData[5][1]) && (p[0] == keymapData[5][0])) /* SPACE */ {
            switch(type_menu) {
                case -5:
                case 4:
                    *chooseMenu = -1;
                    break;
                case -3:
                    *chooseMenu = -9;
                    break;
                case -1: // loadingFrame
                    showTip("");
                    break;
            };
            return;
        };
        bool _check = false;
        #ifdef _WIN32
            if ((p[1] == 134) && (p[0])) {    // F12
                _check = true;
            };
        #else
            if ((p[1] == 52) && (p[0] == 168)) {    // F12
                _check = true;
            };
        #endif
        if (_check) {    // F12
            switch(type_menu) {
                case -2:
                    if (showYesorNo("Do you want to reset?")) {
                        resetKeymapData(true);
                        showBoxText("Reset completed", true);
                        bottomKeymap("Press any key to continue!");
                        anyKey();
                    };
                    break;
            };
        };
    };
    return;
};

void checkARG(int argc, char const *argv[]) {
    if (argc > 1) {
        setResolution(-1);
        int i, j;
        string arg;
        string tmp;
        string text = "";
        text = text + ">> Using debug arguments can affect performance and experience!\n";
        for(i = 1; i < argc; ++i) {
            arg = "";
            tmp = "INVALID";
            j = 0;
            // append char to string
            while(argv[i][j] != '\0') {
                arg = arg + argv[i][j];
                j = j + 1;
            };
            // check arg
            if ((arg == "-h" ) || (arg == "--help")) {
                clearTerminal();
                cout << " >> HELP: \n";
                cout << "COMMAND: " << argv[0] << " [ARG]\n";
                cout << "ARG: ";
                cout << "\t--disable-v2\t\t Disable v2 mode CMD\n";
                cout << "\t--disable-conf\t\t Disable config file (read/write)\n";
                cout << "\t--disable-color\t\t Disable color\n";
                cout << "\t--disable-sound\t\t Disable all sound (Music/SFX)\n";
                cout << "\t--disable-resolution\t Disable resolution (resize)\n";
                cout << "\t--disable-auto\t\t Disable auto mode\n";
                cout << "\t--disable-skin\t\t Disable skin\n";
                cout << "\nPRESS ANY KEY TO EXIT\n";
                flushStdin();
                __getch__();
                exit(-1);
            };
            if (arg == "--disable-v2") {
                settingsData[5] = false;
                tmp = "OK";
            };
            if (arg == "--disable-conf") {
                settingsData[6] = false;
                tmp = "OK";
            };
            if (arg == "--disable-color") {
                settingsData[7] = false;
                tmp = "OK";
            };
            if (arg == "--disable-sound") {
                settingsData[8] = false;
                settingsData[0] = false;
                settingsData[1] = false;
                tmp = "OK";
            };
            if (arg == "--disable-resolution") {
                settingsData[9] = false;
                tmp = "OK";
            };
            if (arg == "--disable-auto") {
                settingsData[3] = false;
                settingsData[10] = false;
                tmp = "OK";
            };
            if (arg == "--disable-skin") {
                settingsData[4] = 0;
                settingsData[11] = false;
                tmp = "OK";
            };
            text = text + "argument: [" + arg + "] => " + tmp + "\n";
        };
        clearTerminal();
        cout << text;
        cout << "\n>> Program will start in 5 seconds...\n";
        __sleep__(5250);
    };
    return;
};

void checkTerminalMode() {
    if (!settingsData[5]) {
        return;
    };
    setResolution(-1);
    clearTerminal();
    string text;
    string text2;
    int row, i, j;
    int p[2];

    int center = (terminalRows / 2) - 7;

    text = "";
    for(j = 0; j < terminalColumns; ++j) {
        text = text + ' ';
    };

    cursorPos_move(0, center);
    for(row = 0; row < 5; row++) {
        text2 = text;
        i = (terminalColumns / 2) - 5;
        for(j = 0; j < 10; ++j) {
            text2[i + j] = ':';
        };
        
        text2 = text2 + "\n";
        cout << text2;
    };

    text = "Is this a square?";
    cursorPos_move((terminalColumns / 2) - (text.length() / 2), terminalRows - 6);
    cout << text;
    
    text = "Input [y/n]: ";
    cursorPos_move((terminalColumns / 2) - (text.length() / 2), terminalRows - 4);
    cout << text;

    #ifndef _WIN32
        cout << flush;
    #endif
    
    while(true) {
        __getch(p);
        if (((p[1] == 'y') || (p[1] == 'Y')) && (!p[0])) {
            cout << "YES";
            #ifndef _WIN32
                cout << flush;
            #endif
            settingsData[5] = true;
            __sleep__(1000);
            return;
        };
        if (((p[1] == 'n') || (p[1] == 'N')) && (!p[0])) {
            cout << "NO";
            #ifndef _WIN32
                cout << flush;
            #endif
            settingsData[5] = false;
            __sleep__(1000);
            return;
        };
    };
    return;
};

int main(int argc, char const *argv[]) {
    #ifdef _WIN32
        SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); // Realtime priority Process
        SetForegroundWindow(consoleWindow); // Windows API
    #else
        system("clear");
        set_terminal_mode();
    #endif

    ios_base::sync_with_stdio(true); // Enable synchronization with stdio (slower performance)

    checkARG(argc, &*argv);

    #ifdef _WIN32
        checkTerminalMode();
    #endif

    configureTerminal();
    resetKeymapData(false);
    loadConfig();
    clearTerminal();
    resizeTerminal(terminalColumns, terminalRows);
    thread lockSizeTer(lockSizeTerminal);
    thread threadFPS(getFPS);

    srand(time(NULL));
    
    loadHighScore();

    __sleep__(1000);
    clearTerminal();

    banner();

    // Loading time... [cho đẹp thôi chứ k có load gì đâu =)))]
    showTip("");
    tmp_int[1] = 1;
    flushStdin();
    for(int i = 0; i <= 100; i = i + 2) {
        loadingFrame(i, true);
        inputMenu(&tmp_int[0], 0, -1);
        if(i < 20) {
            __sleep__(200);
        } else if(i < 70) {
            __sleep__(40);
        } else if (i < 90) {
            __sleep__(60);
        } else {
            __sleep__(120);
        };
    };
    
    mainMenu();
    return 0;
};