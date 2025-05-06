프로그램 실행 방법.
1. .exe와 같은 경로에 아래와 동일한 이름의 파일이 존재해야함.
    a. config.json
    b. body.json

    config.json은 프로그램의 설정 파일로써,
    서버 URL "ServerUrl"을 갖고 있음.
    {
        "ServerUrl" : "http://demo.lampad.co.kr:3688"
    }

    body.json은 Request의 body 역할을 수행하는 파일.
    대게, POST 요청 시에만 필요.
    {
        "type" : "bps",
        "bind" : 1
    }

2. .exe 실행 시 cmd에서 실행해야함.
    a. .exe를 실행할 때 인자 값이 필요함.

    실행 구문.
    ./..exe method endPoint

    method : HTTP 요청 메서드
    endPoint : HTTP 요청 구문

3. 프로그램을 실행하여 요청이 정상적으로 진행되었다면 응답은 response.json에 저장됨.  