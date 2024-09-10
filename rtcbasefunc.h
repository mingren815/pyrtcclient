#pragma once
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>

//audio proxy
long timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}




