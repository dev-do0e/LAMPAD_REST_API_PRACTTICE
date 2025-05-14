프로그램 정보

LAMPAD에 저장되어 있는 모든 피드의 최근 5분 BPS를 파일로 저장하는 프로그램

프로그램 실행 방법.
1. .exe와 같은 경로에 아래와 동일한 이름의 파일이 존재해야함.
    a. config.json

    config.json은 프로그램의 설정 파일로써,
    서버 URL "ServerUrl"과 Feed 정보를 저장할 파일의 경로 "FeedsInformationPath"를 갖고 있음.
    {
        "ServerUrl" : "http://demo.lampad.co.kr:3688",
        "FeedsInformationPath" : "./feedsInformation.json"
    }

2. 프로그램을 실행하여 요청이 정상적으로 진행되었다면 응답은 bpsResult/ 경로에 저장됨.  