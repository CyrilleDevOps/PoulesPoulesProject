
#define MAIL_SERVER "smtp.googlemail.com"
#define MAIL_PORT "587"

#define ACTIVE_MAIL 1


#define SENDER_MAIL "cyrille.devops@gmail.com"
#define SENDER_PASSWORD "yljc wrcv qsop wmyg"

#define FROM_MAIL "Les Poules"
#define RECIPIENT_MAIL "cyrille.devops@gmail.com"



//#define SERVER_USES_STARTSSL 1

#define TASK_STACK_SIZE     (8 * 1024)
#define BUF_SIZE            1024


#define TAG_MAIL "Poules Mails"


struct struct_message_mail {
  char from[50];
  char subject[50];
  char to[50];
  char body[100];
  int  ack;
};

typedef  struct struct_message_mail message_mail;

void smtp_client_task(void *pvParameters);
void Affiche_Mail_Content(char *Etape,message_mail *Mymessage);
void Poules_Mail (message_mail *message);
void Poules_Mail_content (char *subject,char *body);