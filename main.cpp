#include "heist.h"

int main() {
    srand((unsigned)time(0));                       // random culprit every game

    LeaderBoard board("leader/scoreBoard.txt");
    GameEngine* engine = GameEngine::getInstance();
    engine->setLeaderBoard(&board);
    engine->showTitle();

    // game exceptions are caught then game continues
    while (engine->getState() != STATE_EXIT) {
        try {
            engine->runStep();
        }
        catch (GameException& e) {
            engine->handleException(e);
        }
    }

    GameEngine::destroyInstance();
    return 0;
}
