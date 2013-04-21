#ifndef REQUEST_H
#define REQUEST_H

#define TRUE  1
#define FALSE 0

// Request alias
#define TYPE_CLI_HELP       1
#define TYPE_SERV_HELP_ANS  2

#define TYPE_CLI_ANS        8

// request type 0: bat dau ket noi tu cli

//         type 1: mcli xin quyen giai thoat 

//         type 2: serv tra loi quyen giai thoat 
//            num 1: thu nhat (so nguoi cung dap an)
//            num 2: tra loi quyen giai thoat thu hai  (dap an dung - dap an sai)
//            num 3: tra loi quyen giai thoat thu ba   (dap an duoc nhieu nguoi chon nhat)
 
//         type 8: cli gui tra loi cau hoi
//         type 9: serv gui cau hoi

//         type 10: main player tra loi truoc 99 ng cung choi
//         type 11: nguoi cung choi tra loi sai

//         type 12: main player chien thang
//         type 13: main player that bai

//         type 14: tiep tuc hay dung choi
//         type 15: main player chu dong dung choi
//         type 16: khong cho them nguoi cung choi choi vi da du so nguoi

typedef struct Request_t{
  int type;       // message type
  char mess[100]; // message string
  int num;        // message number
} Request;

int sendRequest(int sockfd, int type, char *mess, int num);
Request *recvRequest(int sockfd);

#endif
