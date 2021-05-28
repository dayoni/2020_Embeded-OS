# Embeded-OS
------------
* __개발환경 및 컴파일러__  
	+ 개발환경 : Ubuntu 16.04.3 LTS (GNU/Linux 4.10.0-28-generic x86_64)  
	+ 컴파일러
		+ p2_client1.c -> gcc -o p2_client p2_client.c -lpthread  
		+ p2_server.c -> gcc -o p2_server p2_server.c -lpthread  

* TCP/IP 소켓 프로그래밍을 통한 서버와 클라이언트 소켓 할당  
* __다중 클라이언트 사이의 그룹 채팅__  
* 클라이언트 간 P2P 파일 전송  


소스 코드와 함께 전체 프로세스 설명이 일부 포함되어 있습니다.  
프로젝트에 대한 질문은 언제나 환영입니다!  


__📥 E-mail : <dayonits2@gmail.com>__


</br>


</br>


</br>


</br>


</br>


</br>


## Project #2 : 다중 클라이언트 로그인 + 다중 클라이언트 그룹 채팅
------------
<img src="https://user-images.githubusercontent.com/68980204/119944234-f8024a80-bfce-11eb-9c8b-a7d2bbfbaa0b.PNG"  width="700" height="300">  


	▷ Project #2 조직도



<img src="https://user-images.githubusercontent.com/68980204/119944748-9d1d2300-bfcf-11eb-9bba-efd17b2e55c4.png"  width="800" height="500">  


	▷ Project #2 플로우차트




  
</br>


</br>
Project #2는 지난 Project #1 의 <서버 – 클라이언트 모델>에 의한 로그인이 완료된 이후 다중 클라이언트의 로그인을 허용하여 클라이언트간의 그룹채팅이 가능하도록 구현하는 것이 주된 이슈이다.
따라서 Project #1에서 주로 다루었던 로그인 관련 코드와 알고리즘의 설명은 간결하게 끝내고, 추가된 Project #2와 관련하여 자세하게 설명할 것이다.
우선 Project #2를 수행하기 위해 갖춰야 할 기능들과 이러한 기능을 수행하기 위해 고려했던 방안들은 다음과 같다.



</br>


</br>


+ __[동시 로그인]__  
Project #2에서 구현하고자 하는 주요 이슈는 <서버와 다중 클라이언트 간의 그룹채팅>이다. Project #1에서는 서버가 하나의 클라이언트의 로그인 처리를 하였지만, 이번 프로젝트에서는 하나의 서버가 여러 명의 클라이언트의 로그인 처리를 수용할 수 있어야한다. 따라서 우리 팀은 이러한 로그인 프로세스를 접속 클라이언트마다 따로 동작되는 병행 프로세스로 설정하기 위하여 로그인을 Multi Thread를 이용하여 구현할 수 있도록 하였다.

+ __[클라이언트 간의 메세지 송 수신]__  
클라이언트 간 채팅을 진행하게 된다면 메시지를 보내는 입력모드로 동작하면서 언제든지 다른 클라이언트로부터 메시지 수신이 가능한 상태로 프로세스가 실행되어야 한다. 그러기 위해 클라이언트에서도 병행 프로세스에 관한 설정이 필요하였고, 이 역시 Multi Thread를 이용하여 동시에 동작할 수 있는 병행 프로세스로 설정하였다.

+ __[메세지 입력모드와 수신모드]__  
클라이언트 간 그룹채팅이 시작되었을 때, 편의를 위해 자신이 보낸 메시지와 다른 클라이언트로부터 받은 메시지의 차이를 두어야하기에 우리는 다른 클라이언트에서 보내온 메시지에 [USERn]을 색깔을 변경하여 붙임으로써 입력모드와 수신모드의 차이를 두었다.




</br>


</br>


+ __출력화면__  
<img src="https://user-images.githubusercontent.com/68980204/119945375-52e87180-bfd0-11eb-9182-08ccc9b80d12.PNG"  width="600" height="370">



	▷ User1, User2 동시 정상 로그인



<img src="https://user-images.githubusercontent.com/68980204/119945392-5b40ac80-bfd0-11eb-9247-fa1980ccc59c.PNG"  width="600" height="370">



	▷ 채팅모드 진입



<img src="https://user-images.githubusercontent.com/68980204/119945422-6693d800-bfd0-11eb-9f9e-fb718f7fc288.PNG"  width="600" height="370">



	▷ 채팅모드 활성화



<img src="https://user-images.githubusercontent.com/68980204/119945441-6d224f80-bfd0-11eb-8c6b-c038bee343ba.PNG"  width="600" height="370">



	▷ 채팅 종료



  
</br>


</br>

