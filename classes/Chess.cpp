#include "Chess.h"
#include <limits>
#include <cmath>
#include <ctype.h>
Chess::Chess()
{
    _grid = new Grid(8, 8);
}

Chess::~Chess()
{
    delete _grid;
}

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid->getSquare(x, y)->bit();
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    startGame();
}

ChessPiece charToPieceType(char letter){
    if(__ascii_toupper(letter) == 'P'){
        return ChessPiece::Pawn;
    }
    else if(__ascii_toupper(letter) == 'N'){
        return ChessPiece::Knight;
    }
    else if(__ascii_toupper(letter) == 'B'){
        return ChessPiece::Bishop;
    }
    else if(__ascii_toupper(letter) == 'R'){
        return ChessPiece::Rook;
    }
    else if(__ascii_toupper(letter) == 'Q'){
        return ChessPiece::Queen;
    }
    else if(__ascii_toupper(letter) == 'K'){
        return ChessPiece::King;
    }
    return ChessPiece::NoPiece;
}

void Chess::FENtoBoard(const std::string& fen) {

    ////
    // Esther helper notes:
        // The space at 0x0 is top left, and 7x7 is bottom right.
        // We move through each row left to right, and each column top to down.

        // if that coord is in piece notation,
        // use pieceForPlayer to set and load the piece for that char
        
        // if char's unicode val represents a lowecase letter, add the black piece it represents
        // if char's unicode val represents an uppercase letter, add the white piece it represents
        // if char's unicode val represents a number (can be 1 to 8) skipCount, increment i by skipCount minus 1 (because we still increment at the end of the loop)
        // if the next character is a slash "/", we can move up a column
    ////

    int fenLen = fen.length();
    int x = 0;
    int y = 0;
    for(int i = 0; i < fenLen; i++){
        // if the fen value is a slash, increment y and reset x to 0
        if(fen[i] == '/'){
            y += 1;
            x = 0;
            continue;
        }
        // else if the fen value is a digit, increment x by that amount;
        else if(__ascii_isdigit(fen[i])){
            //std::cout << "digit: " << fen[i] << std::endl;
            char offsetForNum = '0';
            int skipCount = fen[i] - offsetForNum;
            //std::cout << "skipCount: " << skipCount << std::endl;
            x += skipCount;
            continue;
        }
        // else if the fen value is a letter,
        // add a piece to the board, given the letter's case and alphabet letter
        else if(__ascii_isalpha(fen[i])){
            // leaving this here to show how isupper() and islower() from <ctype.h> help
            // std::cout << "alpha(betic): " << fen[i] << std::endl;
            // if(isupper(fen[i])){
            //     std::cout << "alpha(betic) upper: " << fen[i] << std::endl;
            // }
            // else if(islower(fen[i])){
            //     std::cout << "alpha(betic) lower: " << fen[i] << std::endl;
            // }
            int player = isupper(fen[i]) ? 0 : 1; // 0 for player one and white, 1 for player two and black
            ChessPiece pieceType = charToPieceType(fen[i]);
            //std::cout << pieceType << std::endl;
            Bit* bit = PieceForPlayer(player, charToPieceType(fen[i]));
            if(bit){
                // subtracting y from 7 helps us set the white pieces at the bottom instead of the top,
                // and same idea for the black pieces
                bit->setPosition(_grid->getSquare(x, 7-y)->getPosition());
                _grid->getSquare(x, 7-y)->setBit(bit);
            }
        }
        x++;
    }

    // convert a FEN string to a board
    // FEN is a space delimited string with 6 fields
    // 1: piece placement (from white's perspective)
    // NOT PART OF THIS ASSIGNMENT BUT OTHER THINGS THAT CAN BE IN A FEN STRING
    // ARE BELOW
    // 2: active color (W or B)
    // 3: castling availability (KQkq or -)
    // 4: en passant target square (in algebraic notation, or -)
    // 5: halfmove clock (number of halfmoves since the last capture or pawn advance)
}

bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber() * 128;
    int pieceColor = bit.gameTag() & 128;
    if (pieceColor == currentPlayer) return true;
    return false;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return true;
}

void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            s += pieceNotation( x, y );
        }
    );
    return s;}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}
