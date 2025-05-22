프로그램 정보

LAMPAD Rest API 연습하는 중.

5월 23일 까지 과제 완료.

주제 : 쓰레드

목표 : 특정의 서버에 저장되어 있는 모든 피드의 최근 5분 BPS, PPS Unicast 데이터에 대해 최대 값과 최저 값, 그에 따른 시간을 구하기. 


프로그램 실행 방법.
1. .exe와 같은 경로에 아래와 동일한 이름의 파일이 존재해야함.
    a. config.json

    config.json은 프로그램의 설정 파일로써,
    서버 URL "ServerUrl"과 Feed 정보를 저장할 파일의 경로 "FeedsInformationPath"를 갖고 있음.
    {
        "ServerUrl" : "http://demo.lampad.co.kr:3688"
    }
