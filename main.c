#include"stream_packet_process.h"
#include <stdio.h>
#define TAG "MAIN"
#define BUFFER_SIZE 188*5000
#define READ_BUFFER_SIZE 188*100

int *a;
Ts_packet *packet;

int main(){
	
	
	FILE *fp_r;
		
	fp_r = fopen( "test.ts","rb" );
	
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
		
		
		
		if(stream_buf->available_write_size>=READ_BUFFER_SIZE){

			//printf("!!!!!!!!!!!!!!!!! %d\n",++count);
			read_size=fread(buf, 1,READ_BUFFER_SIZE , fp_r);
			write_size=streambuffer_write(stream_buf,&buf[0],READ_BUFFER_SIZE);
			//printf("write_size %d read size %d \n",write_size,read_size );
			//printf("%s:**** available_write_size %d available_read_size %d\n",TAG,stream_buf->available_write_size,stream_buf->available_read_size); 
			//printf("write_ptr %d read ptr %d \n",stream_buf->w_ptr,stream_buf->r_ptr );
		}
		
		//printf("available size %d  %d\n",stream_buf->available_write_size,stream_buf->available_read_size );
		//streambuffer_clean(stream_buf,1);
		//printf("available size %d  %d\n",stream_buf->available_write_size,stream_buf->available_read_size );
		
		//streambuffer_seek(stream_buf,0x47);
		//printf("Value %x %x\n",streambuffer_read(stream_buf,0),stream_buf->buffer[0]);
		
		
		//get_ts_packet(stream_buf,&packet,filter,1);
		
		
		do {
			
		
			unsigned short filter[]={4001};
			
			if( get_ts_packet(stream_buf,&packet,filter,1)){
				
				//printf("%s: write.... \n",TAG);
				fwrite(&packet->payload.data[0], 1 ,packet->payload.size, fp_w );
				
			}else{
				//printf("%s:@@ available_write_size %d available_read_size %d\n",TAG,stream_buf->available_write_size,stream_buf->available_read_size);	
				break;
			}
			
			//printf("%s:!! available_write_size %d available_read_size %d\n",TAG,stream_buf->available_write_size,stream_buf->available_read_size); 
			//printf("!! write_ptr %d read ptr %d \n",stream_buf->w_ptr,stream_buf->r_ptr );
		}while(stream_buf->available_read_size>=188);
		//printf("%s: SYNC_BYTE address  %d\n",TAG,stream_buf->r_ptr);
		//printf("PID %x %x\n",packet->ts_header.pid,packet->ts_header.continuity_counter);
	
	}while(!feof( fp_r ) );

	
	
	
	
	fclose(fp_w);
	
	
	

	//get_ts_packet(stream_buf,&packet,filter,2);

	//printf("PID %x\n",packet->ts_header.pid);
	

	printf("%s\n",TAG);
	printf("unsigned char %d\n",sizeof(char));
	printf("unsigned short %d\n",sizeof(short));
	printf("unsigned int %d\n",sizeof(int));
	/*
	*/	
	printf("finish\n");
	//system("pause");

	
	return 1;
	
}
