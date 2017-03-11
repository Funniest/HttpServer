#Http Server
###Http Server
Http server는 웹 클라이언트가 웹 페이지를 요청하면 Http Server는 요청에 따라HTML문서를 전달합니다.

또는 클라이언트로 부터 파일 등을 전달 받습니다.

Http server를 간단히 그림으로 표현해보았습니다.

아래 그림처럼 Client가 Server에게 요청하면, 서버는 Request로 요청한 문서를 보내 줍니다.

그림[1]

###Http Protocol
위 그림에서 간략하게 설명되었지만, 조금 더 자세하게 알아봅시다.

Http패킷은 위의 그림처럼 요청과 요청에 따른 응답이 있습니다.

이에 대한 패킷 구조는 Http header, Http content를포함합니다.

Http header의 특징은 \r\n으로 줄이 구분된단는 것이 특징입니다.

위 조그마한 글자를 보면, 요청을 하는 패킷이 나와있는데, 대부분 아래와 같은 느낌입니다.

```
GET / HTTP/1.1
Host: www.naver.com
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4
Cookie: test
Connection: close 
```
요청을 하는 패킷은 아래 처럼 생겼습니다.

```
HTTP/1.1 200 OK
Content-Length: 3612
Content-Type: text/xml; charset="utf-8"
Server: Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0 Microsoft-HTTPAPI/2.0
Date: Thu, 09 Mar 2017 10:30:02 GMT
Connection: close

(HTML Content...)
```

이제 위 패킷을 조금 분석해서 HTTP Server를 제작하여 보겠습니다.

###소스코드

####Server
```

```
