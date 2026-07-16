#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

enum TokenState { IN_BASE, ON_TRACK, IN_HOME_STRETCH, FINISHED };
enum PlayerColor { RED, GREEN, YELLOW, BLUE };

const string COLOR_NAMES[] = {"Red", "Green", "Yellow", "Blue"};
const int START_POSITIONS[] = {0, 13, 26, 39};

struct Token {
    int id;
    TokenState state;
    int absolutePosition; // Position on the shared 0-51 track
    int stepsTaken;       // Total steps taken (0-56). 51 = enters home stretch, 57 = finished

    Token(int i) : id(i), state(IN_BASE), absolutePosition(-1), stepsTaken(0) {}
};

class Player {
public:
    PlayerColor color;
    vector<Token> tokens;
    bool hasFinished;

    Player(PlayerColor c) : color(c), hasFinished(false) {
        for (int i = 0; i < 4; ++i) {
            tokens.push_back(Token(i + 1));
        }
    }

    bool checkFinished() {
        for (const auto& t : tokens) {
            if (t.state != FINISHED) return false;
        }
        hasFinished = true;
        return true;
    }
};

class LudoGame {
private:
    vector<Player> players;
    int currentPlayerIndex;
    int numPlayers;

    int rollDice() {
        return (rand() % 6) + 1;
    }

    void printStatus() {
        cout << "\n========================================\n";
        cout << "           CURRENT GAME STATE           \n";
        cout << "========================================\n";
        for (const auto& p : players) {
            cout << COLOR_NAMES[p.color] << " Player: ";
            if (p.hasFinished) {
                cout << "FINISHED!\n";
                continue;
            }
            cout << "\n";
            for (const auto& t : p.tokens) {
                cout << "  Token " << t.id << ": ";
                if (t.state == IN_BASE) cout << "In Base";
                else if (t.state == ON_TRACK) cout << "On Track (Pos: " << t.absolutePosition << ", Steps: " << t.stepsTaken << ")";
                else if (t.state == IN_HOME_STRETCH) cout << "In Home Stretch (" << t.stepsTaken - 50 << "/6 to finish)";
                else if (t.state == FINISHED) cout << "Finished!";
                cout << "\n";
            }
        }
        cout << "========================================\n\n";
    }

    void handleCapture(int targetPosition, PlayerColor attackingColor) {
        // Safe spots can be added here. For standard basic Ludo, starting squares are often safe.
        // We will keep it simple: any landing on an opponent token captures it.
        for (auto& p : players) {
            if (p.color == attackingColor) continue;
            for (auto& t : p.tokens) {
                if (t.state == ON_TRACK && t.absolutePosition == targetPosition) {
                    cout << ">>> " << COLOR_NAMES[attackingColor] << " CAPTURED " << COLOR_NAMES[p.color] << "'s Token " << t.id << "! <<<\n";
                    t.state = IN_BASE;
                    t.absolutePosition = -1;
                    t.stepsTaken = 0;
                }
            }
        }
    }

    bool moveToken(Player& p, int tokenIdx, int dice) {
        Token& t = p.tokens[tokenIdx];

        if (t.state == FINISHED) {
            cout << "Token already finished. Invalid move.\n";
            return false;
        }

        if (t.state == IN_BASE) {
            if (dice == 6) {
                t.state = ON_TRACK;
                t.absolutePosition = START_POSITIONS[p.color];
                t.stepsTaken = 1;
                cout << "Token " << t.id << " moved out of base to start position " << t.absolutePosition << ".\n";
                handleCapture(t.absolutePosition, p.color);
                return true;
            } else {
                cout << "You need a 6 to move out of base.\n";
                return false;
            }
        }

        if (t.state == ON_TRACK || t.state == IN_HOME_STRETCH) {
            if (t.stepsTaken + dice > 57) {
                cout << "Move exceeds home requirement. Token " << t.id << " needs exactly " << 57 - t.stepsTaken << " to finish.\n";
                return false;
            }

            t.stepsTaken += dice;

            if (t.stepsTaken <= 51) {
                // Still on the shared track
                t.absolutePosition = (t.absolutePosition + dice) % 52;
                cout << "Token " << t.id << " moved to track position " << t.absolutePosition << ".\n";
                handleCapture(t.absolutePosition, p.color);
            } else if (t.stepsTaken > 51 && t.stepsTaken < 57) {
                // Entered home stretch
                t.state = IN_HOME_STRETCH;
                t.absolutePosition = -1; // No longer on shared track
                cout << "Token " << t.id << " is in the home stretch!\n";
            } else if (t.stepsTaken == 57) {
                // Finished
                t.state = FINISHED;
                cout << "Token " << t.id << " HAS FINISHED!\n";
            }
            return true;
        }
        return false;
    }

    bool hasValidMoves(Player& p, int dice) {
        for (const auto& t : p.tokens) {
            if (t.state == IN_BASE && dice == 6) return true;
            if (t.state == ON_TRACK || t.state == IN_HOME_STRETCH) {
                if (t.stepsTaken + dice <= 57) return true;
            }
        }
        return false;
    }

public:
    LudoGame(int numP) : numPlayers(numP), currentPlayerIndex(0) {
        srand(static_cast<unsigned int>(time(0)));
        players.push_back(Player(RED));
        players.push_back(Player(GREEN));
        if (numPlayers > 2) players.push_back(Player(YELLOW));
        if (numPlayers > 3) players.push_back(Player(BLUE));
    }

    void start() {
        cout << "Welcome to Terminal Ludo Engine!\n";
        bool gameActive = true;

        while (gameActive) {
            Player& currentP = players[currentPlayerIndex];
            if (currentP.hasFinished) {
                currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
                continue;
            }

            printStatus();
            cout << "\n--- " << COLOR_NAMES[currentP.color] << "'s Turn ---\n";
            cout << "Press Enter to roll the dice...";
            cin.ignore();
            
            int dice = rollDice();
            cout << COLOR_NAMES[currentP.color] << " rolled a " << dice << "!\n";

            if (!hasValidMoves(currentP, dice)) {
                cout << "No valid moves available. Turn skipped.\n";
            } else {
                bool validMoveMade = false;
                while (!validMoveMade) {
                    cout << "Select a token to move (1-4): ";
                    int choice;
                    cin >> choice;
                    
                    // Clear buffer in case of bad input
                    if(cin.fail()) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid input. Please enter a number between 1 and 4.\n";
                        continue;
                    }

                    if (choice >= 1 && choice <= 4) {
                        validMoveMade = moveToken(currentP, choice - 1, dice);
                    } else {
                        cout << "Invalid token ID.\n";
                    }
                }
            }

            currentP.checkFinished();

            // Check win condition (if all but 1 player finished)
            int finishedCount = 0;
            for (const auto& p : players) {
                if (p.hasFinished) finishedCount++;
            }
            if (finishedCount >= numPlayers - 1) {
                cout << "\n========================================\n";
                cout << "              GAME OVER!                \n";
                cout << "========================================\n";
                gameActive = false;
                break;
            }

            // Grant extra turn on 6, otherwise move to next player
            if (dice != 6) {
                currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
            } else {
                cout << COLOR_NAMES[currentP.color] << " gets an extra turn for rolling a 6!\n";
                cin.ignore(10000, '\n'); // clear newline left by cin >> choice
            }
        }
    }
};

int main() {
    int players = 0;
    while (players < 2 || players > 4) {
        cout << "Enter number of players (2-4): ";
        cin >> players;
        if(cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }
    
    // Clear the input buffer before starting the game loop
    cin.ignore(10000, '\n');

    LudoGame game(players);
    game.start();

    return 0;
}