#include"stream_packet_process.h"
#include <stdio.h>

#define TAG "STREAM_PACKET_PROCESS"

unsigned short get_ts_pid(StreamBuffer *buf){
	
	if(streambuffer_read(buf,0)!=TS_SYNC_BYTE || buf->available_read_size<4){
		printf("%s: Not found td header\n",TAG);
		return -1;
	}
	
	unsigned short pid;
		
	pid=streambuffer_read(buf,1)&0x1f;
	pid=pid<<8;
	pid=pid^streambuffer_read(buf,2);
	
	return pid;

}

int is_continuous_zero(StreamBuffer *buf,int p){
	
	if(streambuffer_read(buf,p) == 0x00 && streambuffer_read(buf,p+1) == 0x00 ){
		
		
		if(streambuffer_read(buf,p+2) == 0x01 )
			return p+3;
		else if(streambuffer_read(buf,p+2) == 0x00 && streambuffer_read(buf,p+3) == 0x01)
			return p+4;
	
	}
	
	return -1;
}

int ts_packet_init(StreamBuffer *buf,Ts_packet **packet){
	
	
	if(buf->available_read_size<TS_PACKET_SIZE){
		printf("%s: NO Data\n",TAG);
		return -1;
	}
	
	if(streambuffer_read(buf,0)!=TS_SYNC_BYTE){
		printf("%s: Not found ts sync byte\n",TAG);
		return -1;
	}
		
	Ts_packet *p=malloc(sizeof(Ts_packet));
		
	p->ts_header.transport_error_indicator=(streambuffer_read(buf,1)&0x80) >> 7;
	p->ts_header.payload_unit_start_indicator=(streambuffer_read(buf,1)&0x40) >> 6;
	p->ts_header.transport_priority=(streambuffer_read(buf,1)&0x20) >> 5;
	p->ts_header.pid=(streambuffer_read(buf,1)&0x1f) >> 4;
	p->ts_header.pid=p->ts_header.pid<<8;
	p->ts_header.pid=p->ts_header.pid^streambuffer_read(buf,2);
	p->ts_header.transport_scrambling_control=(streambuffer_read(buf,3)&0xC0) >> 6;
	p->ts_header.adaptation_field_control=(streambuffer_read(buf,3)&0x30) >> 4;
	p->ts_header.continuity_counter=streambuffer_read(buf,3)&0x0f;
	p->adapt.flag=0;
	p->pes_header.flag=0;
	p->payload.flag=0;
	
	int flag_adapt=(p->ts_header.adaptation_field_control&0x02) >> 1;
	int flag_payload=p->ts_header.adaptation_field_control&0x01;
	
	//printf("%s:%x %x flag_adapt %d flag_payload %d\n",TAG,streambuffer_read(buf,3),p->ts_header.adaptation_field_control,flag_adapt,flag_payload);
	
	if(flag_adapt){
		
		p->adapt.flag=1;
		p->adapt.adaptation_field_length=streambuffer_read(buf,4);
		p->adapt.flag_pcr=streambuffer_read(buf,5);
		
		if(p->adapt.flag_pcr == 0x50){
			
			p->adapt.PCR[0]=streambuffer_read(buf,6);
			p->adapt.PCR[1]=streambuffer_read(buf,7);
			p->adapt.PCR[2]=streambuffer_read(buf,8);
			p->adapt.PCR[3]=streambuffer_read(buf,9);
			p->adapt.PCR[4]=streambuffer_read(buf,10);
		}
			
	}
	
	if(flag_payload){
		
		int payload_offset=4;
		
		if(flag_adapt)
			payload_offset=payload_offset+p->adapt.adaptation_field_length+1;
		
		int zero_p=is_continuous_zero(buf,payload_offset);
		//printf("%s:payload_offset %d zero p %d\n",TAG,payload_offset,zero_p);
		
		if(zero_p>0){
			
			int stream_id=streambuffer_read(buf,zero_p);
		
			if(stream_id>=0xc0 && stream_id<=0xef){
				p->pes_header.flag=1;
				p->pes_header.stream_id=stream_id;
				p->pes_header.packet_length=streambuffer_read(buf,zero_p+1);
				p->pes_header.packet_length=p->pes_header.packet_length<<8;
				p->pes_header.packet_length=p->pes_header.packet_length^streambuffer_read(buf,zero_p+2);
				p->pes_header.flag_priority=streambuffer_read(buf,zero_p+3);
				p->pes_header.flag_pts=streambuffer_read(buf,zero_p+4);
				p->pes_header.data_length=streambuffer_read(buf,zero_p+5);
				payload_offset=zero_p+5+p->pes_header.data_length+1;
				if(p->pes_header.flag_pts ==0x80 || p->pes_header.flag_pts ==0xc0){
					
					p->pes_header.pts[0]=streambuffer_read(buf,zero_p+6);
					p->pes_header.pts[1]=streambuffer_read(buf,zero_p+7);
					p->pes_header.pts[2]=streambuffer_read(buf,zero_p+8);
					p->pes_header.pts[3]=streambuffer_read(buf,zero_p+9);
					p->pes_header.pts[4]=streambuffer_read(buf,zero_p+10);
					
					if(p->pes_header.flag_pts ==0xc0) {
						
						p->pes_header.dts[0]=streambuffer_read(buf,zero_p+11);
						p->pes_header.dts[1]=streambuffer_read(buf,zero_p+12);
						p->pes_header.dts[2]=streambuffer_read(buf,zero_p+13);
						p->pes_header.dts[3]=streambuffer_read(buf,zero_p+14);
						p->pes_header.dts[4]=streambuffer_read(buf,zero_p+15);
						
					}
					
				}			
				//printf("%s: pes packet length %d\n",TAG,p->pes_header.packet_length);
			}
		}
		
		/////////////
		
		p->payload.flag=1;
		p->payload.payload_offset=payload_offset;
		p->payload.size=TS_PACKET_SIZE-payload_offset;
		p->payload.data=malloc(sizeof(char)*p->payload.size);
		//printf("%s:##### %d %d\n",TAG,payload_offset,p->payload.size);
		int i;
		
	
		/*
		for(i=0;i<10;i++){
			printf("%x ",streambuffer_read(buf,i));
		}
		printf("\n");
		*/
		for(i=0;i<p->payload.size;i++){
			
			p->payload.data[i]=streambuffer_read(buf,payload_offset+i);
			
		}
		//printf("%s: $$$$$ \n",TAG);
		
		//printf("%s: payload_offset %d %d stream_id %x %x\n",TAG,p->payload.payload_offset,p->payload.size,p->pes_header.flag,p->pes_header.stream_id);
	
	}
	
	
	
	
	*packet=p;
	
	return 1;

}

int get_ts_packet(StreamBuffer *buf,Ts_packet **packet,unsigned short *filter,size_t filter_size){
	
	streambuffer_seek(buf,TS_SYNC_BYTE);
	//printf("%s: SYNC_BYTE address  %d\n",TAG,buf->r_ptr);
	//printf("%s: buf->available_read_size  %d\n",TAG,buf->available_read_size);
	int i;
	unsigned short pid;
	
	while(buf->available_read_size>TS_PACKET_SIZE){
		
		pid=get_ts_pid(buf);
		//printf("%s: PID  %d\n",TAG,pid);
		for(i=0;i<filter_size;i++){
			
			if(filter[i]==pid)	{
			
				ts_packet_init(buf,packet);
			
				//streambuffer_seek_to_next(buf,TS_SYNC_BYTE);
				streambuffer_clean(buf,TS_PACKET_SIZE); 

				return 1;
			}
		
		}
		streambuffer_seek_to_next(buf,TS_SYNC_BYTE);
	}
	
	printf("%s:Not found ts packet\n",TAG);
	
	return 0;
	
}



