#ifndef SIM_H
#define SIM_H

class Hero;
class Board;
class EventQueue;

// rpg ver0051: 새로 시뮬레이션에서 만들어진 부분:
//    - 아래의 class Sim은 모든 소스코드에서 바로 access가 필요한 중요한 variable들이나
//      object들을 public으로 만들어 편리하게 이용하게 해준다.
//    - 간단히 말하면, 이렇게 하므로써 일단 편리해진 부분은, 예를 들면, 모든 class에
//      "Board *board"를 만들고 새 object를 만든 직후, setBoard()로 일일히 board의
//      주소를 넣어 줄 필요가 없다.
//      그래서, class Unit등을 보면 이전 버젼과 비교해서, 그 부분이 삭제된 것을 알 수 있다.
//      대신 모든 board를 사용하는 부분에서 "board->" 대신 "sim->board->"를 사용하고 있고,
//      ".cpp" 맨 위에 "extern Sim *sim;"을 붙인 것을 볼 수 있다.
//    - 더 간단히 말하면, "편할라고"

// design pattern: singleton
class Sim {
public:
    int time; // 중요해? 시뮬레이션에서 전체 시간을 매우 중요하다. 특히 event들의 scheduling에..
              //       또한 나중에 Statistics로 snapshot을 남길 때..
    
    Hero *hero;   // 중요해? main()등에서..
    Board *board; // 중요해? main()뿐만 아니라, 거의 모든 class들에서 필요하다.
                  //       예를 들면, unit들이 움직일 때나, item을 pick up할 때..
                  //       portal이 unit을 random position으로 이동시킬 때..
                  //       locked chest가 자신이 가진 item을 땅바닥에 놓고 자신을 지울 때..

    EventQueue *eventQueue; // 중요해? event들을 priority queue방식으로 관리한다.
                            //       event scheduling이 필요한 곳에서 새로 event를
                            //       만들어 넣어야 하므로 필요하다.
                            //       또한, main()에서 매번 main game loop을 돌 때마다
                            //       sim->time이 1씩 증가되는데, 그때마다 loop을 이용하여
                            //       sim->eventQueue->peek()으로 다음에 나올 event가
                            //       현재 sim->time보다 작다면, 실행될 event이므로..
                            //       그런 event들을 모두 꺼내어(dequeue()) 처리한 후
                            //       main game loop이 진행된다.

    Sim();
    virtual ~Sim();
    
    // 이렇게 아래에 컨스트럭터에서 할 일을 분리한 이유는, main()에서 sim이라는 global variable에
    // 미리 object를 만들고 주소를 줘야만, board등이 initialize를 할 때, tile이나 item등의
    // 생성시 global variable인 sim->board의 주소를 모든 item, prop, 그리고 unit등이
    // 가져갈 수 있다.
    void init(int rowSize, int colSize,
              int numTrees, int numPortals,
              int numPotions, int numKeys,
              int maxNumArmors, int numWeapons,
              int numMons, int maxNumBosses, int maxNumMerchants,
              int heroRow, int heroCol);
};

#endif
