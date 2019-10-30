#include"stream_buffer.h"
#define TAG "STREAM_BUFFER"
#include <stdio.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
     
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
	 
	 
int streambuffer_init(StreamBuffer **buf,size_t size){
	
	StreamBuffer *p=NULL;
	
	p=malloc(sizeof(StreamBuffer));
	p->buffer=calloc(size,sizeof(char));
	p->size=size;
	p->available_write_size=size;
	p->available_read_size=0;
	p->w_ptr=0;
	p->r_ptr=0;
	
	*buf=p;
	
	return 1;
}	   

int streambuffer_write(StreamBuffer *buf,unsigned char *data,size_t size){
	

	size_t write_nmemb;
	size_t write_count=0;
	int p=0;
	
	if(size> buf->available_write_size){
		printf("%s buffer not enough space\n",TAG);
		return -1;
	}else if(size <0){
		printf("%s size  error\n",TAG);
		return -1;	
	}		
		
	do{
		
		//write_nmemb=min(buf->size-buf->w_ptr,size-write_count);
		//write_nmemb=min(write_nmemb,buf->available_write_size);
		write_nmemb=min(buf->available_write_size,size-write_count);
		memcpy(&buf->buffer[buf->w_ptr], &data[p], write_nmemb);
		write_count+=write_nmemb;
		buf->available_write_size=buf->available_write_size-write_nmemb;
		buf->available_read_size=buf->size-buf->available_write_size;
		buf->w_ptr=(buf->w_ptr+write_count)%buf->size;
		p+=write_count;
		
	}while(write_count!=size && buf->available_write_size !=0);
	
	return write_count;	
}

unsigned char streambuffer_read(StreamBuffer *buf,int ptr){
	
	if(ptr > buf->available_read_size-1|| ptr<0){
		printf("%s Out of read range\n",TAG);
		return -1; 
	}
		
	int p=(buf->r_ptr+ptr)%buf->size;
	
	return buf->buffer[p];
}

int streambuffer_clean(StreamBuffer *buf,size_t size){
	
	buf->available_write_size+=size;
	buf->available_read_size=buf->size-buf->available_write_size;
	buf->r_ptr=(buf->r_ptr+size)%buf->size;

}

unsigned char streambuffer_seek(StreamBuffer *buf,unsigned char toByte){
	

	int i;
	
	for(i=0;i<buf->available_read_size;i++) {
	
	
	 	if(streambuffer_read(buf,i)==toByte){
	 		
			streambuffer_clean(buf,i);
			return i;
		}
	}
 	streambuffer_clean(buf,i);
	printf("%s: Not found %x byte\n",TAG,toByte);
		
	return -1;
	
}

unsigned char streambuffer_seek_to_next(StreamBuffer *buf,unsigned char toByte){
	

	int i;
	
	for(i=1;i<buf->available_read_size;i++) {
	
	
	 	if(streambuffer_read(buf,i)==toByte){
	 		
			streambuffer_clean(buf,i);
			return i;
		}
	}
 	streambuffer_clean(buf,i);
	printf("%s:Next Not found %x byte\n",TAG,toByte);
		
	return -1;
	
}
