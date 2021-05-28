# 2020_Embeded-OS
본 프로젝트는 한국산업기술대학교 전자공학부 임베디드 시스템 전공 학부과정 중 수강한 Embeded-OS 프로젝트를 담고 있습니다.
TCP/IP 소켓 프로그래밍을 통한 서버와 클라이언트 소켓 할당, 다중 클라이언트 사이의 그룹 채팅과 P2P 파일 전송 등의 소스 코드와 함께 전체 프로세스 설명이 포함되어 있습니다.
프로젝트에 대한 질문은 언제나 환영입니다!  
§ E-mail : <dayonits2@gmail.com>  
  
  
## Project #1 : 다중 클라이언트 로그인
------------
![Project_#1](https://user-images.githubusercontent.com/68980204/119942646-0cddde80-bfcd-11eb-9a4d-35b57f657892.PNG)
  
	+ Project #1 조직도
  

+ 조건
	+ 클라이언트는 서버의 IP주소와 Port 번호를 가지고 소켓 인터페이스를 이용하여 서버에 접속한다.
	+ 서버가 클라이언트의 요청을 성공적으로 수신하면, 해당 클라이언트에게 수신완료 메세지를 전송한다.
	+ 해당 클라이언트는 서버로부터 수신완료 메세지를 수신하면 해당 메세지를 수신한다.
	+ 수신완료 메세지가 출력되면 클라이언트는 Log-In 정보인 ID와 PW를 입력 후 전송한다.
	+ 서버는 수신받은 IP와 PW를 미리 등록된 ID와 PW와 비교 후 성공/실패 여부를 반환한다.
	+ 클라이언트는 서버로부터 받은 로그인 결과를 출력한다.
	+ 모든 단계에서 서버는 다중 클라이언트로부터의 요청을 개별적으로 처리 가능해야 한다.
  

+ 출력화면
![User1_Login](https://user-images.githubusercontent.com/68980204/119944026-bc678080-bfce-11eb-87bb-da674579f465.PNG)
  
  
  
  
  
	+ User1 로그인 [Client] && User 로그인 [Server]

![User1+User2_Login](https://user-images.githubusercontent.com/68980204/119943987-afe32800-bfce-11eb-884f-5cb150cc9057.PNG)
  
  
  
  
  
	+ User1, User2 동시 정상 로그인 [Server]

![Log-in_Failed](https://user-images.githubusercontent.com/68980204/119944063-c8ebd900-bfce-11eb-914e-763c159d02a0.PNG)
  
  
  
  
  
	+ 로그인 실패 [Server]
  
  
## Project #2 : 다중 클라이언트 로그인 + 다중 클라이언트 그룹 채팅
------------
![Project #2](https://user-images.githubusercontent.com/68980204/119944234-f8024a80-bfce-11eb-9c8b-a7d2bbfbaa0b.PNG)
	+ Project #2 조직도

![Term #2 Flow chart](https://user-images.githubusercontent.com/68980204/119944748-9d1d2300-bfcf-11eb-9bba-efd17b2e55c4.png)
	+ Project #2 플로우차트
  
Project #2는 지난 Project #1 의 <서버 – 클라이언트 모델>에 의한 로그인이 완료된 이후 다중 클라이언트의 로그인을 허용하여 클라이언트간의 그룹채팅이 가능하도록 구현하는 것이 주된 이슈이다.
따라서 Project #1에서 주로 다루었던 로그인 관련 코드와 알고리즘의 설명은 간결하게 끝내고, 추가된 Project #2와 관련하여 자세하게 설명할 것이다.
우선 Project #2를 수행하기 위해 갖춰야 할 기능들과 이러한 기능을 수행하기 위해 고려했던 방안들은 다음과 같다.

+ [동시 로그인]
Project #2에서 구현하고자 하는 주요 이슈는 <서버와 다중 클라이언트 간의 그룹채팅>이다. Project #1에서는 서버가 하나의 클라이언트의 로그인 처리를 하였지만, 이번 프로젝트에서는 하나의 서버가 여러 명의 클라이언트의 로그인 처리를 수용할 수 있어야한다. 따라서 우리 팀은 이러한 로그인 프로세스를 접속 클라이언트마다 따로 동작되는 병행 프로세스로 설정하기 위하여 로그인을 Multi Thread를 이용하여 구현할 수 있도록 하였다.

+ [클라이언트 간의 메세지 송 수신]
클라이언트 간 채팅을 진행하게 된다면 메시지를 보내는 입력모드로 동작하면서 언제든지 다른 클라이언트로부터 메시지 수신이 가능한 상태로 프로세스가 실행되어야 한다. 그러기 위해 클라이언트에서도 병행 프로세스에 관한 설정이 필요하였고, 이 역시 Multi Thread를 이용하여 동시에 동작할 수 있는 병행 프로세스로 설정하였다.

+ [메세지 입력모드와 수신모드]
클라이언트 간 그룹채팅이 시작되었을 때, 편의를 위해 자신이 보낸 메시지와 다른 클라이언트로부터 받은 메시지의 차이를 두어야하기에 우리는 다른 클라이언트에서 보내온 메시지에 [USERn]을 색깔을 변경하여 붙임으로써 입력모드와 수신모드의 차이를 두었다.

+ 출력화면
![P2_User1+User2_Login](https://user-images.githubusercontent.com/68980204/119945375-52e87180-bfd0-11eb-9182-08ccc9b80d12.PNG)
  
  
  
  
  
	+ User1, User2 동시 정상 로그인

![P2_Chating_Mode](https://user-images.githubusercontent.com/68980204/119945392-5b40ac80-bfd0-11eb-9247-fa1980ccc59c.PNG)
  
  
  
  
  
	+ 채팅모드 진입

![P2_Chating_Mode2](https://user-images.githubusercontent.com/68980204/119945422-6693d800-bfd0-11eb-9f9e-fb718f7fc288.PNG)
  
  
  
  
  
	+ 채팅모드 활성화

![P2_Chating_Mode+theEnd](https://user-images.githubusercontent.com/68980204/119945441-6d224f80-bfd0-11eb-8c6b-c038bee343ba.PNG)
  
  
  
  
  
	+ 채팅 종료
  
  
## Project #3 : 다중 클라이언트 로그인 + 다중 클라이언트 그룹 채팅
------------
![Project #3](https://user-images.githubusercontent.com/68980204/119946009-18cb9f80-bfd1-11eb-9553-c7ad37b96401.PNG)
	+ Project #2 조직도  
  
![Term #3 Flow Chart](https://user-images.githubusercontent.com/68980204/119946032-2123da80-bfd1-11eb-8f78-98563b51514b.png)
	+ Project #2 플로우차트  
  

Project #3는 지난 Project #1을 통해 구축한 <서버 – 클라이언트 모델>에 의한 로그인이 완료된 이후 다중 클라이언트의 로그인을 허용하여 클라이언트간의 그룹채팅이 가능하도록 구현한 Project #2의 기능들을 기반으로 메시지뿐만 아니라 P2P 방식을 이용한 파일전송 시스템까지 구현하는 것을 목표로 한다.
이번 프로젝트에 첨부되는 결과 화면과 코드 관련하여 앞서 진행했던 Project #2와 동일한 부분이 다수 존재하기에 많은 부분을 생략하고, Project #3을 진행하며 변화된 부분을 중심으로 설명할 것이다.
이를 위해 필요한 기능과 그것을 해결한 방안에 대하여 서술하면 다음과 같다.
  

+ [클라이언트의 IP, PORT]
Project #3에서 구현하고자 하는 주요 이슈는 클라이언트 간 P2P 통신을 이용한 파일 전송 시스템이다. 따라서 클라이언트끼리 파일을 주고 받을 때, 클라이언트 중에서도 서버의 역할을 수행하는 클라이언트의 IP와 PORT번호가 필요한데, 이를 위해 로그인 정보를 입력받을 때 IP와 PORT를 함께 서버로 전송하여 서버는 이를 가지고 있어야할 필요가 있다. 따라서 우리는 로그인 과정에서 클라이언트의 IP와 PORT를 함께 받아와 이를 저장하고 [FILE]이라고 입력했을 시에 해당 클라이언트의 IP와 PORT를 전송하는 기능을 구현하였다.

+ [파일전송 모드 설정]
클라이언트가 서버에 접속하여 그룹 채팅을 진행하다가 다른 클라이언트로부터 다운로드 받고 싶은 파일이 존재한다면 파일 전송 모드를 실행시킨다. 이때 그룹채팅 모드와 P2P 파일전송 모드의 차이를 두어야하는데, 우리는 클라이언트가 메시지에 “[FILE]“ 이라고 입력하면 P2P 통신을 위한 소켓 생성 및 연결 등의 준비를 하도록 구현하였다.

+ [파일 전송 후 채팅 여부]
클라이언트 간 그룹채팅이 시작되었을 때, 편의를 위해 자신이 보낸 메시지와 다른 클라이언트로부터 받은 메시지의 차이를 두어야하기에 우리는 다른 클라이언트에서 보내온 메시지에 [USERn]을 색깔을 변경하여 붙임으로써 입력모드와 수신모드의 차이를 두었다.

+ 출력화면
![P3_File List](https://user-images.githubusercontent.com/68980204/119947468-bbd0e900-bfd2-11eb-8d1f-b55bd8780ba1.PNG)
  
  
  
  
  
	+ 파일 리스트 받아오기
  
![P3_File send](https://user-images.githubusercontent.com/68980204/119947505-c55a5100-bfd2-11eb-95c2-73a16217d6cb.PNG)
  
  
  
  
  
	+ 파일 전송
  
![P3_File send+Chating](https://user-images.githubusercontent.com/68980204/119947520-cab79b80-bfd2-11eb-8fdd-d8829c4b0315.PNG)
  
  
  
  
  
	+ 파일 전송 후 채팅

![P3_Chating_Mode+theEnd](https://user-images.githubusercontent.com/68980204/119947563-da36e480-bfd2-11eb-88ef-139e0385e718.PNG)
	+ 채팅 종료

![P3_sendFIleCheck](https://user-images.githubusercontent.com/68980204/119947539-d1461300-bfd2-11eb-8469-db78b09f5e98.PNG)
	+ 받은 파일 확인