//
// Created by aniket on 4/28/25.
//

#ifndef TYPES_H
#define TYPES_H

#include<stddef.h>
#include<string>
#include<functional>
#include<memory.h>
#include<iostream>
#include<stdio.h>

namespace cppsock{
	enum DISCONNECT_REASON{
  		UNKNOWN
	};
	typedef std::string cppsock_str;

    typedef std::function<void(size_t /*client_id*/,const std::string& /*message*/)> message_callback_t;
    typedef std::function<void(size_t /*client_id*/)> connect_callback_t;
    typedef std::function<void(size_t /*client_id*/ ,DISCONNECT_REASON)> disconnect_callback_t;
    typedef std::function<void(size_t  )> error_callback_t;
}

#endif //TYPES_H
