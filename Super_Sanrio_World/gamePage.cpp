#include<SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "cloud.h"
#include "endPage.h"
#include "gamePage.h"
#include "startPage.h"
#include "character.h"
#include "position.h"
#include "obstacle.h"
#include "scoreText.h"

using namespace sf;
using namespace std;

#define WIDTH 840
#define HEIGHT 480
#define KITTY_Y_BOTTOM 310

enum CurrentP {
    StartP,
    GameP,
    EndP
};

void gamePage::run() {
    RenderWindow window(VideoMode(840, 480), "Super Sanrio World");
    window.setFramerateLimit(60);

    //페이지 불러오기
    startPage startP("resources/startpage.png");
    endPage endP("resources/endpage.png");
    Character character("resources/character1.png", "resources/character2.png");

     // 최고 기록
    int maxScore = 0;

    const int changeCharacter = 5;
    const int index = 0;
    float frame = 0.0f;
    float frameSpeed = 0.6f;

    float gravity = 15;
    bool isJumping = false;
    bool isBottom = true;

    CurrentP currentP = StartP;

    srand(time(nullptr)); // 랜덤 시드 초기화

    //배경
    Texture map;
    map.loadFromFile("resources/sanrio_map.png");
    Sprite mapSprite(map);
    mapSprite.setTextureRect(IntRect(0, 0, WIDTH, HEIGHT));

    //캐릭터 위치 - 효진
    Position kittyPos;
    kittyPos.x = 70;
    kittyPos.y = KITTY_Y_BOTTOM;

    // 장애물
    Obstacle obs;

    // 구름 - 지수
    Cloud cloud;

    // 점수 측정을 위한 시계
    Clock clock;
    float seconds = 0.0f;
    int score = 0;

    Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\H2GSRB.ttf"))
    {
        printf("폰트 불러오기 실패\n");
    }

    //텍스트(점수) 불러오기
    ScoreText maxscoreText(font, 53, 740, 5,Color::Magenta);
    ScoreText scoreText(font,33,10,5,Color::White);
    ScoreText scoreResultText(font, 100, 340, 165, Color::Magenta);

    // 게임 재시작 버튼 - 지수
    Texture restartBtn;
    restartBtn.loadFromFile("resources/restart_btn.png");
    Sprite reStartBtn_Sprite(restartBtn);
    reStartBtn_Sprite.setPosition(550, 350);

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {

            if (e.type == Event::Closed) {
                window.close();
                cout << "Window is closed." << endl;
            }

            // 키가 눌렸을 때
            if (e.type == Event::KeyPressed) {

                // 시작화면에서 Enter Key가 눌렸을 때 - 지수
                if (e.key.code == Keyboard::Enter && currentP == StartP) {
                    currentP = GameP; // Enter 키를 누르면 게임 화면으로 전환
                    cout << "Switched to GameP." << endl;
                }

                // 게임화면에서 Up Key가 눌렸을 때 - 효진
                if (e.key.code == Keyboard::Up && currentP == GameP) {
                    if (isBottom == true && isJumping == false) {
                        isJumping = true;
                        isBottom = false;
                    }
                }
            }

            // 재시작 버튼 클릭 구현 - 지수
            if (e.type == Event::MouseButtonPressed) { // 마우스가 눌리면
                if (e.mouseButton.button == Mouse::Left) { // 마우스의 왼쪽 버튼이 눌리면
                    Vector2i mousePos = Mouse::getPosition(window); // 프레임 안에서 마우스로 누른 곳

                    // 재시작 버튼의 경계에 마우스로 누른 곳이 포함되었다면
                    if (reStartBtn_Sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        currentP = StartP; // Start 페이지로 변경
                        score = 0;      // 점수 초기화
                        gravity = 15;   // 점프할 때 내려오는 속도 초기화
                    }
                }
            }
        }

        // 화면 전환 구현 - 지수
        if (currentP == StartP) {
            // 시작화면에서 게임화면으로 넘어가게
            startP.draw(window);
        }
        if (currentP == GameP) {
            //점수 - 효진
            seconds += clock.restart().asSeconds();
            //0.3초에 1점씩 점수증가
            if (seconds >= 0.3f) {
                score++;
                seconds = 0.0f;

                // 시간 흐름에 따라 속도, 점프할 때 내려오는 속도 증가 - 지수
                if (seconds >= 90) {
                    for (int i = 0; i < obs.getObstacleCnt(); i++) {
                        obs.sumSpeed(0.1);
                        gravity++;
                    }
                }
                if (seconds >= 200) {
                    for (int i = 0; i < obs.getObstacleCnt(); i++) {
                        obs.sumSpeed(0.2);
                        gravity++;
                    }
                }
            }

            // 현재 점수
            scoreText.setString(score);

            // 최고 기록 기능 - 지수
            maxscoreText.setString(maxScore);

            // gampPage draws
            window.clear();
            window.draw(mapSprite);
            character.draw(window);
            scoreText.draw(window);
            maxscoreText.draw(window);
            cloud.drawCloud(window);
            obs.drawObstacle(window);

            // 캐릭터 점프 설정 - 효진
            if (isJumping == true)
            {
                kittyPos.y -= gravity;
                character.setPosition(kittyPos.x, kittyPos.y);
            }
            else {
                kittyPos.y += gravity;
                character.setPosition(kittyPos.x, kittyPos.y);
            }

            //점프하고 있지 않을 시 Y값에 있도록 - 효진
            if (kittyPos.y >= KITTY_Y_BOTTOM) {
                kittyPos.y = KITTY_Y_BOTTOM;
                isBottom = true;
            }

            //점프 높이 제한(하늘위로 계속 올라가지 않도록) -효진
            if (kittyPos.y <= KITTY_Y_BOTTOM - 210)
            {
                isJumping = false;
                score += 3;
            }
            character.setPosition(kittyPos.x, kittyPos.y);

            //장애물과 캐릭터 충돌 -효진
           FloatRect characterBounds = character.getKittySprite(index).getGlobalBounds();

           for (int i = 0; i < obs.getObstacleCnt(); i++) {
               FloatRect obstacleBounds = obs.getSprites(i).getGlobalBounds();

               if (characterBounds.intersects(obstacleBounds)==true) {
                   
                   for (int j = 0; j < obs.getObstacleCnt(); j++)
                       obs.setObstaclePosX(j, WIDTH);
                   currentP = EndP;
                   break;
               }
           }
            //장애물 움직임 
            obs.moveObatacle();

            // 구름 움직임
            cloud.moveCloud();
        }
        if (currentP == EndP) {
            scoreResultText.setString(score);

            if (score > maxScore) maxScore = score;

            obs.setObstacleSpeed(7);

            window.clear();
            endP.draw(window);
            scoreResultText.draw(window);
            window.draw(reStartBtn_Sprite);

        } // 화면 전환 기능 - 지수

        //캐릭터 다리움직임 - 효진
        character.move(frame, frameSpeed, kittyPos.y, changeCharacter);
        HWND hWndConsole = GetConsoleWindow();
        ShowWindow(hWndConsole, SW_HIDE);
        window.display();
    };
}