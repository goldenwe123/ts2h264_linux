#include"stream_packet_process.h"
#include <stdio.h>
#define TAG "MAIN"
#define BUFFER_SIZE 188*5000
#define READ_BUFFER_SIZE 188*100

int *a;
Ts_packet *packet;

int main(){
	
	
	FILE *fp_r;
		
	fp_r = fopen( "demo.ts","rb" );
	
	if (fp_r == NULL){
		
		printf("%s open file fail\n",TAG);
		return -1;
	
	}
	
	StreamBuffer *stream_buf;
	
	unsigned char buf[READ_BUFFER_SIZE];
	streambuffer_init(&stream_buf,BUFFER_SIZE);
	size_t read_size;
	size_t write_size;
	FILE *fp_w = fopen( "out.h264","w+b" );
	int count=0;
	do{
		
		if(stream_buf->available_write_size>=READ_BUFFER_SIZE) {

			read_size=fread(buf, 1,READ_BUFFER_SIZE , fp_r);
			write_size=streambuffer_write(stream_buf,&buf[0],READ_BUFFER_SIZE);
		
		}
	
		do {
			
			unsigned short filter[]={4001};
			
			if( get_ts_packet(stream_buf,&packet,filter,1)){
				
				fwrite(&packet->payload.data[0], 1 ,packet->payload.size, fp_w );
				
			}else{
				
				break;
			}
				
		}while(stream_buf->available_read_size>=188);
	
	}while(!feof( fp_r ) );
	
	fclose(fp_w);
	
	
		
	return 1;
	
}
