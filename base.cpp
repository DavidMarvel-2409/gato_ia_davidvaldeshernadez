// tic-tac-toe
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <cstdlib>
#include <ctime>

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
                cout << black << "[" << yellow << DISP[sq[y][x] + 1] << black << "]" << blue << Bloque << RESET;
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
    bool isLegal(pair<int, int> &pos)
    {
        if (sq[pos.first][pos.second] == 0)
            return true;
        return false;
    }
    vector<pair<int, int>> getMoves() const
    {
        vector<pair<int, int>> moves;
        for (int y = 0; y < N; y++)
        {
            for (int x = 0; x < N; x++)
            {
                if (sq[y][x] == 0)
                {
                    moves.push_back({y, x});
                }
            }
        }
        return moves;
    }
    bool isTerminal()
    {
        return isWin(P1) || isWin(P2) || full();
    }
    Players getPlayer(int n)
    {
        return n == 1 ? P1 : P2;
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

int evaluateMM(State &st)
{
    if (st.isWin(st.getPlayer(1)))
        return 1;

    if (st.isWin(st.getPlayer(2)))
        return -1;

    return 0;
}

int theMin(int a, int b)
{
    return a < b ? a : b;
}
int theMax(int a, int b)
{
    return a > b ? a : b;
}

int minmax(State &st)
{
    int best = 0;
    if (st.isTerminal())
        return evaluateMM(st);
    auto player = st.get_to_move();
    if (player == st.getPlayer(1))
    {
        best = -999999;
        for (auto move : st.getMoves())
        {
            State jr = st;
            jr.make_move(move.second, move.first);
            best = theMax(best, minmax(jr));
        }
        return best;
    }
    else
    {
        best = 999999;
        for (auto move : st.getMoves())
        {
            State jr = st;
            jr.make_move(move.second, move.first);
            best = theMin(best, minmax(jr));
        }
        return best;
    }
}

int negamax(State &st)
{
    if (st.isTerminal())
    {
        auto player = st.get_to_move();
        if (st.isWin(-player))
            return -1;
        return 0;
    }
    int best = -99999;
    for (auto move : st.getMoves())
    {
        State jr = st;
        jr.make_move(move.second, move.first);
        int score = -negamax(jr);
        best = theMax(best, score);
    }
    return best;
}

pair<int, int> agente_aleatorio(State &st)
{
    pair<int, int> pos;
    do
    {
        pos.first = rand() % State::N;
        pos.second = rand() % State::N;
    } while (!st.isLegal(pos));
    return pos;
}

pair<int, int> agente_minmax(State &st)
{
    pair<int, int> bestMove;
    int bestValor;
    if (st.get_to_move() == st.getPlayer(1))
        bestValor = -99999;
    else
        bestValor = 99999;
    for (auto move : st.getMoves())
    {
        State jr = st;
        jr.make_move(move.second, move.first);
        int value = minmax(jr);
        if (st.get_to_move() == st.getPlayer(1))
        {
            if (value > bestValor)
            {
                bestValor = value;
                bestMove = move;
            }
        }
        else
        {
            if (value < bestValor)
            {
                bestValor = value;
                bestMove = move;
            }
        }
    }
    return bestMove;
}

pair<int, int> agente_negamax(State &st)
{
    pair<int, int> bestMove;
    int bestValor = -99999;
    for (auto move : st.getMoves())
    {
        State jr = st;
        jr.make_move(move.second, move.first);
        int valor = -negamax(jr);
        if (valor > bestValor)
        {
            bestValor = valor;
            bestMove = move;
        }
    }
    return bestMove;
}

int main()
{

    srand(time(nullptr));
    State st;
    int winner = 0;
    for (int i = 0; i < 15; i++)
    {
        cout << yellow << Bloque << RESET;
    }
    cout << endl;
    cout << bold << "Hola, seleccione los ajentes" << green << "\nagente humano 1\nagente aleatorio: 2\nagente MinMax: 3\nagente negamax: 4\nX: " << RESET;
    int agente1 = 0, agente2 = 0;
    do
    {
        cin >> agente1;
        if (agente1 >= 1 && agente1 <= 4)
        {
            break;
        }
        cout << red << bold << "\nAgente no reconosido" << RESET << endl;
        cin.clear();
    } while (true);
    cout << bold << green << "O: " << RESET;
    do
    {
        cin >> agente2;
        if (agente2 >= 1 && agente2 <= 4)
        {
            break;
        }
        cout << red << bold << "\nAgente no reconosido" << RESET << endl;
        cin.clear();
    } while (true);

    for (int i = 0; i < 15; i++)
    {
        cout << yellow << Bloque << RESET;
    }
    cout << bold << "\nPerfecto, ahora comenzamos" << RESET << endl;
    do
    {
        pair<int, int> move = {0, 0};
        st.print();
        auto player = st.get_to_move();
        if (player == st.getPlayer(1))
        {
            switch (agente1)
            {
            case 1:
                cout << bold << green << "\nindique (x y)\nJugador X: " << RESET << endl;
                do
                {
                    cin >> move.second >> move.first;
                    if (move.first >= 0 && move.first <= 2 && move.second >= 0 && move.second <= 2 && st.isLegal(move))
                    {
                        st.make_move(move.second, move.first);
                        break;
                    }
                    cout << red << bold << "\nMovimiento ilegal" << RESET << endl;
                    cin.clear();
                } while (true);
                break;
            case 2:
                move = agente_aleatorio(st);
                st.make_move(move.second, move.first);
                break;
            case 3:
                move = agente_minmax(st);
                st.make_move(move.second, move.first);
                break;
            case 4:
                move = agente_negamax(st);
                st.make_move(move.second, move.first);
                break;
            }
        }
        else
        {
            switch (agente2)
            {
            case 1:
                cout << bold << green << "\nindique (x y)\nJugador X: " << RESET << endl;
                do
                {
                    cin >> move.second >> move.first;
                    if (move.first >= 0 && move.first <= 2 && move.second >= 0 && move.second <= 2 && st.isLegal(move))
                    {
                        st.make_move(move.second, move.first);
                        break;
                    }
                    cout << red << bold << "\nMovimiento ilegal" << RESET << endl;
                    cin.clear();
                } while (true);
                break;
            case 2:
                move = agente_aleatorio(st);
                st.make_move(move.second, move.first);
                break;
            case 3:
                move = agente_minmax(st);
                st.make_move(move.second, move.first);
                break;
            case 4:
                move = agente_negamax(st);
                st.make_move(move.second, move.first);
                break;
            }
        }
        for (int i = 0; i < 15; i++)
        {
            cout << yellow << Bloque << RESET;
        }
        cout << endl;
        if (st.isWin(player))
        {
            st.print();
            winner = player;
            if (player == 1)
            {
                cout << green << bold << "Felicidades jugador: " << yellow << "X" << RESET << endl;
            }
            else
            {
                cout << green << bold << "Felicidades jugador: " << yellow << "O" << RESET << endl;
            }
        }
    } while (!st.full() && winner == 0);

    if (st.full() && winner == 0)
        cout << green << bold << "Lastima pero nadie gano :(" << RESET << endl;

    for (int i = 0; i < 15; i++)
    {
        cout << yellow << Bloque << RESET;
    }
    cout << endl;
    st.print();
}