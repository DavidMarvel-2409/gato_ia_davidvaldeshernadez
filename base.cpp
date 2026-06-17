// tic-tac-toe
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <chrono>
#include <sys/resource.h>

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
#define dell "\033[1A\033[2K"

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
    // static const int N = 3;
    int M, N, K;
    // number of squares
    // static const int SIZE = N * N;
    vector<vector<signed char>> sq;
    // how pieces are displayed ... see below
    // P1, empty, P2
    static const array<char, 3> DISP;

    // initialize empty board
    // P1 to move
    State(int m, int n, int k) : M(m), N(n), K(k)
    {
        sq.assign(M, vector<signed char>(N, 0));
        to_move = P1;
        filled = 0;
    }

    // return true if board is full
    bool full() const
    {
        return filled >= M * N;
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
        for (int y = 0; y < M; ++y)
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
        assert(x >= 0 && x < N && y >= 0 && y < M);
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

        int dx[] = {1, 0, 1, 1};
        int dy[] = {0, 1, 1, -1};

        for (int y = 0; y < M; y++)
        {
            for (int x = 0; x < N; x++)
            {
                if (sq[y][x] != p)
                    continue;
                for (int d = 0; d < 4; d++)
                {
                    int count = 1;
                    for (int s = 1; s < K; s++)
                    {
                        int ny = y + dy[d] * s;
                        int nx = x + dx[d] * s;
                        if (ny < 0 || ny >= M || nx < 0 || nx >= N || sq[ny][nx] != p)
                            break;
                        count++;
                    }
                    if (count >= K)
                        return true;
                }
            }
        }
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
        for (int y = 0; y < M; y++)
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
    // array<array<signed char, N>, N> sq;

    // number of non-empty squares
    int filled;
};

// how pieces are displayed ...
// P1, empty, P2
const array<char, 3> State::DISP = {{'o', '-', 'x'}};

int heuristica(State &st)
{
    int score = 0,
        dx[] = {1, 0, 1, 1},
        dy[] = {0, 1, 1, -1};
    for (int y = 0; y < st.M; y++)
    {
        for (int x = 0; x < st.N; x++)
        {
            for (int d = 0; d < 4; d++)
            {
                int countP1 = 0, countP2 = 0;
                for (int s = 0; s < st.K; s++)
                {
                    int ny = y + dy[d] * s,
                        nx = x + dx[d] * s;
                    if (ny < 0 || ny >= st.M || nx < 0 || nx >= st.N)
                        break;
                    if (st.sq[ny][nx] == st.getPlayer(1))
                        countP1++;
                    else if (st.sq[ny][nx] == st.getPlayer(2))
                        countP2++;
                }
                if (countP1 > 0 && countP2 == 0)
                    score += countP1 * countP1;
                else if (countP2 > 0 && countP1 == 0)
                    score -= countP2 * countP2;
            }
        }
    }
    return score;
}

int evaluateMM(State &st)
{
    if (st.isWin(st.getPlayer(1)))
        return 100000;

    if (st.isWin(st.getPlayer(2)))
        return -100000;

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

int minmax(State &st, int depth, int &nodes)
{
    nodes++;
    // if (nodes % 100000 == 0)
    // {
    //     cout << "Nodos explorados: " << nodes << endl;
    // }
    int best = 0;
    if (st.isTerminal())
        return evaluateMM(st);
    if (depth == 0)
        return heuristica(st);
    auto player = st.get_to_move();
    if (player == st.getPlayer(1))
    {
        best = -999999;
        for (auto move : st.getMoves())
        {
            State jr = st;
            jr.make_move(move.second, move.first);
            best = theMax(best, minmax(jr, depth - 1, nodes));
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
            best = theMin(best, minmax(jr, depth - 1, nodes));
        }
        return best;
    }
}

int negamax(State &st, int depth, int &nodes)
{
    nodes++;
    if (st.isTerminal())
    {
        auto player = st.get_to_move();
        if (st.isWin(-player))
            return -100000;
        return 0;
    }
    if (depth == 0)
    {
        int h = heuristica(st);
        return st.get_to_move() == st.getPlayer(1) ? h : -h;
    }
    int best = -99999;
    for (auto move : st.getMoves())
    {
        State jr = st;
        jr.make_move(move.second, move.first);
        int score = -negamax(jr, depth - 1, nodes);
        best = theMax(best, score);
    }
    return best;
}

int alphabeta(State &st, int alpha, int beta, int depth, int &nodes, int &podas)
{
    nodes++;
    if (st.isTerminal())
        return evaluateMM(st);
    if (depth == 0)
        return heuristica(st);
    auto player = st.get_to_move();

    if (player == st.getPlayer(1))
    {
        int best = -999999;
        for (auto move : st.getMoves())
        {
            State jr = st;
            jr.make_move(move.second, move.first);
            int value = alphabeta(jr, alpha, beta, depth - 1, nodes, podas);
            best = theMax(best, value);
            alpha = theMax(alpha, best);

            if (alpha >= beta)
            {
                podas++;
                break;
            }
        }
        return best;
    }
    else
    {
        int best = 99999;
        for (auto move : st.getMoves())
        {
            State jr = st;
            jr.make_move(move.second, move.first);
            int value = alphabeta(jr, alpha, beta, depth - 1, nodes, podas);
            best = theMin(best, value);
            beta = theMin(beta, best);

            if (alpha >= beta)
            {
                podas++;
                break;
            }
        }
        return best;
    }
}

pair<int, int> agente_aleatorio(State &st)
{
    auto inicio = chrono::high_resolution_clock::now();
    pair<int, int> pos;
    do
    {
        pos.first = rand() % st.M;
        pos.second = rand() % st.N;
    } while (!st.isLegal(pos));
    auto fin = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(fin - inicio).count();
    cout << pink << "tiempo de agente aleatorio: " << ms << " ms" << RESET << endl;
    return pos;
}

pair<int, int> agente_minmax(State &st, int H)
{
    auto inicio = chrono::high_resolution_clock::now();
    int nodes = 0;
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
        int value = minmax(jr, H - 1, nodes);
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
    auto fin = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(fin - inicio).count();
    cout << pink << "tiempo de agente minmax: " << ms << " ms" << RESET << endl;
    cout << pink << "Nodos recorridos: " << nodes << RESET << endl;
    cout << pink << "profundidad de busqeueda: " << H << RESET << endl;
    return bestMove;
}

pair<int, int> agente_negamax(State &st, int H)
{
    int nodes = 0;
    auto inicio = chrono::high_resolution_clock::now();
    pair<int, int> bestMove;
    int bestValor = -99999;
    for (auto move : st.getMoves())
    {
        State jr = st;
        jr.make_move(move.second, move.first);
        int valor = -negamax(jr, H - 1, nodes);
        if (valor > bestValor)
        {
            bestValor = valor;
            bestMove = move;
        }
    }
    auto fin = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(fin - inicio).count();
    cout << pink << "tiempo de agente negamax: " << ms << " ms" << RESET << endl;
    cout << pink << "Nodos recorridos: " << nodes << RESET << endl;
    cout << pink << "profundidad de busqueda: " << H << RESET << endl;
    return bestMove;
}

pair<int, int> agente_alphabeta(State &st, int H)
{
    auto inicio = chrono::high_resolution_clock::now();
    int nodes = 0, podas = 0;
    pair<int, int> bestMove;
    int alpha = -99999;
    int beta = 99999;
    auto player = st.get_to_move();
    if (player == st.getPlayer(1))
    {
        int bestvalor = -99999;
        for (auto move : st.getMoves())
        {
            State jr = st;
            jr.make_move(move.second, move.first);
            int value = alphabeta(jr, alpha, beta, H - 1, nodes, podas);
            if (value > bestvalor)
            {
                bestvalor = value;
                bestMove = move;
            }
            alpha = theMax(alpha, bestvalor);
        }
    }
    else
    {
        int bestvalor = 99999;
        for (auto move : st.getMoves())
        {
            State jr = st;
            jr.make_move(move.second, move.first);
            int value = alphabeta(jr, alpha, beta, H - 1, nodes, podas);
            if (value < bestvalor)
            {
                bestvalor = value;
                bestMove = move;
            }
            beta = theMin(beta, bestvalor);
        }
    }
    auto fin = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(fin - inicio).count();
    cout << pink << "tiempo de agente alphabeta: " << ms << " ms" << RESET << endl;
    cout << pink << "Nodos recorridos: " << nodes << RESET << endl;
    cout << pink << "Podas: " << podas << RESET << endl;
    cout << pink << "profundidad de busqueda: " << H << RESET << endl;
    return bestMove;
}

int main(int argc, char *argv[])
{
    int M = atoi(argv[1]),
        N = atoi(argv[2]),
        K = atoi(argv[3]),
        H = atoi(argv[4]),
        ag1 = atoi(argv[5]),
        ag2 = atoi(argv[6]);

    struct rusage usage;
    srand(time(nullptr));
    State st(M, N, K);
    int winner = 0, turnos = 0;
    while (!st.full() && winner == 0)
    {
        pair<int, int> move = {0, 0};
        auto player = st.get_to_move();
        if (player == st.getPlayer(1))
        {
            switch (ag1)
            {
            case 0:
                st.print();
                cout << bold << green << "indique (fila columna)\nJugador: " << RESET << endl;
                do
                {
                    cin >> move.first >> move.second;
                    if (move.first >= 0 && move.first < st.M && move.second >= 0 && move.second < st.N && st.isLegal(move))
                    {
                        st.make_move(move.second, move.first);
                        turnos++;
                        break;
                    }
                    cout << red << bold << "Movimiento ilegal" << RESET << endl;
                    cin.clear();
                    cin.ignore(999, '\n');
                } while (true);
                break;
            case 1:
                cout << yellow << "Ejecutando agente aleatorio" << RESET << endl;
                move = agente_aleatorio(st);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            case 2:
                cout << yellow << "Ejecutando agente minmax" << RESET << endl;
                move = agente_minmax(st, H);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            case 3:
                cout << yellow << "Ejecutando agente negamax" << RESET << endl;
                move = agente_negamax(st, H);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            case 4:
                cout << yellow << "Ejecutando agente alphabeta" << RESET << endl;
                move = agente_alphabeta(st, H);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            }
        }
        else
        {
            switch (ag2)
            {
            case 0:
                st.print();
                cout << bold << green << "indique (fila columna)\nJugador: " << RESET << endl;
                do
                {
                    cin >> move.first >> move.second;
                    if (move.first >= 0 && move.first < st.M && move.second >= 0 && move.second < st.N && st.isLegal(move))
                    {
                        st.make_move(move.second, move.first);
                        turnos++;
                        break;
                    }
                    cout << red << bold << "Movimiento ilegal" << RESET << endl;
                    cin.clear();
                    cin.ignore(999, '\n');
                } while (true);
                break;
            case 1:
                cout << yellow << "Ejecutando agente aleatorio" << RESET << endl;
                move = agente_aleatorio(st);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            case 2:
                cout << yellow << "Ejecutando agente minmax" << RESET << endl;
                move = agente_minmax(st, H);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            case 3:
                cout << yellow << "Ejecutando agente negamax" << RESET << endl;
                move = agente_negamax(st, H);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            case 4:
                cout << yellow << "Ejecutando agente alphabeta" << RESET << endl;
                move = agente_alphabeta(st, H);
                st.make_move(move.second, move.first);
                turnos++;
                break;
            }
        }
        if (ag1 == 0 || ag2 == 0)
            st.print();
        if (st.isWin(player))
        {
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
    }
    cout << endl;
    for (int i = 0; i < 15; i++)
    {
        cout << yellow << Bloque << RESET;
    }
    cout << endl
         << endl;
    st.print();
    if (st.full() && winner == 0)
        cout << green << bold << "Lastima pero nadie gano :(" << RESET << endl;
    cout << pink << "Numero de turnos: " << turnos << RESET << endl;
    getrusage(RUSAGE_SELF, &usage);
    cout << pink << "Memoria usada: " << usage.ru_maxrss << " KB" << RESET << endl;
}