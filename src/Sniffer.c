/*
 *
 * Author: jovani Brasil	
 * Email: jovanibrasil@gmail.com
 *
 */

#include "Sniffer.h"
#include <jni.h>

#include <stdlib.h>
#include <stdio.h>
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

#define PORTS 65536
#define BUFFSIZE 1518

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
struct ipv4_node *ipv4_listhead = NULL;

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

void get_udp(unsigned char *buff, int offset){
	
	printf("\n        >UDP HEADER\n");
	stats->count_udp += 1;

	struct udphdr udp_header;
	memcpy(&udp_header, &buff[offset], sizeof(udp_header));

	printf("        Source port = %d", udp_header.source);
	tports[udp_header.source] += 1;	
	printf("        Destination Port =  %d\n", udp_header.dest);
	rports[udp_header.dest] += 1;
	printf("        Length = %d", udp_header.len);
	printf("        Checksum = %d", udp_header.check);

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

void get_tcp(unsigned char *buff, int offset){
	
	printf("\n        >TCP HEADER\n");
	stats->count_tcp += 1;

	struct tcphdr tcp_header;
	memcpy(&tcp_header, &buff,sizeof(tcp_header));

	printf("        Source Port = %d ", tcp_header.source);
	tports[tcp_header.source] += 1;
	printf("Destination Port = %d \n", tcp_header.dest);
	rports[tcp_header.dest] += 1;
	printf("        Sequence Number = %d \n", tcp_header.seq);
	printf("        Acknowledment number = %d \n", tcp_header.ack_seq);
	printf("        Data Offset = %x ", tcp_header.doff);
	printf("Reserved = %x\n", tcp_header.res1);

	// TODO Arrumar definição na Struct.	
	printf("NS = %d CWR = %d ECE = %d URG = %d \n", 0, 0, 0,tcp_header.urg);
	printf("ACK = %d PSH = %d RST = %d", tcp_header.ack, tcp_header.psh, tcp_header.rst);
	printf("SYN = %d FIN = %d", tcp_header.syn, tcp_header.fin);

	printf("Window Size = %d \n", tcp_header.window);
	printf("        Checksum = %d ", tcp_header.check);
	printf("Urgent Pointer = %d \n", tcp_header.urg_ptr);
	

	// TODO
	//printf("        Option %x \n");

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

void get_icmp4(unsigned char *buff, int offset){
	
	printf("\n      >ICMP4 HEADER\n");
	stats->count_icmp4 += 1;

	struct icmphdr icmp4_header;
	memcpy(&icmp4_header, &buff, sizeof(icmp4_header));

	printf("      Type = %d ", icmp4_header.type);
	printf("Code = %d ", icmp4_header.code);
	printf("      Checksum = %d \n", icmp4_header.checksum);

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
void get_icmp6(unsigned char *buff, int offset){

	printf("\n      >ICMP6 HEADER\n");
	stats->count_icmp6 += 1;

	struct icmp6_hdr icmp6_header;
	memcpy(&icmp6_header,&buff[offset], sizeof(icmp6_header));

	printf("      Type = %d \n", icmp6_header.icmp6_type);
	printf("      Code = %d \n", icmp6_header.icmp6_code);
	printf("      Checkum = %d \n", icmp6_header.icmp6_cksum);

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

void get_ipv4(unsigned char *buff, int offset){

	printf("    >IPV4 HEADER\n");
	stats->count_ipv4 += 1;

	struct ip ip_header;    	

	// Allocates a default 20 bytes header space
	memcpy(&ip_header, &buff[offset] , sizeof(ip_header));
	
	printf("    Version = %x ", ip_header.ip_v);
	printf("IHL = %x ", ip_header.ip_hl);
	printf("DSCP = %x ", ip_header.ip_tos);
	printf("ECN = ");
	printf("Total Length = %x \n", ip_header.ip_len);
	
	printf("    Identification = %x ", ip_header.ip_id);
	printf("Flags = ");
	printf("Fragment Offset = %x \n", ip_header.ip_off);
	
	printf("    Time To Live =  %x ", ip_header.ip_ttl);
	printf("Protocol = %x ", ip_header.ip_p);
	printf("Header Checksum = %x \n", ip_header.ip_sum);
	
	printf("    Source IP Address = %s \n", inet_ntoa(ip_header.ip_src));
	printf("    Destination IP Address = %s \n", inet_ntoa(ip_header.ip_dst));	

	insert_ipv4(inet_ntoa(ip_header.ip_src), 0);
	insert_ipv4(inet_ntoa(ip_header.ip_dst), 1);	

	//printf("Press Any Key to Continue\n");  
	//getchar(); 


	offset += ip_header.ip_hl * 4;

	printf("%d - %x \n", ip_header.ip_p, ip_header.ip_p);
		
	switch(ip_header.ip_p){
		case IPPROTO_ICMP:
			get_icmp4(buff, offset);
			break;
		case IPPROTO_TCP:
			get_tcp(buff, offset);
			break;
		case IPPROTO_UDP:
			get_udp(buff, offset);
			break;
		default:
			break;
	}
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
void get_ipv6(unsigned char *buff, int offset){
	
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
			get_icmp6(buff, offset);
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
void get_arp(unsigned char *buff, int offset){
	
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

void get_ethernet(unsigned char *buff){
	
	printf(">ETHERNET HEADER\n");
	stats->count_ethernet += 1;

	struct ether_header ethernet_header;
	int offset = 0;

	memcpy(&ethernet_header, buff, sizeof(ethernet_header));

	//print_values(buff, 0, 40);

	printf("MAC Destino: %x:%x:%x:%x:%x:%x \n", buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
	
	// header.ether_dhost[i]
		
	printf("MAC Origem:  %x:%x:%x:%x:%x:%x \n", buff[6],buff[7],buff[8],buff[9],buff[10],buff[11]);

	//printf("Ether type: 0x%x%x \n", buff[12], buff[13]);

	//printf("0x%04x --- (htons) 0x%04x \n\n", header.ether_type, htons(header.ether_type));

	printf("Ether Type: 0x%04x \n\n", htons(ethernet_header.ether_type));


	offset += 14;
	// IPv4 | IPv6 | ARP		

	switch (htons(ethernet_header.ether_type)){
		case ETHERTYPE_IP:
			get_ipv4(buff, offset);
			break;
		case ETHERTYPE_IPV6:
			get_ipv6(buff, offset);
			break;
		case ETHERTYPE_ARP:
			get_arp(buff, offset);
			break;
		default:
			// printf("Press Any Key to Continue\n");  
			// getchar(); 
			printf("[Error] Unknown protocol. \n\n");
	}

}

int process_packet(unsigned char *buff){
	get_ethernet(buff);
}

void run(){
	unsigned char buff[BUFFSIZE];

    stats = (struct statistics*) malloc(sizeof(struct statistics));

  	int sockd;
  	int on;
  	struct ifreq ifr;
	
	/* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
	/* De um "man" para ver os parametros.*/
	/* htons: converte um short (2-byte) integer para standard network byte order. */
	if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		printf("Erro na criacao do socket.\n");
		exit(1);
	}

	// O procedimento abaixo eh utilizado para configurar a interface em modo promiscuo
	strcpy(ifr.ifr_name, "wlp6s0");
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("erro no ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);


	// recepcao de pacotes
	while (1) {
		printf("\n\n---------------------------------------------------------------------\n");
		recv(sockd, (char *) &buff, sizeof(buff), 0x0);	
		process_packet(buff);		
		
		stats->total_packets += 1;

		print_statistics();
	}
}

int main(int argc,char *argv[]){
	run();
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

