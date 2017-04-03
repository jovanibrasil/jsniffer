/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - Captura pacotes recebidos na interface */
/*-------------------------------------------------------------*/

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
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 

#define BUFFSIZE 1518

/*
 * PQ buffsize com 1518?
 *	Pois eh o tamanho maximo de um frame ethernet, dado os 14 bytes de ethernet header,
 *	4 bytes de CRC Checksum, e de 46  1500 bytes de dados (payload). 
 *
 * Como funciona a fragmentacao?
 *
 *
 * Iteration 1
 *
 *
 * TODO PT1
 *
 * Cabecalhos que devem ser capturados pelo Sniffer: 
 *
 * v Ethernet
 * 
 * TODO Arp
 * v IPv4
 * TODO IPv6
 * 
 * TODO ICMP - 0x01
 * TODO ICMPv6 - 0x3A
 * v UDP - 0x11
 * TODO TCP -0x06
 *
 * TODO PT2
 *
 * TODO PT3
 *
 *
 *
 *
 *
 *
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
	printf("\n    >UDP HEADER\n");
	
	struct udphdr udp_header;

	memcpy(&udp_header, &buff[offset], sizeof(udp_header));

	printf("    Source port = %x", udp_header.uh_sport);
	printf("    Destination Port =  %x\n", udp_header.uh_dport);
	printf("    Length = %x", udp_header.uh_ulen);
	printf("    Checksum = %x", udp_header.uh_sum);

	// TODO
	// Here we need to know what applications are sending and receiving this 
	// package, respectivealy.

}

/*
 * TODO 
 *
 *
 *
 *
 *
 *
 *
 *
 */

void get_tcp(unsigned char *buff, int offset){
	printf("\n    >TCP HEADER\n");

}

void get_icmp(unsigned char *buff, int offset){
	printf("\n    >ICMP HEADER\n");

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

	printf("  >IPV4 HEADER\n");

	struct ip ip_header;    	

	// Allocates a default 20 bytes header space
	memcpy(&ip_header, &buff[offset] , sizeof(ip_header));
	
	printf("  Version = %x ", ip_header.ip_v);
	printf("IHL = %x ", ip_header.ip_hl);
	printf("DSCP = %x ", ip_header.ip_tos);
	printf("ECN = ");
	printf("Total Length = %x \n", ip_header.ip_len);
	
	printf("  Identification = %x ", ip_header.ip_id);
	printf("Flags = ");
	printf("Fragment Offset = %x \n", ip_header.ip_off);
	
	printf("  Time To Live =  %x ", ip_header.ip_ttl);
	printf("Protocol = %x ", ip_header.ip_p);
	printf("Header Checksum = %x \n", ip_header.ip_sum);
	
	printf("  Source IP Address = %x \n", ip_header.ip_src.s_addr);

	printf("  Destination IP Address = %x \n", ip_header.ip_dst.s_addr);	

	//offfset +=  

	//printf("%d - %x \n", ip_header.ip_p, ip_header.ip_p);
		
	switch(ip_header.ip_p){
		case IPPROTO_ICMP:
			get_icmp(buff, offset);
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

	// TODO Can I get manually a header size?
	// TODO Can I instantiate ipv4 structure using the header size encountered?

}

void get_ipv6(unsigned char *buff, int offset){
	printf("  >IPv6. \n\n");

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
	printf("  >ARP. \n\n");

	struct ether_arp arp_header;

	memcpy(&arp_header, &buff[offset], sizeof(arp_header));

	printf("Hardware Type = %x", arp_header.ea_hdr.ar_hrd);
	printf("Protocol Type = %x", arp_header.ea_hdr.ar_pro);
	printf("Hardware Address Length = %x", arp_header.ea_hdr.ar_hln);
	printf("Protocol Address Length = %x", arp_header.ea_hdr.ar_pln);
	printf("Operation = %x \n", arp_header.ea_hdr.ar_op);
	// TODO This types are strange
	printf("Sender Hardware Address = %x \n", arp_header.arp_sha);
	printf("Sender Protocol Address = %x \n", arp_header.arp_spa);
	printf("Target Hardware Address = %x \n", arp_header.arp_tha);
 	printf("Target Protocol Address = %x \n", arp_header.arp_tpa);	

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

	struct ether_header ethernet_header;
	int offset = 0;

	memcpy(&ethernet_header, buff, sizeof(ethernet_header));

	// print_values(buff, 0, 6);

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
			printf("[Error] Unknown protocol. \n\n");
	}

	// TODO
	//free(&header);

}

int process_data(unsigned char *buff){
	get_ethernet(buff);
}

int main(int argc,char *argv[]){

	unsigned char buff[BUFFSIZE];

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

	// O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
	strcpy(ifr.ifr_name, "eth0");
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("erro no ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

	// recepcao de pacotes
	while (1) {
		printf("\n\n------------------------------------------------\n");
		recv(sockd, (char *) &buff, sizeof(buff), 0x0);
		// impressão do conteudo - exemplo Endereco Destino e Endereco Origem
		process_data(buff);		
	}

}
