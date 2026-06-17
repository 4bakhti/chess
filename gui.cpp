#ifdef _WIN32

#include <windows.h>
#include <windowsx.h>
#include <string>
#include <sstream>
#include <vector>
#include "board.hpp"

namespace {
constexpr int kBoardSize = 640;
constexpr int kStatusHeight = 72;
constexpr int kMargin = 24;
constexpr int kHistoryWidth = 220;
constexpr int kPanelGap = 20;
constexpr int kWindowWidth = kBoardSize + kHistoryWidth + kPanelGap + 2 * kMargin;
constexpr int kCellSize = kBoardSize / 8;

Board board;
bool whiteTurn = true;
bool gameOver = false;
int selectedRow = -1;
int selectedCol = -1;
std::wstring statusMessage = L"White to move";
std::vector<std::wstring> moveHistory;

COLORREF lightSquare = RGB(238, 238, 210);
COLORREF darkSquare = RGB(118, 150, 86);
COLORREF selectedSquare = RGB(246, 211, 101);
COLORREF legalHint = RGB(80, 120, 64);
COLORREF borderColor = RGB(42, 55, 38);
COLORREF panelBackground = RGB(232, 229, 214);

bool pointToSquare(int x, int y, int& row, int& col) {
    x -= kMargin;
    y -= kMargin;
    if (x < 0 || y < 0 || x >= kBoardSize || y >= kBoardSize) return false;
    col = x / kCellSize;
    row = y / kCellSize;
    return true;
}

std::wstring pieceGlyph(Piece* piece) {
    if (!piece) return L"";

    if (dynamic_cast<King*>(piece)) return L"\u265A";
    if (dynamic_cast<Queen*>(piece)) return L"\u265B";
    if (dynamic_cast<Rook*>(piece)) return L"\u265C";
    if (dynamic_cast<Bishop*>(piece)) return L"\u265D";
    if (dynamic_cast<Knight*>(piece)) return L"\u265E";
    if (dynamic_cast<Pawn*>(piece)) return L"\u265F";
    return L"?";
}

std::wstring squareName(int row, int col) {
    std::wstring name;
    name += static_cast<wchar_t>(L'a' + col);
    name += static_cast<wchar_t>(L'8' - row);
    return name;
}

std::wstring pieceLetter(Piece* piece) {
    if (!piece || dynamic_cast<Pawn*>(piece)) return L"";
    if (dynamic_cast<King*>(piece)) return L"K";
    if (dynamic_cast<Queen*>(piece)) return L"Q";
    if (dynamic_cast<Rook*>(piece)) return L"R";
    if (dynamic_cast<Bishop*>(piece)) return L"B";
    if (dynamic_cast<Knight*>(piece)) return L"N";
    return L"";
}

std::wstring buildMoveText(Piece* movingPiece, Piece* targetPiece, int fromRow, int fromCol, int toRow, int toCol) {
    const bool whiteMove = movingPiece->isWhitePiece();
    const bool pawnMove = dynamic_cast<Pawn*>(movingPiece) != nullptr;
    const bool castle = dynamic_cast<King*>(movingPiece) && abs(toCol - fromCol) == 2;
    const bool enPassantCapture = pawnMove && fromCol != toCol && targetPiece == nullptr;
    const bool capture = targetPiece != nullptr || enPassantCapture;

    std::wstring text;
    if (whiteMove) {
        std::wstringstream number;
        number << (moveHistory.size() / 2 + 1) << L". ";
        text += number.str();
    } else {
        text += L"... ";
    }

    if (castle) {
        text += toCol > fromCol ? L"O-O" : L"O-O-O";
        return text;
    }

    text += pieceLetter(movingPiece);
    if (pawnMove && capture) {
        text += static_cast<wchar_t>(L'a' + fromCol);
    }

    if (capture) text += L"x";
    text += squareName(toRow, toCol);

    int promotionRow = whiteMove ? 0 : 7;
    if (pawnMove && toRow == promotionRow) text += L"=Q";

    return text;
}

bool isLegalDestination(int fromRow, int fromCol, int toRow, int toCol) {
    Piece* piece = board.getPiece(fromRow, fromCol);
    if (!piece || piece->isWhitePiece() != whiteTurn) return false;

    Board testBoard(board);
    return testBoard.movePiece(fromRow, fromCol, toRow, toCol, true);
}

void updateGameStatus() {
    if (board.isCheckmate(whiteTurn)) {
        gameOver = true;
        statusMessage = whiteTurn ? L"Black wins by checkmate" : L"White wins by checkmate";
        return;
    }

    if (board.isStalemate(whiteTurn)) {
        gameOver = true;
        statusMessage = L"Stalemate. Game is a draw";
        return;
    }

    statusMessage = whiteTurn ? L"White to move" : L"Black to move";
    if (board.isInCheck(whiteTurn)) statusMessage += L" - check";
}

void resetGame(HWND hwnd) {
    board.initialize();
    whiteTurn = true;
    gameOver = false;
    selectedRow = -1;
    selectedCol = -1;
    moveHistory.clear();
    updateGameStatus();
    InvalidateRect(hwnd, nullptr, TRUE);
}

void trySelectOrMove(HWND hwnd, int row, int col) {
    if (gameOver) return;

    Piece* clicked = board.getPiece(row, col);
    if (selectedRow == -1) {
        if (clicked && clicked->isWhitePiece() == whiteTurn) {
            selectedRow = row;
            selectedCol = col;
            statusMessage = L"Selected " + squareName(row, col);
        }
        InvalidateRect(hwnd, nullptr, TRUE);
        return;
    }

    if (row == selectedRow && col == selectedCol) {
        selectedRow = -1;
        selectedCol = -1;
        updateGameStatus();
        InvalidateRect(hwnd, nullptr, TRUE);
        return;
    }

    if (clicked && clicked->isWhitePiece() == whiteTurn) {
        selectedRow = row;
        selectedCol = col;
        statusMessage = L"Selected " + squareName(row, col);
        InvalidateRect(hwnd, nullptr, TRUE);
        return;
    }

    const std::wstring from = squareName(selectedRow, selectedCol);
    const std::wstring to = squareName(row, col);
    Piece* movingPiece = board.getPiece(selectedRow, selectedCol);
    Piece* targetPiece = board.getPiece(row, col);
    std::wstring moveText = buildMoveText(movingPiece, targetPiece, selectedRow, selectedCol, row, col);

    if (board.movePiece(selectedRow, selectedCol, row, col, true)) {
        whiteTurn = !whiteTurn;
        selectedRow = -1;
        selectedCol = -1;
        updateGameStatus();
        if (gameOver && board.isCheckmate(whiteTurn)) moveText += L"#";
        else if (board.isInCheck(whiteTurn)) moveText += L"+";
        moveHistory.push_back(moveText);
    } else {
        statusMessage = L"Invalid move: " + from + L" to " + to;
    }
    InvalidateRect(hwnd, nullptr, TRUE);
}

void drawCenteredText(HDC hdc, const std::wstring& text, const RECT& rect, HFONT font, COLORREF color) {
    HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    DrawTextW(hdc, text.c_str(), -1, const_cast<RECT*>(&rect), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
}

void drawPiece(HDC hdc, Piece* piece, const RECT& square, HFONT font) {
    const std::wstring glyph = pieceGlyph(piece);
    if (piece->isWhitePiece()) {
        RECT shadowRect = square;
        OffsetRect(&shadowRect, 1, 1);
        drawCenteredText(hdc, glyph, shadowRect, font, RGB(34, 40, 30));
        drawCenteredText(hdc, glyph, square, font, RGB(252, 252, 236));
        return;
    }

    drawCenteredText(hdc, glyph, square, font, RGB(18, 24, 16));
}

void drawBoard(HDC hdc) {
    HPEN borderPen = CreatePen(PS_SOLID, 2, borderColor);
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, borderPen));

    HFONT pieceFont = CreateFontW(58, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Symbol");
    HFONT labelFont = CreateFontW(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            RECT square = {
                kMargin + col * kCellSize,
                kMargin + row * kCellSize,
                kMargin + (col + 1) * kCellSize,
                kMargin + (row + 1) * kCellSize
            };

            COLORREF fill = ((row + col) % 2 == 0) ? lightSquare : darkSquare;
            if (row == selectedRow && col == selectedCol) fill = selectedSquare;

            HBRUSH brush = CreateSolidBrush(fill);
            FillRect(hdc, &square, brush);
            DeleteObject(brush);

            if (selectedRow != -1 && !(row == selectedRow && col == selectedCol) &&
                isLegalDestination(selectedRow, selectedCol, row, col)) {
                HBRUSH hintBrush = CreateSolidBrush(legalHint);
                HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, hintBrush));
                int centerX = square.left + kCellSize / 2;
                int centerY = square.top + kCellSize / 2;
                int radius = board.getPiece(row, col) ? 20 : 10;
                Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
                SelectObject(hdc, oldBrush);
                DeleteObject(hintBrush);
            }

            Piece* piece = board.getPiece(row, col);
            if (piece) {
                drawPiece(hdc, piece, square, pieceFont);
            }
        }
    }

    HBRUSH borderBrush = CreateSolidBrush(borderColor);
    RECT borderRect = {kMargin, kMargin, kMargin + kBoardSize, kMargin + kBoardSize};
    FrameRect(hdc, &borderRect, borderBrush);
    DeleteObject(borderBrush);
    SelectObject(hdc, oldPen);

    for (int i = 0; i < 8; ++i) {
        RECT fileRect = {kMargin + i * kCellSize, kMargin + kBoardSize, kMargin + (i + 1) * kCellSize, kMargin + kBoardSize + 22};
        std::wstring file(1, static_cast<wchar_t>(L'a' + i));
        drawCenteredText(hdc, file, fileRect, labelFont, borderColor);

        RECT rankRect = {2, kMargin + i * kCellSize, kMargin - 4, kMargin + (i + 1) * kCellSize};
        std::wstring rank(1, static_cast<wchar_t>(L'8' - i));
        drawCenteredText(hdc, rank, rankRect, labelFont, borderColor);
    }

    DeleteObject(pieceFont);
    DeleteObject(labelFont);
    DeleteObject(borderPen);
}

void drawStatus(HDC hdc, HWND hwnd) {
    HFONT statusFont = CreateFontW(22, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                   OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                   DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT smallFont = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    RECT statusRect = {kMargin, kMargin + kBoardSize + 24, kMargin + kBoardSize, kMargin + kBoardSize + 52};
    drawCenteredText(hdc, statusMessage, statusRect, statusFont, RGB(28, 34, 24));

    RECT helpRect = {kMargin, kMargin + kBoardSize + 50, kMargin + kBoardSize, kMargin + kBoardSize + 72};
    std::wstring help = L"Click a piece, then click a destination. Press R to reset or Esc to quit.";
    drawCenteredText(hdc, help, helpRect, smallFont, RGB(76, 83, 70));

    DeleteObject(statusFont);
    DeleteObject(smallFont);
}

void drawMoveHistory(HDC hdc) {
    RECT panel = {
        kMargin + kBoardSize + kPanelGap,
        kMargin,
        kMargin + kBoardSize + kPanelGap + kHistoryWidth,
        kMargin + kBoardSize
    };

    HBRUSH panelBrush = CreateSolidBrush(panelBackground);
    FillRect(hdc, &panel, panelBrush);
    DeleteObject(panelBrush);

    HBRUSH borderBrush = CreateSolidBrush(borderColor);
    FrameRect(hdc, &panel, borderBrush);
    DeleteObject(borderBrush);

    HFONT titleFont = CreateFontW(21, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT moveFont = CreateFontW(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, L"Consolas");

    RECT titleRect = {panel.left + 14, panel.top + 14, panel.right - 14, panel.top + 44};
    drawCenteredText(hdc, L"Move History", titleRect, titleFont, RGB(28, 34, 24));

    HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, moveFont));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(36, 42, 31));

    const int firstVisible = moveHistory.size() > 24 ? static_cast<int>(moveHistory.size()) - 24 : 0;
    int y = panel.top + 62;
    for (int i = firstVisible; i < static_cast<int>(moveHistory.size()); ++i) {
        RECT rowRect = {panel.left + 16, y, panel.right - 16, y + 22};
        DrawTextW(hdc, moveHistory[i].c_str(), -1, &rowRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        y += 23;
    }

    if (moveHistory.empty()) {
        RECT emptyRect = {panel.left + 16, panel.top + 62, panel.right - 16, panel.top + 96};
        DrawTextW(hdc, L"No moves yet", -1, &emptyRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(hdc, oldFont);
    DeleteObject(titleFont);
    DeleteObject(moveFont);
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            resetGame(hwnd);
            return 0;

        case WM_LBUTTONDOWN: {
            int row = -1;
            int col = -1;
            if (pointToSquare(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), row, col)) {
                trySelectOrMove(hwnd, row, col);
            }
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == 'R') resetGame(hwnd);
            if (wParam == VK_ESCAPE) DestroyWindow(hwnd);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HBRUSH background = CreateSolidBrush(RGB(242, 239, 228));
            FillRect(hdc, &ps.rcPaint, background);
            DeleteObject(background);
            drawBoard(hdc);
            drawMoveHistory(hdc);
            drawStatus(hdc, hwnd);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int commandShow) {
    const wchar_t className[] = L"CppChessGuiWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = windowProc;
    wc.hInstance = instance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    RECT windowRect = {0, 0, kWindowWidth, kBoardSize + kStatusHeight + 2 * kMargin + 28};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowExW(
        0,
        className,
        L"C++ Chess",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, commandShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

#else
int main() {
    return 0;
}
#endif
