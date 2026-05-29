// tic-tac-toe
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>

// solo es para decorar
#define red "\033[31m"
#define green "\033[32m"
#define yellow "\033[33m"
#define blue "\033[34m"
#define pink "\033[35m"
#define black "\033[30m"
#define bold "\033[1m"
#define RESET "\033[0m"
#define Bloque "[+]"

using namespace std;

// thrown when set() encounters an illegal input
struct InputException
{
};

class State
{
public:
    // players
    enum Players
    {
        P2 = -1,
        P1 = 1
    };
    // edge length
    static const int N = 3;
    // number of squares
    static const int SIZE = N * N;
    // how pieces are displayed ... see below
    // P1, empty, P2
    static const array<char, 3> DISP;

    // initialize empty board
    // P1 to move
    State()
    {
        sq = {{}};
        to_move = P1;
        filled = 0;
    }

    // return true if board is full
    bool full() const
    {
        return filled >= SIZE;
    }

    // initialize state from string (P1 to move)
    // throw InputExceptiuon when you encounter
    // an error in s
    void set(const string &s)
    {
        // create input stream from string, now you can use >>, etc.
        // like for cin
        istringstream is(s);
        char c;

        to_move = P1;
        filled = 0;

        for (int y = 0; y < N; ++y)
        {
            for (int x = 0; x < N; ++x)
            {
                is >> c;
                if (!is)
                {
                    throw InputException();
                }
                if (c == DISP[1 + P1])
                {
                    sq[y][x] = P1;
                    ++filled;
                }
                else if (c == DISP[1 + P2])
                {
                    sq[y][x] = P2;
                    ++filled;
                }
                else
                {
                    sq[y][x] = 0;
                }
            }
        }
        is >> c;
        if (is)
        {
            // trailing character(s)
            throw InputException();
        }
    }

    // print state to cout
    // format:
    //
    //  xox
    //  o-x
    //  xxo
    //  x (8)
    //
    // last line: player to move, number of filled squares
    // followed by new-line
    void print() const
    {
        for (int i = 0; i < N * 2 + 1; i++)
        {
            cout << blue << Bloque << RESET;
        }
        cout << endl;
        for (int y = 0; y < N; ++y)
        {
            cout << blue << Bloque << RESET;
            for (int x = 0; x < N; ++x)
            {
                cout << black << "[" << RESET << DISP[sq[y][x] + 1] << black << "]" << blue << Bloque << RESET;
            }
            cout << endl;

            for (int i = 0; i < N * 2 + 1; i++)
            {
                cout << blue << Bloque << RESET;
            }
            cout << endl;
        }
        // print player to move and #filled squares
        cout << DISP[to_move + 1]
             << " (" << filled << ")"
             << endl;
    }

    // make move (x, y) for player to_move
    // and return true iff move is legal
    // pre-condition: x, y within range
    bool make_move(int x, int y)
    {
        assert(x >= 0 && x < N && y >= 0 && y < N);
        auto &c = sq[y][x];
        if (c)
        {
            return false; // already occupied
        }

        c = to_move;
        to_move = -to_move;
        ++filled;
        return true;
    }

    // return player to move
    int get_to_move() const
    {
        return to_move;
    }

    bool isWin(int p)
    {
        // hay 8 posibles formas de ganar en un 3*3

        if (sq[0][0] == p && sq[0][1] == p && sq[0][2] == p)
            return true;
        else if (sq[1][0] == p && sq[1][1] == p && sq[1][2] == p)
            return true;
        else if (sq[2][0] == p && sq[2][1] == p && sq[2][2] == p)
            return true;
        else if (sq[0][0] == p && sq[1][0] == p && sq[2][0] == p)
            return true;
        else if (sq[0][1] == p && sq[1][1] == p && sq[2][1] == p)
            return true;
        else if (sq[0][2] == p && sq[1][2] == p && sq[2][2] == p)
            return true;
        else if (sq[0][0] == p && sq[1][1] == p && sq[2][2] == p)
            return true;
        else if (sq[0][2] == p && sq[1][1] == p && sq[2][0] == p)
            return true;

        return false;
    }

private:
    // P1 or P2 to move
    int to_move;

    // squares (2d array)
    // stores P1,0,P2 values
    // bounds are checked in debug mode
    // and sq[y][x] = 0 works
    array<array<signed char, N>, N> sq;

    // number of non-empty squares
    int filled;
};

// how pieces are displayed ...
// P1, empty, P2
const array<char, 3> State::DISP = {{'o', '-', 'x'}};

int main()
{
    // std::vector<string> boards{
    //     "---
    //  ---
    //  ---", // x draws with (0,0)

    //     "---
    //  -o-
    //  ---", // x draws with (0,0)

    //     "---
    //  --o
    //  ---", // x draws with (0,1)

    //     "-oo
    //  ooo
    //  ooo", // a player already won

    //     "xox
    //  oxo
    //  oxo", // draw

    //     "--o
    //  -oo
    //  ---", // x loses

    //     "oxo
    //  xx-
    //  oox", // x wins with (2,1)

    //     "-xo
    //  oox
    //  xoo", // x draws with (0,0)

    //     "---
    //  -x-
    //  ---" // x wins with (0,0)
    // };

    // for (const string &s : boards)
    // {
    //     // construct state from string
    //     // also catch exception and print
    //     // error message
    //     try
    //     {
    //         State st;
    //         st.set(s);
    //         cout << green << "Before move" << RESET << endl;
    //         st.print();
    //         cout << green << "After move. Status: " << st.make_move(0, 0) << RESET << endl;
    //         st.print();
    //         cout << yellow << "===============================" << RESET << endl;
    //         cout << endl;
    //     }
    //     catch (InputException &e)
    //     {
    //         cerr << "corrupt input: " << s << endl;
    //     }
    // }
    State st;
    int winner = 0;
    for (int i = 0; i < 15; i++)
    {
        cout << yellow << Bloque << RESET;
    }
    cout << endl;
    cout << bold << "Hola, seleccione los ajentes" << green << "\nagente humano 1\nagente aleatorio: 2\nX: " << RESET;
    int agente1 = 0, agente2 = 0;
    cin >> agente1;
    cout << bold << green << "O: " << RESET;
    cin >> agente2;

    for (int i = 0; i < 15; i++)
    {
        cout << yellow << Bloque << RESET;
    }
    cout << bold << "\nPerfecto, ahora comenzamos\n"
         << RESET;
    do
    {
        pair<int, int> move = {0, 0};
        st.print();
        if (st.get_to_move() == State::P1)
        {
            if (agente1 == 1)
            {
                cout << bold << green << "\nindique (x y)\nJugador X: " << RESET << endl;
                cin >> move.first >> move.second;
                st.make_move(move.first, move.second);
            }
            else
            {
                // movimiento aleatorio
            }
        }
        else
        {
            if (agente2 == 1)
            {
                cout << bold << green << "indique (x y)\nJugador O: " << RESET << endl;
                cin >> move.first >> move.second;
                st.make_move(move.first, move.second);
            }
            else
            {
                // movimiento aleatorio
            }
        }
        for (int i = 0; i < 15; i++)
        {
            cout << yellow << Bloque << RESET;
        }
        cout << endl;
        if (st.isWin(st.get_to_move()))
        {
            winner = st.get_to_move();
        }
    } while (!st.full() && winner != 0);

    cout << "gano\n";
}