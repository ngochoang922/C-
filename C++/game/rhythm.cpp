#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Tạo cửa sổ game
    sf::RenderWindow window(sf::VideoMode(800, 600), "Simple Rhythm Game");

    // Tải file nhạc
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("music.wav")) {
        std::cerr << "Failed to load music file." << std::endl;
        return 1;
    }

    // Tạo đối tượng nhạc
    sf::Sound music;
    music.setBuffer(buffer);
    music.setLoop(true); // Lặp lại nhạc

    // Phát nhạc
    music.play();

    // Vòng lặp chính của trò chơi
    while (window.isOpen()) {
        // Xử lý sự kiện
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            // Xử lý sự kiện nhấn phím
            if (event.type == sf::Event::KeyPressed) {
                // Xử lý nhấn phím theo ý muốn của bạn
                // Ví dụ: kiểm tra xem phím được nhấn có phải là phím mũi tên không và thực hiện hành động tương ứng
            }
        }

        // Vẽ mọi thứ lên cửa sổ
        window.clear();
        // Vẽ các đối tượng game (như các nút nhấn) ở đây
        window.display();
    }

    return 0;
}
