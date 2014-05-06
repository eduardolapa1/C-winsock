#define _WIN32_WINNT 0x501 //por algum motivo obscuro eu tenho que colocar isso antes de incluir os headers senão da merda
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#define DEFAULT_PORT "80"
#define MAXTHREADS 30
DWORD WINAPI  ClientThread(LPVOID pParam);
typedef struct UserDate{
	SOCKET Client;
}Dados,*pDados;
int val3=89;

int _tmain() {
	//define a estrutura de dados
	struct addrinfo *result=NULL,hints,*ptr=NULL;
	int MeuResultado;
	pDados pDadosArray[MAXTHREADS];
	WSADATA wsadata;
	int i=0;
	WSAStartup(MAKEWORD(2,2),&wsadata);
	ZeroMemory(&hints,sizeof(hints));
	hints.ai_family = AF_INET; //familia ipv4
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; //indica que vai usar a estrturua na função bind
	//converte endereço ANSI para estrutura socket
	MeuResultado = getaddrinfo(NULL,DEFAULT_PORT,&hints,&result); //converte o endereço local e porta para estrutura 
 
	if(MeuResultado != 0 ){ //deu merda
		printf("Problema na função getaddrinfo:%d\n",MeuResultado);
		WSACleanup();
		getch();
		return 1;
	}
	
	  for( int x=0; x<MAXTHREADS; x++ ) {
    	   pDadosArray[x] = (pDados) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(Dados)); //aloca espaço para os dados
    	   if( pDadosArray[x] == NULL ){
            ExitProcess(2);
          }
		pDadosArray[x]->Client = INVALID_SOCKET;
	}
    		
	//cria socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
    	printf("Erro no Socket: %d\n", WSAGetLastError());
    	freeaddrinfo(result);
    	WSACleanup();
   	    getch();
		return 1;
	}
	
	//vincula  endereço e porta 
	MeuResultado = bind(ListenSocket,result->ai_addr,(int)result->ai_addrlen);
	if (MeuResultado == SOCKET_ERROR) {
        printf("bind falhou:%d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        getch();
        return 1;
    }
    
    //escutar conexões
    if (listen(ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) { //SOMAXCONN permite um numero de conexões pendentes muito alta
    	printf("Listen falhou:%d",WSAGetLastError());
    	freeaddrinfo(result);
    	closesocket(ListenSocket);
    	WSACleanup();
    	getch();
    	return 1;
    }
    
    //aceitar conexões
    SOCKET ClientSocket = INVALID_SOCKET;
    while(1){
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
    	printf("accept falhou: %d\n", WSAGetLastError());
    	closesocket(ListenSocket);
   		 WSACleanup();
    	return 1;
	}
	printf("Numero Conexao:%d\n",i);
	pDadosArray[i]->Client = ClientSocket;
	 CreateThread(NULL,0,ClientThread,(LPVOID)pDadosArray[i],0,NULL); 
	 i++;
   // freeaddrinfo(result);
   if(i>29)
   	i=0;
	}
	return 0;
}




DWORD WINAPI  ClientThread(LPVOID pParam){
	
	pDados pDadosArray =(pDados)pParam;
	char buff[512];
	char recvbuf[512];
	char ch;
	/*FILE *file;
	file=fopen("index.html","r");
	while(getc(file) != EOF){
		ch = getc(file);
		strcat(buff,ch);
	}*/
	sprintf(buff,"<html><head><title>teste</title></head><body><h1>Servidor Rodando...</h1></body></html>");
	send(pDadosArray->Client,buff,strlen(buff),0);
	recv(pDadosArray->Client,recvbuf,512,0);
	printf("%s",recvbuf);
    shutdown(pDadosArray->Client, SD_SEND);
   // fclose(file);
	return 0;
}
