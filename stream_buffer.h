#include <stdio.h>

typedef struct {
	
	unsigned char * buffer;
	unsigned int w_ptr;
	unsigned int r_ptr;
	size_t available_write_size;
	size_t available_read_size;
	size_t size;
	
}StreamBuffer;

int streambuffer_init(StreamBuffer **buf,size_t size);

int streambuffer_write(StreamBuffer *buf,unsigned char *data,size_t size);

unsigned char streambuffer_read(StreamBuffer *buf,int ptr);

unsigned char streambuffer_seek(StreamBuffer *buf,unsigned char toByte);

int streambuffer_clean(StreamBuffer *buf,size_t size);
