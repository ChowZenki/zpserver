#ifndef ST_MESSAGE_H
#define ST_MESSAGE_H

#pragma  pack (push,1)

#define SMARTLINK_UUID_LEN 24

typedef struct tag_smartlink_message{
    unsigned char uuid_from [SMARTLINK_UUID_LEN];
    unsigned char uuid_to [SMARTLINK_UUID_LEN];
    unsigned char message_size[2]; // an Int-16 size of this message block,message_size[0] is low 8 bits, 1 is height 8 bits
    unsigned char message_block[1];// an message-Block array, have message_size items.
} SMARTLINK_MSG;

#pragma pack(pop)

#endif // ST_MESSAGE_H
