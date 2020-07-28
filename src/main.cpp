#include "Rocha.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>

class Keyboard {
  public:
    void update(sf::Event e)
    {
        switch (e.type) {
            case sf::Event::KeyPressed:
                m_keys[e.key.code] = true;
                break;

            case sf::Event::KeyReleased:
                m_keys[e.key.code] = false;

            default:
                break;
        }
    }
    bool isKeyDown(sf::Keyboard::Key key) const
    {
        return m_keys[key];
    }

  private:
    std::array<bool, sf::Keyboard::KeyCount> m_keys{false};
};

int main()
{
    Rocha::Machine rocha;
    rocha.loadScript("test.roc");
    rocha.runFunction("test");
}
/*
int main()
{
    sf::RenderWindow window({1280, 720}, "SFML");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    Keyboard keyboard;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            keyboard.update(e);
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                default:
                    break;
            }
        }

        window.clear();

        window.display();
    }
}
*/