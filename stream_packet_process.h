#include"stream_packet.h"
#include"stream_buffer.h"

unsigned short get_ts_pid(StreamBuffer *buf);

int ts_packet_init(StreamBuffer *buf,Ts_packet **packet);

int get_ts_packet(StreamBuffer *buf,Ts_packet **packet,unsigned short *filter,size_t filter_size);

/*
int ts_packet_init(StreamBuffer *buf,Ts_packet **packet);

int streambuffer_init(StreamBuffer **buf,size_t size);

int streambuffer_write_data(StreamBuffer *buf,unsigned char * data,size_t size);

void streambuffer_clean_data(StreamBuffer *buf,int p);

int seek_to_sync_byte(StreamBuffer *buf);

int get_ts_packet(StreamBuffer *buf,Ts_packet **packet,unsigned short *filter,size_t size);
	
*/	
