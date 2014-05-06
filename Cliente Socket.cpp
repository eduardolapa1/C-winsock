#define _WIN32_WINNT 0x501 //por algum motivo obscuro eu tenho que colocar isso antes de incluir os headers senão da merda
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#define PORTA_DEFAULT "80"
#define BUFLEN 512
#pragma comment(lib, "Ws2_32.lib")

//add -lws2_32 no linker
int main() {
	
	WSADATA wsadata; //estrutura de dados contendo informações sobre a versão do scoket utilizada
	int MeuResultado;
    char msg[BUFLEN];
	MeuResultado = WSAStartup(MAKEWORD(2,2),&wsadata); //inicializa socket versão 2.2
	char recvbuf[BUFLEN]; //variavel que vai receber os dados
	if(MeuResultado != 0 ){
		printf("Problema na inicialização do socket!");
		return 1;
	}
	
	struct addrinfo *resultado = NULL,*ptr=NULL,hints;
	ZeroMemory(&hints,sizeof(hints)); //zera a estrutura de dados
	hints.ai_family = AF_UNSPEC; //familia de endereço não especificada...pode ser ipv4 ou ipv6,possiveis valores estão disponiveis em ws2def ou http://msdn.microsoft.com/en-us/library/windows/desktop/ms737530(v=vs.85).aspx
	hints.ai_socktype = SOCK_STREAM; //socket TCP/IP
	hints.ai_protocol = IPPROTO_TCP; 
	
	MeuResultado = getaddrinfo("127.0.0.1",PORTA_DEFAULT,&hints,&resultado);  /*getaddrinfo transforma endereço ANSI em um endereço valido,em resultado sera salvo o 
																					  estrutura valida do endereço.hints contém as opções posiveis de conexao*/
	if(MeuResultado != 0 ){
		printf("Falha getaddrinfo:%d\n",MeuResultado);
		WSACleanup();
		getch();
		return 1;
	} 
	
	SOCKET ConexaoSocket = INVALID_SOCKET;
	ptr=resultado;
	ConexaoSocket =  socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol); //cria socket
	
	if(ConexaoSocket == INVALID_SOCKET ){
		printf("Erro ao criar socket!:%d\n",ConexaoSocket);
		freeaddrinfo(resultado);
		WSACleanup(); // descarrega DLL
		getch();
		return 1;
	}
	
	MeuResultado = connect(ConexaoSocket,ptr->ai_addr,(int)ptr->ai_addrlen); //conecta no host
	
	if(MeuResultado == SOCKET_ERROR){
		printf("Erro conect!\n");
		closesocket(ConexaoSocket);
		ConexaoSocket = INVALID_SOCKET;
		WSACleanup();
		getch();
		return 1;
	}
	
	else{
		
	do{
			scanf("%s",msg);
			MeuResultado = send(ConexaoSocket,msg,strlen(msg),0);
			MeuResultado = recv(ConexaoSocket,recvbuf,BUFLEN,0);
			if(MeuResultado>0){
				printf("bytes recebidos:%d\n",MeuResultado);
				printf("%s\n",recvbuf);
			}
			else if(MeuResultado==0)
				printf("Conexão terminada!");	
			else
				printf("Falha na conexão\n");
			ZeroMemory(&recvbuf,sizeof(recvbuf)); //limpa recvbuf 
			}while(MeuResultado>0);
   }
   
	freeaddrinfo(resultado);
	closesocket(ConexaoSocket);
	getch();
    return 0;
}

