#include <iostream>
#include <list>
#include <string>
#include <map>
#include "helpers/strings.h"
#include "helpers/pattern.h"
#include "mail_messages/MailTable.h"

const int MAX_SENT = 20;
const int MAX_MSG_LENGTH = 255;

using namespace std;

typedef std::list<std::string> Responses;
typedef std::map<std::string, std::string> Bindings;
typedef std::list<MailItem> MAIL_ITEMS;

void sendMessageHandler(Responses&, MailTable&, string, string);
void checkMessagesHandler(Responses&, MailTable&, string);

MAIL_ITEMS get_items_from_user(const MAIL_ITEMS &, std::string);
MAIL_ITEMS get_items_to_user(const MAIL_ITEMS &, std::string);
MAIL_ITEMS get_unread_items(const MAIL_ITEMS &, std::string);



Responses process_message(std::string from, std::string message)
{
  Responses responses;
  if (startsWith(message, "/postal help"))
    {
      std::cout << "Entered help.";
    }
  else if (startsWith(message, "/postal send"))
    {
   
    }
  else if (startsWith(message, "/postal sentlist"))
    {
      std::cout << "Entered sentlist.";
    }
  else if (startsWith(message, "/postal unsend"))
    {
      std::cout << "Entered unsend.";
    }
  else if (startsWith(message, "/postal read"))
    {
      std::cout << "Entered read.";
    }
  else if (startsWith(message, "/postal ignore"))
    {
      std::cout << "Entered ignore.";
    }
  else if (startsWith(message, "/postal ignorelist"))
    {
      std::cout << "Entered ignorelist.";
    }
  else if (startsWith(message, "/postal unignore"))
    {
      std::cout << "Entered unignore.";
    }
  return responses;
}

std::string process_room_entrance(std::string user);

//void print(const MAIL_ITEMS &);

int main(void)
{
  MailTable mail;
  Responses responses;
  
  string in;
  string out;
  while (true) {
    getline(cin, in);
    out = in;
    cout << out << endl;
  }
  return 0;
}
  /*
  MAIL_ITEMS items;
  std::string header("POSTAL - IM the offline.\nSend me a message and I'll give it to someone else when they join the room. I try to keep things private so please interact with me via IMs.\n");
  std::string help("Commands:"                                         "\n"
  "/postal help"                                                       "\n" 
  "/postal send [user] [message]"                                      "\n"
       "\tYou can only have 30 sent messages at any one time."         "\n"
  "/postal sentlist"                                                   "\n"
       "\tReturns a list of the sent messages and their IDs."          "\n"
  "/postal unsend [message_id]"                                        "\n"
       "\tReturns a list of unread sent messages."                     "\n"
  "/postal read"                                                       "\n"
  "/postal ignore [user]"                                              "\n"
  "/postal ignorelist"                                                 "\n"
  "/postal unignore [user]"                                            "\n");
  std::cout << help << std::endl; */




MAIL_ITEMS get_items_to_user(const MAIL_ITEMS & items, std::string user)
{
  MAIL_ITEMS items_to_return;
  for (MAIL_ITEMS::const_iterator iter = items.begin(); iter != items.end(); iter++)
  {
    if (iter->to == user)
    {
      items_to_return.push_back(*iter);
    }
  }
  return items_to_return;
}

void print(const MAIL_ITEMS &items)
{
  for (MAIL_ITEMS::const_iterator iter = items.begin(); iter != items.end(); iter++)
  {
    std::cout << iter->from << " ";
  }
}

void sendMessageHandler(Responses &responses, 
			MailTable &table, 
			string from, 
			string message) {
  if (MAX_SENT <= table.getMailFrom(from).size()) {
    responses.push_back("In order to prevent abuse, there is a limit on the "
      "number of messages which can be placed in escrow. You have reached "
      "this limit. Before you can send additional messages you must either "
      "(a) wait for your messages to be read or (b) use the '/postal unsend' "
      " command to remove a message from escrow.");
    return;
  }
  Bindings binds = matchPattern("/postal send ?user ?*body", message);
  if (isFailedMatch(binds)) {
    responses.push_back("Postal expects messages in the form '/postal send "
     "<user> <message ...>'.");
    return;
  }
  string to = binds.find("?user")->second;
  string body = binds.find("?body")->second;
  if (MAX_MSG_LENGTH <= body.length()) {
    responses.push_back("In order to prevent abuse, there is a limit on the "
      "length of messages. You have passed this limit. This postal system is "
      "more like twitter than email.");
    return;
  }
  table.addMailItem(from, to, body);
  return;
}

void checkMessagesHandler(Responses& responses, MailTable&table, string user) {
  int size = table.getMailTo(user).size();
  if (size) {
    responses.push_back(user + " you have " + size + " unread message(s). To "
      " read a message you can use '/postal read'");
  }
  return;
}
