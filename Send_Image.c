#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define UDP_PORT 12345
#define SERVER_IP "127.0.0.1"

int main() {
    // Kamera açma
    cv::VideoCapture cap(0);  // 0, ilk kamera
    if (!cap.isOpened()) {
        printf("Kamera açılamadı!\n");
        return -1;
    }

    // Fotoğraf çekme
    cv::Mat frame;
    cap >> frame; // Fotoğrafı al
    if (frame.empty()) {
        printf("Fotoğraf çekilemedi!\n");
        return -1;
    }

    // Fotoğrafı JPEG formatında kodlama
    std::vector<uchar> buf;
    cv::imencode(".jpg", frame, buf);
    
    // UDP soketi oluşturma
    int sockfd;
    struct sockaddr_in server_addr;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket oluşturulamadı");
        return -1;
    }

    // Sunucu bilgilerini ayarlama
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Fotoğrafı gönderme
    ssize_t sent_bytes = sendto(sockfd, buf.data(), buf.size(), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0) {
        perror("Veri gönderilemedi");
        close(sockfd);
        return -1;
    }

    printf("Fotoğraf başarıyla gönderildi!\n");

    // Soketi kapatma
    close(sockfd);
    return 0;
}