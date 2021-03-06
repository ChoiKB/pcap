nclude <pcap.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   pcap_t *handle;         /* Session handle */
   char *dev;         /* The device to sniff on */
   char errbuf[PCAP_ERRBUF_SIZE];   /* Error string */
   struct bpf_program fp;      /* The compiled filter */
   char filter_exp[] = "port 80";   /* The filter expression */
   bpf_u_int32 mask;      /* Our netmask */
   bpf_u_int32 net;      /* Our IP */
   struct pcap_pkthdr header;   /* The header that pcap gives us */
   const u_char *packet;      /* The actual packet */
   int pcap_stat=0;            /*계속 진행해주기 위한 코드*/


   /* Define the device */
   dev = pcap_lookupdev(errbuf);
   if (dev == NULL) {
      fprintf(stderr, "기본 장치를 찾지 못했습니다. : %s\n", errbuf);
      return(2);
   }
   /* Find the properties for the device */
   if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
      fprintf(stderr, "장치로부터 netstat 값을 가져올 수 없습니다. %s: %s\n", dev, errbuf);
      net = 0;
      mask = 0;
   }
   /* 세션 열기 */
   handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
   if (handle == NULL) {
      fprintf(stderr, "장치를 열 업습니다. %s: %s\n", dev, errbuf);
      return(2);
   }
   /* 필터 컴파일, 적용 */
   if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
      fprintf(stderr, "필터를 분석하지 못했습니다. %s: %s\n", filter_exp, pcap_geterr(handle));
      return(2);
   }
   if (pcap_setfilter(handle, &fp) == -1) {
      fprintf(stderr, "필터를 설치할 수 없습니다. %s: %s\n", filter_exp, pcap_geterr(handle));
      return(2);

   }


   while(1)
   {
      
      /* 패킷 잡기 */
      pcap_stat=pcap_next_ex(handle, &header, &packet);

      /* 패킷 길이 출력 */
      printf("패킷 길이는 [%d]이다.\n", header.len);
      if(pcap_stat == 0)
      {   
         continue;   
      }

      printf("아래는 이더넷 헤더 정보이다.\n");
      printf("목적지 MAC 주소: %02x:%02x:%02x:%02x:%02x:%02x \n",packet[0],packet[1],packet[2],packet[3],packet[4],packet[5]);
      printf("출발지 MAC 주소: %02x:%02x:%02x:%02x:%02x:%02x \n",packet[6],packet[7],packet[8],packet[9],packet[10],packet[11]);

      /*'%02x'가 16진수로 표시하되 2자리로 표시하라는 것임을 새롭게 알게 됐다.*/

      /*만약 08 00 값을 받았을 때 이 ip 헤더는 ~
      소스 ip 주소는 패킷 26번, 27번, 28번, 29번의 번호를 출력
      목적지 ip 주소는 패킷 30, 31, 32, 33번의 번호를 출력*/

      if(packet[12]==0x08 && packet[13]==0x00)
      {
         printf("아래는 IP 헤더 정보이다. \n");
         printf("Source IP Address: %d.%d.%d.%d \n",packet[26], packet[27], packet[28], packet[29]);
         printf("Destination IP Address: %d.%d.%d.%d \n", packet[30], packet[31], packet[32], packet[33]);
      }
      
      /*이것은 TCP 헤더이다.
      소스 포트와 목적지 포트를 보여준다.
      현재 피킷에서 54를 더해 데이터를 보여준다.*/

      printf("다음은 TCP 헤더 정보이다. \n");
      printf("Source Port: %d", packet[34]*256+packet[35]);
      printf("Destination Port: %d", packet[36]*256+packet[37]);

      int DP = packet + 54;
      printf("데이터는 %s", DP)
   }

      /*세션 종료!*/
      pcap_close(handle);
      return(0);
   
}
