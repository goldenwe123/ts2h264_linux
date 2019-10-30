# ts2h264_linux
stream_buffer.c 52 line have a bug
write_nmemb=min(buf->available_write_size,size-write_count);
