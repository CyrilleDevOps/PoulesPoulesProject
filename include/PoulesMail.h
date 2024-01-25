// Poiles H

#define MAIL_SERVER "smtp.googlemail.com"
#define MAIL_PORT "587"

#define SENDER_MAIL "cyrille.devops@gmail.com"
#define SENDER_PASSWORD "C6l2d11F3@gmail"

#define RECIPIENT_MAIL "cyrille.devops@gmail.com"

#define EMAIL_SUBJECT "Subject: ESP32 Email Test\r\n"
#define EMAIL_BODY "Hello from ESP32!\r\n"



#define SERVER_USES_STARTSSL 1

#define TASK_STACK_SIZE     (8 * 1024)
#define BUF_SIZE            512


#define TAG_MAIL "Poules Mails"


void smtp_client_task(void *pvParameters);