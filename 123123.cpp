#include <iostream>
#include <variant>

namespace state {
    struct _PlayerAlive {
        void PrintState() const { std::cout << "State: PlayerAlive" << std::endl; }
        using Next = _1;
    };

    struct _PlayerDead {
        void PrintState() const { std::cout << "State: PlayerDead" << std::endl; }
        using Next = _1;
    };

    struct _GameOver {
        void PrintState() const { std::cout << "State: GameOver" << std::endl; }
        using Next = _1;
    };

    struct _3 {
        void PrintState() const { std::cout << "State: 3" << std::endl; }
        using Next = _1;
    };

    struct _4 {
        void PrintState() const { std::cout << "State: 4" << std::endl; }
        using Next = _1;
    };

}

using State = std::variant<state::_PlayerAlive, state::_PlayerDead, state::_GameOver, state::_3, state::_4>;

auto StateReporter = [](const auto& d){ d.PrintState(); };
auto nextProcessor = [](const auto& s) { return typename std::decay_t<decltype(s)>::Next {}; };

class StateMachine {
private:
    State state_;
public:
    void start();
    void reportCurrentState();
    void next();
};

void StateMachine::start() {
    state_ = state::_PlayerAlive {};
}

void StateMachine::reportCurrentState() {
    std::visit(StateReporter, state_);
}

void StateMachine::next() {
    state_ = std::visit(nextProcessor, state_);
}

int main() {
    StateMachine program;
    program.start();
    try {
        program.reportCurrentState();
        program.next();
        program.reportCurrentState();
    }
    catch(std::exception& ex){
        std::cout << "Exception: " << ex.what() << std::endl;
    }
}
