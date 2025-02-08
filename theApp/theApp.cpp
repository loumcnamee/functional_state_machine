#include <string>
#include <iostream>
#include <variant>
#include <optional>
#include <cstdint>

using namespace std;
/* --------------------------------- Events ------------------------------------------ */
struct EventStart {  };
struct EventTooHot { };
struct EventTooCold { };
struct EventStop { };


using Event = variant<EventStart, EventTooHot, EventTooCold, EventStop>;
/* ------------------------------------------------------------------------------------- */


/* --------------------------------- States ------------------------------------------ */
struct IState {
    virtual ~IState() = default;
    float temp_ = 0.0;
    float power_ = 0.0;

};
struct Off : public IState { };
struct Idle : public IState { };
struct Heating : public IState { };
struct Cooling : public IState{ };

using State = variant<Off, Idle, Heating, Cooling>;
/* ------------------------------------------------------------------------------------- */


/* ------------------------------- Transitions ---------------------------------------- */
struct Transitions {
    optional<State> operator()(Off &, const EventStart &e) {
        cout << "Off -> Idle" << endl;
        return Idle{};
    }

    optional<State> operator()(Idle &, const EventTooHot &) {
        cout << "Idle -> Cooling" << endl;
        return Cooling{};
    }

    optional<State> operator()(Idle &, const EventTooCold &) {
        cout << "Idle -> Heating" << endl;
        return Heating{};
    }
    optional<State> operator()(Heating &s, const EventTooCold &) {
        cout << "Heating -> Heating" << endl;
        return Heating{};
    }

    optional<State> operator()(Cooling &s, const EventTooHot &) {
        cout << "Cooling -> Cooling" << endl;
        return Cooling{};
    }

    optional<State> operator()(Cooling &, const EventStop &) {
        cout << "Cooling -> Idle" << endl;
        return Idle{};
    }

    optional<State> operator()(Heating &, const EventStop &) {
        cout << "Heating -> Idle" << endl;
        return Idle{};
    }
    
    optional<State> operator()(Idle &, const EventStop &) {
        cout << "Idle -> Idle" << endl;
        return Idle{};
    }

    template <typename State_t, typename Event_t>
    optional<State> operator()(State_t &, const Event_t &) const {
        cout << "Unkown" << endl;
        return Idle{};
    }
};

/* ------------------------------------------------------------------------------------- */
template <typename StateVariant, typename EventVariant, typename Transitions>
class Controller {
    StateVariant m_curr_state = Off{};

    void dispatch(const EventVariant &Event)
    {
        optional<StateVariant> new_state = visit(Transitions{}, m_curr_state, Event);
        if (new_state)
            m_curr_state = *move(new_state);
    }

    public: 
        template <typename... Events>
        void process(Events... e) { (dispatch(e), ...); }
};

int main() {
    float temperture = 0.0;
    float setting = 10.0;
    Controller<State, Event, Transitions> ctrl;
    
    ctrl.process(EventStart{});
    
    
    for (int i = 0; i < 100; i++) {
    
        if (temperture == setting ) {
            ctrl.process(EventStop{});
        }
        if (temperture > setting) {
            ctrl.process(EventTooHot{});
            temperture += 1.0f;
        }
        if (temperture < setting) {
            ctrl.process(EventTooCold{});
            temperture += 1.0f;
        }

        std::cout << "Temp: " << temperture << std::endl;
    }

                      //      EventStarted{},
                      //      EventTimeout{},
                      //      EventConnected{},
                      //      EventDisconnect{});
    return EXIT_SUCCESS;
}
/*
Idle -> Connect
Connecting -> Timeout
Connecting -> Connected
Connected -> Disconnect
*/
