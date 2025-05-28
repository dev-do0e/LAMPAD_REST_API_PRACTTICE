프로그램 정보

LAMPAD Rest API 연습하는 중.

5월 30일 까지 과제 완료.

주제 : 쓰레드

목표 :  프로그램 내 쓰레드는 쓰레드 전용 함수를 호출 
        특정의 서버에 저장되어 있는 모든 피드의 최근 5분 BPS, PPS Unicast 데이터에 대해 최대 값과 최저 값, 그에 따른 시간을 구하기. 

프로그램 실행 방법.
1. .exe와 같은 경로에 아래와 동일한 이름의 파일이 존재해야함.
    a. config.json

    config.json은 프로그램의 설정 파일로써,
    서버 URL "ServerUrl"과 Feed 정보를 저장할 파일의 경로 "FeedsInformationPath"를 갖고 있음.
    {
        "ServerUrl" : "http://demo.lampad.co.kr:3688"
    }

5월 30일 과제 시에 의문점.
1.  지금 코드 구현 방식 기준으론,
    main에서 bps 분석, pps 분석이 끝나면 그때부터 std::this_thread::sleep_for(std::chrono::seconds(10)); 를 진행하여, 테스트 텀이 "bps, pps 분석 시간 + 10초"가 되는데
    이게 아닌
    테스트 텀이 무조건 std::this_thread::sleep_for(std::chrono::seconds(10));에 맞게 진행되려면 어떤 기법을 사용해야하는지

2.  테스트 텀이 무조건 std::this_thread::sleep_for(std::chrono::seconds(10));에 맞게 진행되게끔 했을 때
    만약 bps 분석과 pps 분석이 std::this_thread::sleep_for(std::chrono::seconds(10)); 보다 오래 걸렸을 땐 어떻게 처리해야하는지
