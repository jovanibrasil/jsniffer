/*
 *
 * Author: Jovani Brasil
 * Email: jovanibrasil@gmail.com
 *
 */


#ifdef JNI
#include "Sniffer.h"
#include <jni.h>
#endif /* JNI */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

/* Diretorios: net, netinet, linux contem os includes que descrevem as estruturas de dados do header dos protocolos.
 *
 * net/if.h  		- 	Estruturas para gerencia de interfaces de rede.
 * netinet/in_systm.h 	- 	Tipos de dados. 
 * netinet/ether.h 	- 	Estrutura do header ethernet.
 * netinet/in.h 	-	Definicao de protocolos. 
 * arpa/inet.h 		- 	Funcoes para manipulacao de enderecos IP.
 */

#include <net/if.h>  
#include <netinet/in_systm.h> 
#include <netinet/ether.h> 
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <semaphore.h>
#define PORTS 65536
#define BUFFSIZE 1518
#define MAX_BUFFER_SIZE 50

// Buffer node
typedef struct BufferNode {
    unsigned char *content;
    int size;
    int id;
    struct BufferNode *next;
} BufferNode;

// Estrutura auxiliar para contagem de endereços
struct ipv4_node {
	char ip[16];
	int rcounter;
	int tcounter;
	struct ipv4_node *next;
};

struct statistics {
    // Packages counters by type
    int total_packets;
    int count_ipv4;
    int count_ipv6;
    int count_udp;
    int count_tcp;
    int count_icmp4;
    int count_icmp6;
    int count_arp;
    int count_ethernet;
};

// Contador de transmissão pelas portas
int tports[PORTS];
// Contador de recepção pelas portas
int rports[PORTS];


struct statistics *stats = NULL;
/* Header */
struct ipv4_node *ipv4_listhead = NULL;

BufferNode *buffer_node_head = NULL;
BufferNode *buffer_node_tail = NULL;
int buffer_length = 0;
int id = 0;
pthread_mutex_t lock;

/*
 * 
 * Adiciona um elemento da lista de contagem das estatísticas de IP. 
 * Uma possível otimização seria a implementação de uma fila de pri-
 * oridades.
 *
 * TODO Otimizar.
 *
 */
void insert_ipv4(char ip[16], int counter){
	
	// Lista vazia, inicializa
	if(ipv4_listhead == NULL){
		ipv4_listhead = (struct ipv4_node *)malloc(sizeof(struct ipv4_node));
		memcpy(ipv4_listhead->ip, ip, 16 * sizeof(char));
		if(counter)
			ipv4_listhead->rcounter = 1;
		else
			ipv4_listhead->tcounter = 1;
		ipv4_listhead->next = NULL;	
		//printf("Elemento adicionado: %s \n", ipv4_listhead->ip);	
		return;
	}

	// // Procura elemento na lista de elementos
	struct ipv4_node *node = ipv4_listhead; 
	while(node != NULL){
		//printf("%s == %s \n\n", node->ip, ip);
		if(strcmp(node->ip, ip) == 0){
			if(counter)
				ipv4_listhead->rcounter += 1;
			else
				ipv4_listhead->tcounter += 1;
			//printf("Elemento atualizado --------------------------_>\n");
			return;
		}
		node = node->next;
	}

	// Se não achou, cria um elemento e adiciona no inicio
	node = (struct ipv4_node *)malloc(sizeof(struct ipv4_node));
	memcpy(node->ip, ip, 16 * sizeof(char));
	if(counter)
		node->rcounter = 1;
	else
		node->tcounter = 1;
	//printf("Elemento adicionado: %s \n", node->ip);	
	node->next = ipv4_listhead;
	
	ipv4_listhead = node;
	
	return;
}

/*
 *
 *
 *
 *
 */

void print_statistics(){

	system("clear");	
	
	// Percentuais de transmissão de diferentes tipos de pacotes.

	printf("Total de pacotes capturados %d.\n", stats->total_packets);
	printf("Pacotes ARP: %d.\n", stats->count_arp);
	printf("Pacotes IPv4: %d.\n", stats->count_ipv4);
	printf("Pacotes IPv6: %d.\n", stats->count_ipv6);
	printf("Pacotes ICMPv4: %d.\n", stats->count_icmp4);
	printf("Pacotes ICMPv6: %d.\n", stats->count_icmp6);
	printf("Pacotes TCP: %d.\n", stats->count_tcp);
	printf("Pacotes UDP: %d.\n", stats->count_udp);

	// Protocolos de aplicações que mais enviaram e receberam pacotes.
	int i = 0;
	int tmax = 0; // Protocolo que mais transmitiu.
	int rmax = 0; // Protocolo que mais recebeu.
	for(i; i<PORTS; i++){
		if(tports[i] > tmax)
			tmax = i;
		if(rports[i] > rmax)
			rmax = i;	
	}
	
	printf("Protocolo de aplicação mais usado:\n");
	printf("Para transmissões: %d\n", tmax);
	printf("Para recepções: %d\n", rmax);

	// IP dos dispositivos que mais enviaram e receberam pacotes.
	struct ipv4_node *node = ipv4_listhead;
	struct ipv4_node *maxrc = ipv4_listhead, *maxtc = ipv4_listhead;
	while(node != NULL){
		// printf("%s   %d   \n ", node->ip, node->rcounter);
		if(node->rcounter > maxrc->rcounter)
			maxrc = node;
		if(node->tcounter > maxrc->tcounter)
			maxtc = node;
		
		node = node->next;
	}

	printf("IP's mais utilizados para comunicação:\n");
	printf("Para transmissões: %s\n", maxtc->ip);
	printf("PAra recepções: %s\n", maxrc->ip);
			

}


/*
 *
 *
 *
 */

void print_values(unsigned  char *buff, int offset, int length){

	int i;
	for(i=0; i < length; i++){
		printf("%x-", buff[offset + sizeof(unsigned char) * i]);
	}
	printf("\n\n");
}

/*
 * UDP HEADER
 *
 * 0                   16		31
 * ---------------------------------------
 * |    Source port   | Destination port |
 * ---------------------------------------
 * |       Length     |    Checksum      |
 * ---------------------------------------
 *
 *
 * */
jobject* get_udp(unsigned char *buff, int offset, JNIEnv *env, jobject obj){

	printf("\n        >UDP HEADER\n");
	stats->count_udp += 1;

	struct udphdr udp_header;
	memcpy(&udp_header, &buff[offset], sizeof(udp_header));

//	printf("        Source port = %d", udp_header.source);
	tports[udp_header.source] += 1;
//	printf("        Destination Port =  %d\n", udp_header.dest);
	rports[udp_header.dest] += 1;
//	printf("        Length = %d", udp_header.len);
//	printf("        Checksum = %d", udp_header.check);

    jclass cls = (*env)->FindClass(env, "UdpHeader");
//    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>",
//        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>",
        "(IIII)V");

    jobject *object = (*env)->NewObject(env, cls, constructor,
          (int) udp_header.source, // u_short (2 byte unsigned 0 - 65 535)
          (int) udp_header.dest, // u_short
          (int) udp_header.check, // u_short
          (int) udp_header.len); // short (2 byte signed -32 768 - 32 767) -> short

    return object;

}



/* 0      4         78                16			    31
 * -------------------------------------------------------------------
 * |        	 Source Port	     |       Destination Port	     |
 * -------------------------------------------------------------------
 * |	       	              Sequence Number			     |
 * -------------------------------------------------------------------
 * |	            Acknowledgment number (if ACK set)               |
 * -------------------------------------------------------------------
 * | Data |  	   |N|C|E|U|A|P|R|S|F|				     |
 * |Offset|Reserved|S|W|C|R|C|S|S|Y|I|		Window Size          |
 * |	  |	   | |R|E|G|K|H|T|N|N|				     |
 * -------------------------------------------------------------------
 * |		Checksum	     |	  Urgent Pointer (if URG set)|
 * -------------------------------------------------------------------
 * |			Option (if data offset > 5)		     |
 * |	     (Padded at the end with "0" bytes if necessary.)        |
 * -------------------------------------------------------------------
 */

jobject* get_tcp(unsigned char *buff, int offset, JNIEnv *env, jobject obj){
	
	printf("\n        >TCP HEADER\n");
	stats->count_tcp += 1;

	struct tcphdr tcp_header;
	memcpy(&tcp_header, &buff,sizeof(tcp_header));

	tports[tcp_header.source] += 1;
	rports[tcp_header.dest] += 1;

//	printf("        Source Port = %d ", tcp_header.source);
//	printf("Destination Port = %d \n", tcp_header.dest);
//	printf("        Sequence Number = %d \n", tcp_header.seq);
//	printf("        Acknowledment number = %d \n", tcp_header.ack_seq);


//	printf("Window Size = %d \n", tcp_header.window);


//	printf("        Checksum = %d ", tcp_header.check);
//	printf("Urgent Pointer = %d \n", tcp_header.urg_ptr);



//	printf("        Data Offset = %x ", tcp_header.doff);
//	printf("Reserved = %x\n", tcp_header.res1);

	// TODO Arrumar definição na Struct.	
//	printf("NS = %d CWR = %d ECE = %d URG = %d \n", 0, 0, 0,tcp_header.urg);
//	printf("ACK = %d PSH = %d RST = %d", tcp_header.ack, tcp_header.psh, tcp_header.rst);
//	printf("SYN = %d FIN = %d", tcp_header.syn, tcp_header.fin);


	// TODO
	//printf("        Option %x \n");

    jclass cls = (*env)->FindClass(env, "TcpHeader");
//    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>",
//        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>",
        "(IIIIIIIIIIIIIIIIII)V");

    jobject *object = (*env)->NewObject(env, cls, constructor,
          (int) tcp_header.source,
          (int) tcp_header.dest,
          (int) tcp_header.seq,
          (int) tcp_header.ack_seq,
          (int) tcp_header.doff,
          (int) tcp_header.res1,
          0, 0, 0,
          (int) tcp_header.urg,
          (int) tcp_header.ack,
          (int) tcp_header.psh,
          (int) tcp_header.rst,
          (int) tcp_header.syn,
          (int) tcp_header.fin,
          (int) tcp_header.window,
          (int) tcp_header.check,
          (int) tcp_header.urg_ptr);

    return object;
}

/*
 *
 *     8 Bytes    8 Bytes          16 Bytes	
 * -------------------------------------------------
 * |	Type   |   Code   |        Checksum        |
 * -------------------------------------------------
 * |		   Rest of Header                  |
 * -------------------------------------------------
 *
 */

void get_icmp4(unsigned char *buff, int offset, JNIEnv *env, jobject obj){
	
	printf("\n      >ICMP4 HEADER\n");
	stats->count_icmp4 += 1;

	struct icmphdr icmp4_header;
	memcpy(&icmp4_header, &buff, sizeof(icmp4_header));

//	printf("      Type = %d ", icmp4_header.type);
//	printf("Code = %d ", icmp4_header.code);
//	printf("      Checksum = %d \n", icmp4_header.checksum);

}


/*
 *    	   ICMP6 HEADER
 *
 *    0-7   8-15      16-31
 * -----------------------------
 * | Type | Code |   Checksum  |
 * -----------------------------
 * |        Message Body       |
 * -----------------------------
 *
 */
void get_icmp6(unsigned char *buff, int offset, JNIEnv *env, jobject obj){

	printf("\n      >ICMP6 HEADER\n");
	stats->count_icmp6 += 1;

	struct icmp6_hdr icmp6_header;
	memcpy(&icmp6_header,&buff[offset], sizeof(icmp6_header));

//	printf("      Type = %d \n", icmp6_header.icmp6_type);
//	printf("      Code = %d \n", icmp6_header.icmp6_code);
//	printf("      Checkum = %d \n", icmp6_header.icmp6_cksum);

}


/* 				IPV4 HEADER 
 *
 *   4 bytes  4 bytes   6 bytes 2 bytes          16 bytes       
 *  0				     16				    31
 *  ------------------------------------------------------------------
 *  |Version|   IHL   |   DSCP   |ECN|         Total Length          |
 *  ------------------------------------------------------------------
 *  |         Identification         | Frags | 	     Fragment Offset |
 *  ------------------------------------------------------------------
 *  |   TimeToLive    |   Protocol   |        Header Checksum        |
 *  ------------------------------------------------------------------
 *  |  		             Source IP Address		             |
 *  ------------------------------------------------------------------
 *  |  		          Destination IP Address                     |
 *  ------------------------------------------------------------------
 *  |                       Options (if IHL > 5)           	     |
 *  ------------------------------------------------------------------
 *
 * */

jobject *get_ipv4(unsigned char *buff, int offset, JNIEnv *env, jobject obj){

	printf("    >IPV4 HEADER\n");
	stats->count_ipv4 += 1;

	struct ip ip_header;    	

	// Allocates a default 20 bytes header space
	memcpy(&ip_header, &buff[offset] , sizeof(ip_header));
	
//	printf("    Version = %x ", ip_header.ip_v);
//	printf("IHL = %x ", ip_header.ip_hl);
//	printf("DSCP = %x ", ip_header.ip_tos);
//	printf("ECN = ");
//	printf("Total Length = %x \n", ip_header.ip_len);
//
//	printf("    Identification = %x ", ip_header.ip_id);
//	printf("Flags = ");
//	printf("Fragment Offset = %x \n", ip_header.ip_off);
//
//	printf("    Time To Live =  %x ", ip_header.ip_ttl);
//	printf("Protocol = %x ", ip_header.ip_p);
//	printf("Header Checksum = %x \n", ip_header.ip_sum);
//
//	printf("    Source IP Address = %s \n", inet_ntoa(ip_header.ip_src));
//	printf("    Destination IP Address = %s \n", inet_ntoa(ip_header.ip_dst));

	insert_ipv4(inet_ntoa(ip_header.ip_src), 0);
	insert_ipv4(inet_ntoa(ip_header.ip_dst), 1);	

	//printf("Press Any Key to Continue\n");  
	//getchar(); 


	offset += ip_header.ip_hl * 4;

//    printf("----------------------------------------------");
//    jclass cls = (*env)->FindClass(env, "UdpHeader");
//    jmethodID c = (*env)->GetMethodID(env, cls, "<init>", "(IIII)V");
//    jobject *udp = (*env)->NewObject(env, cls, c, 123, 123, 123, 123);
//
//

    jclass clsipv4 = (*env)->FindClass(env, "Ipv4Header");
    jmethodID constructorIpv4 = (*env)->GetMethodID(env, clsipv4, "<init>", "(ILHeader;)V");
    jobject* o = NULL;

	switch(ip_header.ip_p){
		case IPPROTO_ICMP:
			get_icmp4(buff, offset, env, obj);
			break;
		case IPPROTO_TCP:
			o = get_tcp(buff, offset, env, obj);
			break;
		case IPPROTO_UDP:
		    o = get_udp(buff, offset, env, obj);
            break;
		default:
			break;
	}

	if(o){
        return (*env)->NewObject(env, clsipv4, constructorIpv4,
              (int) ip_header.ip_len, // u_short (2 byte unsigned 0 - 65 535)
              o); // short (2 byte signed -32 768 - 32 767) -> short
	}

	return NULL;
}


/*
 *		     IPv6 HEADER
 * 0        4              12     16            24 	  31
 * ---------------------------------------------------------
 * |Version|Traffic Class |           Flow Label           |
 * ---------------------------------------------------------
 * |        Payload Length       | Next header | Hop Limit |
 * ---------------------------------------------------------
 * |                     Source Address       		 4x|
 * ---------------------------------------------------------
 * |                  Destination Address                4x|
 * ---------------------------------------------------------
 *
 *
 */
void get_ipv6(unsigned char *buff, int offset, JNIEnv *env, jobject obj){
	
	printf("    >IPv6. \n");
	stats->count_ipv6 += 1;

	struct ip6_hdr ipv6_header;

	memcpy(&ipv6_header, &buff[offset], sizeof(ipv6_header));	
	
        printf("    Version = %x \n", ipv6_header.ip6_ctlun.ip6_un1.ip6_un1_flow);
        printf("    Traffic Class = %x \n",1);
	printf("    Flow Label = %x \n",1);
	printf("    Payload Length = %x \n", ipv6_header.ip6_ctlun.ip6_un1.ip6_un1_plen);	
	printf("    Next Header = %x \n", ipv6_header.ip6_ctlun.ip6_un1.ip6_un1_nxt);
	printf("    Hop Limit = %x \n", ipv6_header.ip6_ctlun.ip6_un1.ip6_un1_hlim);
	
	//printf("    Source Address = %s \n", inet_ntoa((struct in6_addr)ipv6_header.ip6_src));
	//printf("    Destination Address = %s \n", inet_ntoa(ipv6_header.ip6_dst));
	// TODO IP	
	

	// TODO Ajustar offset

	switch(ipv6_header.ip6_ctlun.ip6_un1.ip6_un1_nxt){
		case IPPROTO_ICMPV6:
			get_icmp6(buff, offset, env, obj);
			break;
		default:
			break;
	}

}

/*
 * 		     ARP HEADER
 * 0						  15
 * -------------------------------------------------
 * |            Hardware Type (HTYPE)		   |
 * -------------------------------------------------
 * |	 	Protocol Type (PTYPE)		   |
 * -------------------------------------------------
 * | 		HLEN*  	    | 	    PLEN* 	   |		
 * -------------------------------------------------
 * |		    Operation (OPER)		   |
 * -------------------------------------------------
 * |         Sender Hardware Address (SHA)         |
 * -------------------------------------------------
 * |						   |
 * -------------------------------------------------
 * |						   |
 * -------------------------------------------------
 * |	     Sender Protocol Address (SPA)	   |
 * -------------------------------------------------
 * |						   |
 * -------------------------------------------------
 * |	     Target Hardware Address (THA)	   |
 * -------------------------------------------------
 * |						   |
 * -------------------------------------------------
 * |						   |
 * -------------------------------------------------
 * |	     Target Protocol Address (TPA)         |
 * -------------------------------------------------
 * |						   |
 * -------------------------------------------------
 *
 * HLEN - Hardware Address Length
 * PLEN - Protocol Address Length
 *
 *
 */
void get_arp(unsigned char *buff, int offset, JNIEnv *env, jobject obj){
	
	printf("    >ARP. \n");
	stats->count_arp += 1;

	struct ether_arp arp_header;

	memcpy(&arp_header, &buff[offset], sizeof(arp_header));

	printf("    Hardware Type = %x\n", arp_header.ea_hdr.ar_hrd);
	printf("    Protocol Type = %x\n", arp_header.ea_hdr.ar_pro);
	printf("    Hardware Address Length = %x ", arp_header.ea_hdr.ar_hln);
	printf("    Protocol Address Length = %x\n", arp_header.ea_hdr.ar_pln);
	printf("    Operation = %x \n", arp_header.ea_hdr.ar_op);
	// TODO This types are strange
	printf("    Sender Hardware Address = %x \n", arp_header.arp_sha);
	printf("    Sender Protocol Address = %x \n", arp_header.arp_spa);
	printf("    Target Hardware Address = %x \n", arp_header.arp_tha);
 	printf("    Target Protocol Address = %x \n", arp_header.arp_tpa);	

}

/*			ETHERNET HEADER
 *
 *
 *     6 bytes      6 bytes       2 bytes
 * ----------------------------------------
 * |  Dest MAC  |  Source MAC  | Ethertype| 
 * ----------------------------------------
 *
 * Destination MAC adress - 6 bytes
 * Source MAC address - 6 bytes
 * Ether Type - 2 bytes 
 * 	Examples: IPv4 (0x0800) IPv6 (0x86DD) ARP (0x0806) 
 *
 */

jobject *get_ethernet(unsigned char *buff, JNIEnv *env, jobject obj){
	
	printf(">ETHERNET HEADER\n");
	stats->count_ethernet += 1;

	struct ether_header ethernet_header;
	int offset = 0;

	memcpy(&ethernet_header, buff, sizeof(ethernet_header));

	//print_values(buff, 0, 40);

//	printf("MAC Destino: %x:%x:%x:%x:%x:%x \n", buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
	
	// header.ether_dhost[i]
		
//	printf("MAC Origem:  %x:%x:%x:%x:%x:%x \n", buff[6],buff[7],buff[8],buff[9],buff[10],buff[11]);

	//printf("Ether type: 0x%x%x \n", buff[12], buff[13]);

	//printf("0x%04x --- (htons) 0x%04x \n\n", header.ether_type, htons(header.ether_type));

//	printf("Ether Type: 0x%04x \n\n", htons(ethernet_header.ether_type));


	offset += 14;
	// IPv4 | IPv6 | ARP		

	switch (htons(ethernet_header.ether_type)){
		case ETHERTYPE_IP:
			return get_ipv4(buff, offset, env, obj);
			break;
		case ETHERTYPE_IPV6:
			get_ipv6(buff, offset, env, obj);
			break;
		case ETHERTYPE_ARP:
			get_arp(buff, offset, env, obj);
			break;
		default:
			// printf("Press Any Key to Continue\n");  
			// getchar(); 
			printf("[Error] Unknown protocol. \n\n");
	}
    return NULL;
}

jobject *process_packet(unsigned char *buff, JNIEnv *env, jobject obj){
	return get_ethernet(buff, env, obj);
}

void run(){
	unsigned char buff[BUFFSIZE];

    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init failed\n");
        return 1;
    }

    stats = (struct statistics*) malloc(sizeof(struct statistics));

  	int sockd;
  	int on;
  	struct ifreq ifr;
	
	/* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
	/* De um "man" para ver os parametros.*/
	/* htons: converte um short (2-byte) integer para standard network byte order. */
	if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		printf("An error occurred when opening the socket.\n");
		exit(1);
	}

	// O procedimento abaixo eh utilizado para configurar a interface em modo promiscuo
	strcpy(ifr.ifr_name, "wlp6s0");
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("erro no ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

    int packetLen = 0;
	// recepcao de pacotes
	while (1) {
		packetLen = recv(sockd, (char *) &buff, sizeof(buff), 0x0);

		if(packetLen >= 0) {
		    id++;
		    pthread_mutex_lock(&lock);
            stats->total_packets += 1;
		    //printf("Packet Received. Size: %i\n", packetLen);
		    if(buffer_length == MAX_BUFFER_SIZE){
//		        printf("Removing package %d\n", buffer_node_head->id);
		        BufferNode *old = buffer_node_head;
		        buffer_node_head = buffer_node_head->next;
		        free(old);
		        buffer_length--;
		    }

            buffer_length++;

//            printf("HEad is %s and tail is %s\n", buffer_node_head, buffer_node_tail);

		    if(buffer_node_tail == NULL || buffer_node_head == NULL) {
//                printf("Buffering tail/head package ...\n");
                buffer_node_tail = (BufferNode*) malloc(sizeof(BufferNode) + packetLen);
                buffer_node_tail->id = id;
                buffer_node_tail->next = NULL;
                buffer_node_tail->size = packetLen;
                buffer_node_tail->content = malloc(packetLen);
                memcpy(buffer_node_tail->content, buff, packetLen); // received package content
                buffer_node_head = buffer_node_tail;
		    } else{
//                printf("Buffering package ...\n");
                BufferNode *newNode = (BufferNode*) malloc(sizeof(BufferNode) + packetLen);
                newNode->id = id;
                newNode->size = packetLen;
                newNode->next = NULL;
                newNode->content = malloc(packetLen);
                memcpy(newNode->content, buff, packetLen); // received package content
                buffer_node_tail->next = newNode;
                buffer_node_tail = newNode;
                //printf("Create package with id %d\n", buffer_node_tail->id);
		    }
            //printf("Buffer length: %i\n", buffer_length);
            pthread_mutex_unlock(&lock);
		}

		//print_statistics();
	}
	pthread_mutex_destroy(&lock);
}


int main(int argc,char *argv[]){
	run();
}

#ifdef JNI

JNIEXPORT jobjectArray JNICALL Java_Sniffer_getPacketsFromBuffer(JNIEnv *env, jobject obj, jint quantity){

    //printf("Getting buffer with size %d. \n", (int) quantity);
    jobjectArray strarr = (*env)->NewObjectArray(env, (int)quantity,
            (*env)->FindClass(env, "Ipv4Header"), NULL);

    //printf("Buffer length %d\n", buffer_length);
    int idx = 0;
    while(quantity > 1){
        BufferNode *node = NULL;
//        fflush(stdout);
        pthread_mutex_lock(&lock);
        if(buffer_length == 0) {
//            printf("------------> Empty buffer\n");
            fflush(stdout);
            buffer_node_tail = NULL;
            buffer_node_head = NULL;
            pthread_mutex_unlock(&lock);
            break; // there are no packets to return
        }

        node = buffer_node_head;
        buffer_node_head = buffer_node_head->next;
        buffer_length--;
//        printf("Processing package id %d\n", node->id);
        fflush(stdout);
        idx++;
        quantity--;

       jobject *object = process_packet(node->content, env, obj);

       if(object){
           (*env)->SetObjectArrayElement(env, strarr, 0, object);
       }else{
           printf("Not a UDP package\n");
       }

       free(node); // free memory
        pthread_mutex_unlock(&lock);

    }
    return strarr;
}

JNIEXPORT void JNICALL Java_Sniffer_run(JNIEnv *env, jobject obj) {
    run();
}

JNIEXPORT jobjectArray JNICALL Java_Sniffer_getPrintBuffer(JNIEnv *env, jobject obj){
    jobjectArray strarr = (*env)->NewObjectArray(env, 5,
        (*env)->FindClass(env, "java/lang/String"), NULL);

    for (int i = 0; i < 5; ++i){
        (*env)->SetObjectArrayElement(env, strarr, i, (*env)->NewStringUTF(env, "Teste\n"));
    }

    return strarr;
}

JNIEXPORT jobject JNICALL Java_Sniffer_getStatistics(JNIEnv *env, jobject obj){

    // Create the object of the class Statistics
    jclass statisticsClass = (*env)->FindClass(env, "Statistics");
    jobject statisticsData = (*env)->AllocObject(env, statisticsClass);

    // Get the UserData fields to be set
    jfieldID totalPacketsField = (*env)->GetFieldID(env, statisticsClass , "totalPackets", "I");
    jfieldID countIpv4Field = (*env)->GetFieldID(env, statisticsClass , "countIpv4", "I");
    jfieldID countIpv6Field = (*env)->GetFieldID(env, statisticsClass , "countIpv6", "I");
    jfieldID countUdpField = (*env)->GetFieldID(env, statisticsClass , "countUdp", "I");
    jfieldID countTcpField = (*env)->GetFieldID(env, statisticsClass , "countTcp", "I");
    jfieldID countIcmp4Field = (*env)->GetFieldID(env, statisticsClass , "countIcmp4", "I");
    jfieldID countIcmp6Field = (*env)->GetFieldID(env, statisticsClass , "countIcmp6", "I");
    jfieldID countArpField = (*env)->GetFieldID(env, statisticsClass , "countArp", "I");
    jfieldID countEthernetField = (*env)->GetFieldID(env, statisticsClass , "countEthernet", "I");

    (*env)->SetIntField(env, statisticsData, totalPacketsField, stats->total_packets);
    (*env)->SetIntField(env, statisticsData, countIpv4Field, stats->count_ipv4);
    (*env)->SetIntField(env, statisticsData, countIpv6Field, stats->count_ipv6);
    (*env)->SetIntField(env, statisticsData, countUdpField, stats->count_udp);
    (*env)->SetIntField(env, statisticsData, countTcpField, stats->count_tcp);
    (*env)->SetIntField(env, statisticsData, countIcmp4Field, stats->count_icmp4);
    (*env)->SetIntField(env, statisticsData, countIcmp6Field, stats->count_icmp6);
    (*env)->SetIntField(env, statisticsData, countArpField, stats->count_arp);
    (*env)->SetIntField(env, statisticsData, countEthernetField, stats->count_ethernet);

    return statisticsData;
}

#endif /* JNI */
