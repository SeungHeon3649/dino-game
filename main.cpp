#include "seungheon.h"                                                        //"seungheon"헤더파일 include   
int main()
{

    String model = "pose_iter_160000.caffemodel";                             // Net 객체를 생성하기 위한 모델 파일
    String config = "pose_deploy_linevec_faster_4_stages.prototxt";           // Net 객체를 생성하기 위한 구성 파일

    VideoCapture cap(0);                                                      // 0번째 카메라를 사용하기 위한 생성자
    if (!cap.isOpened()) {                                                    // 카메라가 켜져있지 않으면
        cerr << "Camera open failed!" << endl;                                // 카메라 안 켜져 있다 출력
        return -1;                                                            // 프로그램 종료
    }

    cap.set(CAP_PROP_FRAME_HEIGHT, 720);                                      // 카메라 해상도 조절
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);                                      // 카메라 해상도 조절

    Mat src(720, 1024, CV_8UC3, Scalar(237, 246, 255));                       //1024*720 배경 생성

    //캐릭터1(공룡) 이미지 
    Mat dino = imread("dino1.png", IMREAD_COLOR);                             // 첫번째 공룡 이미지를 dino에 저장
    if (dino.empty()) {                                                       // 사진 없으면 
        cerr << "Image load failed!" << endl;                                 // Image load failed! 출력
        return -1;                                                            // 프로그램 종료
    }
    resize(dino, dino, Size(150, 150));                                       // dino 이미지를 150 * 150 사이즈로 변경
    //Mat dino2 = dino.clone();	//dino의 이미지를 dino2에 깊은 복사

    Mat dinor = imread("dino2.png", IMREAD_COLOR);
    if (dinor.empty()) {
        cerr << "Image load failed!" << endl;
        return -1;
    }
    resize(dinor, dinor, Size(150, 150));
    //Mat dinor2 = dinor.clone();	//dino의 이미지를 dino2에 깊은 복사

    Net net = readNet(model, config);
    net.setPreferableBackend(DNN_BACKEND_CUDA);
    net.setPreferableTarget(DNN_TARGET_CUDA);

    Mat frame;
    Point triangle_point[triangle_num];                                                         //삼각형의 무게중심을 배열로 선언 후 모두 -1,-1로 초기화
    for (int i = 0; i < triangle_num; i++) {
        triangle_point[i] = Point(-1, -1);
    }
    Point rect_point[rect_num];
    for (int i = 0; i < rect_num; i++) {
        rect_point[i] = Point(-1, -1);
    }

    //int start_point_num = 0;
    //double score = 0;
    int cnt = 0;                                                                                //와일문 한번 돌 때 마다 1씩 증가하는 변수 선언
    int speed = 14;                                                                             //장애물의 속도를 올려 줄 변수 선언
    int score = 0;
    int maxscore = 0;
    int interval_tri = 50;
    int interval_rect = 77;
    int level = 0;
    float scale = 1 / 255.f;
    Point neckpt;
    int dinox = 20;
    int dinoy = 360;
    bool start = false;                                                                         //게임 시작을 판단 할 변수 선언
    bool menu = true;
    bool jump = false;
    bool seat = false;
    bool jump_pandan = false;
    bool seat_pandan = false;
    bool gameover = false;
    int dead_cnt = 0;

    while (true)
    {
        cap >> frame;
        if (frame.empty()) {
            cerr << "Camera open failed!" << endl;
            break;
        }
        resize(frame, frame, Size(1280, 720));
        flip(frame, frame, 1);
        Mat inputBlob = blobFromImage(frame, scale, Size(368, 368), Scalar(0, 0, 0));
        net.setInput(inputBlob);
        Mat result = net.forward();

        int H = result.size[2];
        int W = result.size[3];

        float SX = float(frame.cols) / W;
        float SY = float(frame.rows) / H;

        int nparts = result.size[1];

        //몸의 좌표 검출
        Point body[19], maxp;
        double maxloc;
        for (int i = 0; i < 19; i++) {
            body[i] = Point(-50, -50);
        }
        for (int i = 0; i < 19; i++) {
            Mat heatMap(H, W, CV_32F, result.ptr(0, i));
            minMaxLoc(heatMap, 0, &maxloc, 0, &maxp);
            if (maxloc > 0.3) {
                body[i] = maxp;
                body[i].x *= SX;
                body[i].y *= SY;
                circle(frame, body[i], 10, Scalar(255, 0, 0), -1);
            }
        }
        circle(frame, body[1], 10, Scalar(0, 0, 255), -1);
        circle(frame, body[4], 10, Scalar(0, 0, 255), -1);
        circle(frame, body[7], 10, Scalar(0, 0, 255), -1);
        src = Mat(720, 1024, CV_8UC3, Scalar(237, 246, 255));                                   //배경 그려주기
        line(src, Point(0, 480), Point(1024, 480), Scalar(0, 0, 0));                            //src 배경에 선 그려주기

        //menu버튼
        if (menu) {
            //버튼 생성
            rectangle(frame, Rect(280, 80, 210, 92), Scalar(0, 255, 0), -1);
            cv::putText(frame, "Start", Point(300, 150), 1, 4, Scalar(0, 0, 0), 3);
            rectangle(frame, Rect(788, 80, 210, 92), Scalar(0, 0, 255), -1);
            cv::putText(frame, "EXIT", Point(830, 150), 1, 4, Scalar(0, 0, 0), 3);

            if (body[4].inside(Rect(280, 80, 210, 92)) || body[7].inside(Rect(280, 80, 210, 92))) {	//손목의 좌표가 시작 버튼 안에 있을 경우
                PlaySound(TEXT("효과음.wav"), NULL, SND_FILENAME | SND_ASYNC);		//버튼 터치 시 효과음 발생
                start = true;		//게임 시작 카운트 기능을 활성화
                menu = false;			//메뉴 기능을 비활성화
            }
            else if (body[4].inside(Rect(788, 80, 210, 92)) || body[7].inside(Rect(788, 80, 210, 92))) {	//손목의 좌표가 종료 버튼 안에 있을 경우
                PlaySound(TEXT("권총한발.wav"), NULL, SND_FILENAME | SND_ASYNC);		//버튼 터치 시 효과음 발생
                frame = Scalar(0, 0, 0);
                cv::putText(frame, "See you!", Point(295, 340), 1, 9, Scalar(50, 255, 255), 7);	//영상에 "Bye bye~!!" 문구 출력
                imshow("frame", frame);	//모니터에 frame 영상 출력
                waitKey(1500);	//1.5초 동안 키 입력 대기
                return 0;		//프로그램 종료
            }
        }

        //게임 시작
        if (start) {                                                                            //만약 start가 true이면
            //목의 좌표 저장
            if (cnt == 0) neckpt = body[1];

            //점프 판단
            if (neckpt.y - body[1].y > 70) {
                PlaySound(TEXT("점프19.wav"), NULL, SND_FILENAME | SND_ASYNC);
                jump = true;
            }
            //점프 좌표 이동
            if (jump) {
                if (dinoy <= 260) jump_pandan = true;
                if (jump_pandan) {
                    dinoy = dinoy + 10;
                    if (dinoy >= 360) {
                        dinoy = 360;
                        jump = false;
                        jump_pandan = false;
                    }
                }
                else if (jump_pandan == false) dinoy = dinoy - 10;
            }

            //앉기 판단
            if (body[1].y - neckpt.y > 180) {
                seat = true;
            }
            //앉기 좌표 이동
            if (seat) {
                if (dinoy >= 430) seat_pandan = true;
                if (seat_pandan) {
                    dinoy = dinoy - 10;
                    if (dinoy <= 360) {
                        dinoy = 360;
                        seat = false;
                        seat_pandan = false;
                    }
                }
                else if (jump_pandan == false) dinoy = dinoy + 10;
            }

            //공룡 생성
            if (cnt % 6 <= 2) {
                bitwise_and(src(Rect(dinox, dinoy, 150, 150)), dino, src(Rect(dinox, dinoy, 150, 150)));
            }
            else {
                bitwise_and(src(Rect(dinox, dinoy, 150, 150)), dinor, src(Rect(dinox, dinoy, 150, 150)));
            }

            //장애물과 부딪히는지 판단
            Point dino_tri_bomb(dinox + 75, dinoy + 100);
            Point dino_rect_bomb(dinox + 75, dinoy + 20);
            //circle(src, dino_rect_bomb, 7, Scalar(255, 0, 0), -1);
            //circle(src, dino_tri_bomb, 7, Scalar(255, 0, 0), -1);
            for (int i = 0; i < 10; i++) {
                if (abs(dino_tri_bomb.x - triangle_point[i].x) < 20 && abs(dino_tri_bomb.y - triangle_point[i].y) < 20) {
                    start = false;
                    gameover = true;
                    PlaySound(TEXT("총소리2.wav"), NULL, SND_FILENAME | SND_ASYNC);
                    continue;
                }
                if (abs(dino_rect_bomb.x - rect_point[i].x) < 20 && dino_rect_bomb.y - rect_point[i].y < 20) {
                    start = false;
                    gameover = true;
                    PlaySound(TEXT("총소리2.wav"), NULL, SND_FILENAME | SND_ASYNC);
                    continue;
                }
            }

            //삼각형 무게중심 좌표 대입
            if (cnt % interval_tri == 0) {                                                      //while문이 1000번 돌 때 마다 if문(간격조절)
                for (int i = 0; i < triangle_num; i++) {                                        //반복문 i = 0부터 i < 10까지 (triangle_num = 10)
                    if (triangle_point[i] == Point(-1, -1)) {                                   //만약 triangle_point[i]가 화면속에 없고 빈 공간에 있으면
                        triangle_point[i] = Point(1024, 470);                                   //triangle_point(무게중심)의 좌표에 1310,480 대입
                        break;                                                                  //1개만 대입하고 for문 탈출
                    }
                }
            }

            //사각형 좌상 좌표 대입
            if (cnt != 0 && cnt % interval_tri > 15 && cnt % interval_rect == 0)                //삼각형 장애물과 겹치지 않는 if문
            {
                for (int i = 0; i < rect_num; i++) {                                            //반복문 10까지
                    if (rect_point[i] == Point(-1, -1)) {                                       //만약 삼각형의 좌표가 (-1,-1)이면
                        rect_point[i] = Point(1054, 370);                                       //사각형의 좌상 좌표에 1310,370 대입             
                        break;                                                                  //1개만 대입하고 for문 탈출
                    }
                }
            }

            //속도 조절
            if (cnt % 1 == 0) {                                                                 //while 문이 5번 돌 때마다(속도조절)
                for (int i = 0; i < 10; i++) {                                                  //반복문
                    if (triangle_point[i] != Point(-1, -1)) {                                   //삼각형의 무게중심이 -1, -1이 아니면(즉, src에 있으면)
                        triangle_point[i].x -= speed;                                           //삼각형의 좌표를 speed만큼 좌측으로 이동
                    }
                    if (rect_point[i] != Point(-1, -1)) {                                       //사각형의 좌상 좌표가 (-1, -1)이 아니면(즉, src에 있으면)
                        rect_point[i].x -= speed;                                               //사각형의 x의 좌표를 speed만큼 좌측으로 이동
                    }
                }
            }

            //장애물 좌표 초기화
            for (int i = 0; i < 10; i++) {                                                      //반복문
                if (triangle_point[i].x <= -30) {                                               //만약 삼각형의 무게중심의 x좌표가 -30보다 작거나 같으면
                    triangle_point[i] = Point(-1, -1);                                          //무게중심의 좌표를 -1,-1로 초기화(i번째 삼각형을 다시 사용할 수 있음)
                }
                if (rect_point[i].x <= -30) {                                                   //사각형의 좌상 좌표가 -30보다 작거나 같으면 
                    rect_point[i] = Point(-1, -1);
                }
            }

            //장애물 생성
            for (int i = 0; i < 10; i++) {                                                      //반복문
                if (triangle_point[i] != Point(-1, -1)) {                                       //만약 삼각형의 무게중심이 -1, -1이 아니면(즉, src에 있으면)
                    triangle(src, triangle_point[i]);                                           //triangle 함수 호출(삼각형 그려주기)
                    triangle(src, Point(triangle_point[i].x + 30, triangle_point[i].y));        //triangle 함수 호출(삼각형 그려주기) -> 위에 삼각형보다 조금 더 오른쪽에 그려주기
                }
                if (rect_point[i] != Point(-1, -1)) {                                           //사각형의 좌상의 좌표가 (-1, -1)이 아니면
                    rect(src, rect_point[i]);                                                   //rect 함수 호출(사각형 그려주기)
                }
            }

            //속도,점수,레벨 증가
            if (cnt != 0 && cnt % 100 == 0)                                                    //cnt 1000으로 나눈 나머지가 0일때
            {
                speed = speed + 2;                                                                        //speed 1증가
                level++;
                interval_tri -= 2;                                                             //간격 70감소
                interval_rect -= 4;
                if (interval_tri <= 30) interval_tri = 30;
                if (interval_rect <= 43) interval_tri = 43;
            }

            //점수 출력
            cnt++;                                                                              //cnt = cnt + 1;
            score = cnt;
            if (score % 100 == 0) {
                PlaySound(TEXT("마법소리.wav"), NULL, SND_FILENAME | SND_ASYNC);
            }
            if (score > maxscore) maxscore = score;
            String score_text = format("HI_Score : %d Score : %d", maxscore, score);
            String level_text = format("LV : %d", level);
            putText(src, score_text, Point(530, 50), 1, 2, Scalar(0, 0, 0), 3);
            putText(src, level_text, Point(25, 50), 1, 2, Scalar(0, 0, 0), 3);
        }
        if (gameover) {
            if (dead_cnt < 7) putText(src, "Dead", Point(350, 260), 1, 9, Scalar(0, 0, 0), 13);
            else {
                String strScore = format("%d", score);
                putText(src, strScore, Point(380, 260), 1, 8, Scalar(0, 0, 0), 13);
            }
            if (dead_cnt == 14) {
                cnt = 0;
                score = 0;
                level = 0;
                dead_cnt = 0;
                menu = true;
                gameover = false;
                for (int i = 0; i < 10; i++) {                                                  //장애물 좌표 초기화
                    triangle_point[i] = Point(-1, -1);                                          //무게중심의 좌표를 -1,-1로 초기화(i번째 삼각형을 다시 사용할 수 있음)
                    rect_point[i] = Point(-1, -1);
                }
            }
            dead_cnt++;
        }
        cout << dinoy << endl;
        //cout << cnt << endl;
        imshow("frame", frame);
        imshow("Game", src);
        int key = waitKey(2);
        if (key == 27) break;
    }
    return 0;
}
